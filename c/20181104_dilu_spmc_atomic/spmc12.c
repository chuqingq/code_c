// gcc -g -o producer -DPRODUCER spmc12.c -pthread -Wall; gcc -g -o consumer spmc12.c -pthread -Wall
// ./producer &; ./consumer

// gcc -o spmc12 spmc12.c -pthread -Wall
// ./spmc12

// 1.改为atomic实现读写锁
// 2.xsi接口改为posix接口，易于管理
// 3.
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define ENTRY_NUM_MAX 64

typedef struct
{
    int key;      // for debug
    int size;     // 在使用的entry数量，只增不减，<=cap。目的是避免consumer读到从没写过的entry
    int capacity; // entry_capacity
    int buf_size; // 每个entry的大小

    int write_pos; // producer下次将要写入的位置。只有producer访问，无需原子
    int read_pos;  // producer最近写入成功的位置，即消费者需要读取的位置。是producer和consumer同步的关键

    // int nconsumers[ENTRY_NUM_MAX]; // 每个entry有多少个consumer在读，-1表示正在被producer写入。不用于同步，可以用relaxed
    pthread_rwlock_t rwlocks[ENTRY_NUM_MAX];
    uint8_t buf[0];                // capacity*buf_size
} spmc_t;

// bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...)
// #define CAS(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)

// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
// #define LOAD(ptr) __sync_sub_and_fetch(ptr, 0)

void *spmc_init(int key, int entry_num, int buf_size, int is_producer)
{
    if (entry_num <= 0 || entry_num > ENTRY_NUM_MAX || buf_size <= 0)
    {
        fprintf(stderr, "ERROR entry_num[%d] or buf_size[%d] is invalid\n", entry_num, buf_size);
        exit(-1);
    }

    // 改为shm
    int shmid = shmget((key_t)key, sizeof(spmc_t) + entry_num * buf_size, 0666 | IPC_CREAT);
    if (shmid == -1)
    {
        fprintf(stderr, "shmget failed\n");
        exit(-1);
    }

    // 将共享内存连接到当前进程的地址空间
    void *shm = shmat(shmid, (void *)0, 0);
    if (shm == (void *)-1)
    {
        fprintf(stderr, "shmat failed\n");
        exit(-1);
    }
    spmc_t *spmc = (spmc_t *)shm;

    if (is_producer)
    {
        spmc->key = key;
        spmc->size = 0;
        spmc->capacity = entry_num;
        spmc->buf_size = buf_size;
        spmc->write_pos = 0;
        spmc->read_pos = 0;

        for (int i = 0; i < spmc->capacity; i++)
        {
            // spmc->nconsumers[i] = 0;
            pthread_rwlock_init(&spmc->rwlocks[i], NULL);
        }
    }

    return (void *)spmc;
}

void spmc_free(void *s)
{
    spmc_t *spmc = (spmc_t *)s;
    if (spmc == NULL)
    {
        return;
    }

    free(spmc);
}

// 生产者

// 生产者申请第一个能够写入的entry
uint8_t *producer_alloc(void *s)
{
    spmc_t *spmc = (spmc_t *)s;
    for (int i = spmc->write_pos;; i = (i + 1) % spmc->capacity)
    {
        // // 如果i这个entry有消费者在读，则跳过
        // int old = spmc->nconsumers[i];
        // // int old = LOAD(&spmc->nconsumers[i]); // 改成LOAD后慢一秒
        // if (old != 0)
        // {
        //     continue;
        // }

        // if (CAS(&spmc->nconsumers[i], 0, -1)) // TODO 这里只需relaxed，无需barrier // TODO 这里应该派出掉i是read_pos的可能
        // int val = 0;
        // if (__atomic_compare_exchange_n(&spmc->nconsumers[i], &val, -1, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
        if (!pthread_rwlock_tryrdlock(&spmc->rwlocks[i]))
        {
            if (spmc->size <= i)
            {
                spmc->size = i + 1;
            }
            uint8_t *buf = spmc->buf + i * spmc->buf_size;
            spmc->write_pos = (i + 1) % spmc->capacity; // 下次就从下一个位置开始尝试写入
            return buf;
        }
    }

    // unreachable
    return NULL;
}

// 生产者写入后释放写锁
void producer_alloc_end(spmc_t *spmc, const uint8_t *buf)
{
    // 计算entry的位置
    int i = (buf - spmc->buf) / spmc->buf_size;

#ifdef DEBUG
    // printf("debug\n");
    // 大约0.3秒的影响：1.7到2.0
    int old = __atomic_exchange_n(&spmc->nconsumers[i], 0, __ATOMIC_RELAXED);
    if (old != -1) // DEBUG
    {
        printf("ERROR producer_alloc_end unexpected: old nconsumers not -1\n");
        exit(-1);
    }
#else
    // __atomic_store_n(&spmc->nconsumers[i], 0, __ATOMIC_RELAXED);
    pthread_rwlock_unlock(&spmc->rwlocks[i]);
#endif

    __atomic_store_n(&spmc->read_pos, i, __ATOMIC_RELEASE); // 此处优化针对producer单独运行降低0.5秒
}

// 消费者

// 对申请的entry的buf只读
uint8_t *consumer_alloc(void *s)
{
    spmc_t *spmc = (spmc_t *)s;

    while (1)
    {
        // 应该读取的位置
        int i = __atomic_load_n(&spmc->read_pos, __ATOMIC_ACQUIRE);

        // int old = __atomic_load_n(&spmc->nconsumers[i], __ATOMIC_RELAXED);
        // if (old < 0)
        // { 
        //     // 已被生产者占用
        //     continue;
        // }

        // if (CAS(&spmc->nconsumers[i], old, old + 1)) // TODO 这里可以是relaxed，无需barrier
        // if (__atomic_compare_exchange_n(&spmc->nconsumers[i], &old, old+1, 0, __ATOMIC_RELAXED, __ATOMIC_RELAXED))
        if (!pthread_rwlock_tryrdlock(&spmc->rwlocks[i]))
        {
            uint8_t *buf = spmc->buf + i * spmc->buf_size;
            return buf;
        }
    }

    // unreachable
    return NULL;
}

// 读取完成后解锁
void consumer_alloc_end(spmc_t *spmc, const uint8_t *buf)
{
    int i = (buf - spmc->buf) / spmc->buf_size;
    
    // 直接原子减一即可
    // int new = __atomic_sub_fetch(&spmc->nconsumers[i], 1, __ATOMIC_RELAXED);
    // if (new < 0) // 
    // {
    //     printf("ERROR consumer_alloc_end unexpected: new nconsumers < 0\n");
    //     exit(-1);
    // }
    pthread_rwlock_unlock(&spmc->rwlocks[i]);
}
