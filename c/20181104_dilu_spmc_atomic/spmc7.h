#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>

#define ENTRY_NUM_MAX 64

// typedef struct {
//     // pthread_rwlock_t lock;
//     // pthread_rwlockattr_t attr;
//     int nconsumers; // 消费者数量，0表示未使用，-1表示生产者使用
//     // int size;
//     // int capacity;

//     //int flagWrite; // for debug
//     // uint8_t buf[BUF_SIZE];
// } spmc_entry_t;

typedef struct {
    int key;  // for debug
    int size; // entry_size
    int capacity; // entry_capacity
    int buf_size;

    int write_pos; // 正在写入的位置
    volatile int read_pos; // 最近写入成功的位置，即消费者需要读取的位置

    int nconsumers[ENTRY_NUM_MAX];
    uint8_t buf[0]; // capacity*buf_size
} spmc_t;

void *spmc_init(int key, int entry_num, int buf_size, int is_producer);
void spmc_free(void *s);

uint8_t *producer_alloc(void *s);
void producer_alloc_end(spmc_t *spmc, const uint8_t *buf);

uint8_t *consumer_alloc(void *s);
void consumer_alloc_end(spmc_t *spmc, const uint8_t *buf);
