// gcc -g -o spmc spmc.c -pthread -Wall
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "spmc6.h"

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

// http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
// bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...)
#define cas(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)

void *spmc_init(int key, int entry_num, int buf_size, int is_producer) {
    if (entry_num > ENTRY_NUM_MAX) {
        printf("ERROR entry_num > ENTRY_NUM_MAX\n");
        exit(-1);
    }

    // 改为shm
    int shmid = shmget((key_t)key, sizeof(spmc_t)+entry_num*buf_size, 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(-1);
	}

	//将共享内存连接到当前进程的地址空间
	void *shm = shmat(shmid, (void*)0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(-1);
	}
    spmc_t *spmc = (spmc_t *)shm;

    if (is_producer) {
        spmc->key = key;
        spmc->size = 0;
        spmc->capacity = entry_num;
        spmc->buf_size = buf_size;

        for (int i = 0; i < spmc->capacity; i++) {
            spmc->nconsumers[i] = 0;
        }
    }

    // printf("spmc_init: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
    return (void *)spmc;
}

void spmc_free(void *s) {
    spmc_t *spmc = (spmc_t *)s;
    if (spmc == NULL) {
        return;
    }

    free(spmc);
}

// 生产者

// 生产者申请第一个能够写入的entry
uint8_t *producer_alloc(void *s) {
    spmc_t *spmc = (spmc_t *)s;
    for (int i = spmc->write_pos; ; i = (i+1) % spmc->capacity) {
        int old = spmc->nconsumers[i];
        if (old != 0) {
            continue;
        }

        if (cas(&spmc->nconsumers[i], 0, -1)) {
            if (spmc->size <= i) {
                spmc->size = i+1;
            }
            uint8_t *buf = spmc->buf+i*spmc->buf_size;
            printf("producer_alloc: buf: %p\n", buf);
            spmc->write_pos = (i+1) % spmc->capacity;
            return buf;
        }
    }

    // unreachable
    return NULL;
}

// 生产者写入后释放写锁
void producer_alloc_end(spmc_t *spmc, const uint8_t *buf) {
    int i = (buf-spmc->buf)/spmc->buf_size;
    // printf("producer_alloc_end: buf: %d, %p\n", i, buf);
    if (!cas(&spmc->nconsumers[i], -1, 0)) {
        printf("ERROR unexpected\n");
        exit(-1);
    }
}

// 消费者

// 对申请的entry的buf只读
uint8_t *consumer_alloc(void *s) {
    spmc_t *spmc = (spmc_t *)s;

    // spmc_entry_t *entry;
    for (int i = spmc->read_pos; ; i = (i+1) % spmc->size) {\
        int old = spmc->nconsumers[i];
        if (old < 0) { // 已被生产者占用
            continue;
        }

        if (cas(&spmc->nconsumers[i], old, old+1)) {
            uint8_t *buf = spmc->buf+i*spmc->buf_size;
            // printf("consumer_alloc: %d, %d, %d, %d, %d, %d\n", i, spmc->read_pos, spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
            while (1) {
                int old = spmc->read_pos;
                if (cas(&spmc->read_pos, old, (old+1)%spmc->size)) {
                    break;
                }
            }
            return buf;
        }
    }

    // unreachable
    return NULL;
}

// 读取完成后解锁
void consumer_alloc_end(spmc_t * spmc, const uint8_t *buf) {
    int i = (buf-spmc->buf)/spmc->buf_size;
    while (1) {
        int old = spmc->nconsumers[i];
        if (cas(&spmc->nconsumers[i], old, old-1)) {
            return;
        }
    }
}

// #if 0
/////////////////////////// 测试 ////////////////////////////////////////////////////////
spmc_t *spmc;
const int count = 200000000;

void* produce(void *not_used) {
    for (int i = 0; i < count; i++) {
        uint8_t *buf = producer_alloc(spmc);
        if (buf == NULL) {
            printf("ERROR producer_alloc NULL!!!\n");
            exit(-1); // 不应该返回NULL
        }

        buf[0] = (uint8_t)i;
        usleep(70000);
        // printf("produce: %p\n", &buf[0]);
        
        //consumer_alloc_end(buf);
        printf("produce: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
		producer_alloc_end(spmc, buf);
        printf("produce: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
    }

    return NULL;
}

void* consume(void *not_used) {
    int i = 0;
    uint8_t old = 10;
    for (; i < 10000; ) {
        uint8_t *buf = consumer_alloc(spmc);
        if (buf == NULL) {
            continue;
        }

        // printf("consume: %d %p %d\n", buf[0], &buf[0], i);

        if (old != buf[0]) {
            printf("consume: %d %p %d\n", buf[0], &buf[0], i);
            old = buf[0];
            i++;
        }

        usleep(100000);
        printf("consume: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
        consumer_alloc_end(spmc, buf);
        printf("consume: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
    }

    return NULL;
}

#define SHM_KEY 12310

int main_p() {
    spmc = (spmc_t*)spmc_init(SHM_KEY, 4, 10, 1);
    produce(NULL);
    return 0;
}

int main_c() {
    spmc = (spmc_t*)spmc_init(SHM_KEY, 4, 10, 0);
    // spmc = (spmc_t*)spmc_init(SHM_KEY, 4, 10, 1);
    consume(NULL);
    return 0;
}

int main() {
    return main_p();
    // return main_c();
}
// #endif
