// gcc -g -o spmc spmc.c -pthread -Wall
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "spmc5.h"

#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})

	
void *spmc_init(int key, int entry_num, int buf_size, int is_producer) {
    // spmc_t *spmc = (spmc_t *)malloc(sizeof(spmc_t)+entry_num*(sizeof(spmc_entry_t)+buf_size));
    // if (spmc == NULL) {
    //     printf("spmc_init: malloc error\n");
    //     return NULL;
    // }

    // 改为shm
    int shmid = shmget((key_t)key, sizeof(spmc_t)+entry_num*(sizeof(spmc_entry_t)+buf_size), 0666|IPC_CREAT);
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
        spmc->size = 0;
	spmc->key = key;
        spmc->capacity = entry_num;

        for (int i = 0; i < spmc->capacity; i++) {
            // pthread_rwlockattr_init(&spmc->entries[i].attr);
            // pthread_rwlockattr_setpshared(&spmc->entries[i].attr, PTHREAD_PROCESS_SHARED);

            int ret = pthread_rwlock_init(&spmc->entries[i].lock, NULL); // &spmc->entries[i].attr
            if (ret != 0) {
                printf("pthread_rwlock_init error\n");
                free(spmc);
                return NULL;
            }

            spmc->entries[i].size = 0;
            spmc->entries[i].capacity = buf_size;
        }
    }

    return (void *)spmc;
}

void spmc_free(void *s) {
    spmc_t *spmc = (spmc_t *)s;
    if (spmc == NULL) {
        return;
    }

    for (int i = 0; i < spmc->capacity; i++) {
        pthread_rwlockattr_destroy(&spmc->entries[i].attr);
        // pthread_rwlock_destroy(&spmc->entries[i].lock);
    }

    free(spmc);
}

// 生产者

// 生产者申请第一个能够写入的entry
uint8_t *producer_alloc(void *s) {
    spmc_t *spmc = (spmc_t *)s;
    int ret;
    while (1) {
        for (int i = 0; i < spmc->capacity; i++) {
	    printf("aaaaaa%d, %d\n", spmc->capacity, i);
            ret = pthread_rwlock_trywrlock(&spmc->entries[i].lock);
            if (ret == 0) {
                // 更新size，使用的entry数量。只增不减
                if (spmc->size <= i) {
                    spmc->size = i+1;
                }
                // printf("producer_alloc index: %d\n", i);
                return spmc->entries[i].buf;
            } else {
                printf("producer_alloc trywrlock error: %d key 0x%x\n", i, spmc->key);
            }
        }
    }

    return NULL;
}

// 生产者写入后释放写锁
void producer_alloc_end(const uint8_t *buf) {
    spmc_entry_t *entry = container_of(buf, spmc_entry_t, buf[0]);
    pthread_rwlock_unlock(&entry->lock);
}

// 消费者

// 对申请的entry的buf只读
uint8_t *consumer_alloc(void *s) {
    spmc_t *spmc = (spmc_t *)s;
    int ret;
    for (int i = 0; i < spmc->size; i++) {
        ret = pthread_rwlock_tryrdlock(&spmc->entries[i].lock);
        if (ret == 0) {
            printf("consumer_alloc index: %d key 0x%x\n", i, spmc->key);
            return spmc->entries[i].buf;
        } else {
            printf("consumer_alloc tryrdlock error: %d key 0x%x\n", i, spmc->key);
	    if (ret != EBUSY) {
		pthread_rwlock_unlock(&spmc->entries[i].lock);
            }
        }
    }

    return NULL;
}

// 读取完成后解锁
void consumer_alloc_end(const uint8_t *buf) {
    spmc_entry_t *entry = container_of(buf, spmc_entry_t, buf[0]);
    pthread_rwlock_unlock(&entry->lock);
}



// #if 0
/////////////////////////// 测试 ////////////////////////////////////////////////////////
spmc_t *spmc;
const int count = 20000000;

void* consume(void *not_used) {
    int i = 0;
    uint8_t old = 10;
    for (; i < 10000; ) {
        uint8_t *buf = consumer_alloc(spmc);
        if (buf == NULL) {
            continue;
        }

        if (old != buf[0]) {
            printf("consume: %d %p %d\n", buf[0], &buf[0], i);
            old = buf[0];
            i++;
        }

        // usleep(100000);
        consumer_alloc_end(buf);
    }

    return NULL;
}

void* produce(void *not_used) {
    for (int i = 0; i < count; i++) {
        uint8_t *buf = producer_alloc(spmc);
        if (buf == NULL) {
            printf("ERROR producer_alloc NULL!!!\n");
            return NULL;
        }

        buf[0] = (uint8_t)i;
        // usleep(70000);
        printf("produce: %d %p %d\n", buf[0], &buf[0], i);
        //consumer_alloc_end(buf);
		producer_alloc_end(buf);
    }

    return NULL;
}

#define SHM_KEY 12307

int main_p() {
    spmc = (spmc_t*)spmc_init(SHM_KEY, 4, 10, 1);
    produce(NULL);
    return 0;
}

int main_c() {
    spmc = (spmc_t*)spmc_init(SHM_KEY, 4, 10, 0);
    consume(NULL);
    return 0;
}

int main() {
    // return main_p();
    return main_c();
}
// #endif
