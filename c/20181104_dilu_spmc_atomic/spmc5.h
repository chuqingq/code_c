#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    pthread_rwlock_t lock;
    pthread_rwlockattr_t attr;
    int size;
    int capacity;

    //int flagWrite; // for debug
    uint8_t buf[0];
} spmc_entry_t;

typedef struct {
    int size;
    int capacity;
    int key;  // for debug

    spmc_entry_t entries[0];
} spmc_t;

void *spmc_init(int key, int entry_num, int buf_size, int is_producer);
void spmc_free(void *s);

uint8_t *producer_alloc(void *s);
void producer_alloc_end(const uint8_t *buf);

uint8_t *consumer_alloc(void *s);
void consumer_alloc_end(const uint8_t *buf);
