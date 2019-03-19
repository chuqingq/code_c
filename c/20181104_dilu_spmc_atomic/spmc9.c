// gcc -g -o producer -DPRODUCER spmc8.c -pthread -Wall; gcc -g -o consumer spmc8.c -pthread -Wall
// ./producer &; ./consumer
// spmc7 生产者写入成功后记录新写入的位置，消费者只读取最新写入的位置
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

    int write_pos; // 正在写入的位置
    int read_pos;  // 最近写入成功的位置，即消费者需要读取的位置

    int nconsumers[ENTRY_NUM_MAX]; // 每个entry有多少个consumer在读，-1表示正在被producer写入
    uint8_t buf[0];                // capacity*buf_size
} spmc_t;

// http://gcc.gnu.org/onlinedocs/gcc-4.1.2/gcc/Atomic-Builtins.html
// bool __sync_bool_compare_and_swap (type *ptr, type oldval type newval, ...)
#define CAS(ptr, oldval, newval) __sync_bool_compare_and_swap(ptr, oldval, newval)

// https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html
#define LOAD(ptr) __sync_sub_and_fetch(ptr, 0)

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
            spmc->nconsumers[i] = 0;
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

        if (CAS(&spmc->nconsumers[i], 0, -1)) // TODO 这里只需relaxed，无需barrier
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

    // 释放写锁，不应该失败
    // if (!CAS(&spmc->nconsumers[i], -1, 0)) // TODO 这里直接设置成0即可，无需barrier
    // {
    //     printf("ERROR unexpected\n");
    //     exit(-1);
    // }
    __atomic_store_n(&spmc->nconsumers[i], 0, __ATOMIC_RELAXED);

    // 写入成功后设置read_pos
    // while (1)
    // {
    //     // int old = spmc->read_pos;
    //     int old = LOAD(&spmc->read_pos);
    //     if (CAS(&spmc->read_pos, old, i))
    //     {
    //         return;
    //     }
    // }
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
        // int i = spmc->read_pos;
        // int i = LOAD(&spmc->read_pos);
        int i = __atomic_load_n(&spmc->read_pos, __ATOMIC_ACQUIRE);

        // int old = spmc->nconsumers[i];
        int old = __atomic_load_n(&spmc->nconsumers[i], __ATOMIC_RELAXED);
        if (old < 0)
        { // 已被生产者占用
            continue;
        }

        if (CAS(&spmc->nconsumers[i], old, old + 1)) // TODO 这里可以是relaxed，无需barrier
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
    // while (1)
    // {
    //     int old = spmc->nconsumers[i];
    //     if (CAS(&spmc->nconsumers[i], old, old - 1))
    //     {
    //         return;
    //     }
    // }
    // 直接原子减一即可
    __atomic_fetch_sub(&spmc->nconsumers[i], 1, __ATOMIC_RELEASE);
}

/////////////////////////// 测试 ////////////////////////////////////////////////////////

#if 1

#include <sys/time.h>
#include <unistd.h>

static unsigned long long ustime(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long)tv.tv_sec) * 1000000 + tv.tv_usec;
}

spmc_t *spmc;
const int count = 20000000;

void *produce(void *not_used)
{
    unsigned long long start = ustime();

    for (int i = 0; i < count; i++)
    {
        uint8_t *buf = producer_alloc(spmc);
        if (buf == NULL)
        {
            printf("ERROR producer_alloc NULL!!!\n");
            exit(-1); // 不应该返回NULL
        }

        // buf[0] = (uint8_t)i;
        *((int *)buf) = i;
        // usleep(70000);

        // printf("produce: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
        producer_alloc_end(spmc, buf);
        // printf("produce: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
    }

    printf("producer: %llu\n", ustime() - start);
    return NULL;
}

void *consume(void *not_used)
{
    unsigned long long start = ustime();

    int diff = 0; // 不同的次数
    int same = 0; // 相同的次数。这两个值用来判断consume的空转率
    uint8_t old = 1;
    for (;;)
    {
        uint8_t *buf = consumer_alloc(spmc);
        if (buf == NULL)
        {
            printf("ERROR unexpected consumer_alloc returns NULL\n");
            continue;
        }
        int value = *((int *)buf);

        if (old != buf[0])
        {
            // printf("consume diff: %d %p %d\n", value, &buf[0], i);
            old = buf[0];
            diff++;
        }
        else if (value == count - 1)
        {
            break;
        }
        else
        {
            // printf("consume: %d %p %d\n", value, &buf[0], i);
            same++;
        }

        // usleep(100000);
        // printf("consume: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
        consumer_alloc_end(spmc, buf);
        // printf("consume: %d, %d, %d, %d\n", spmc->nconsumers[0], spmc->nconsumers[1], spmc->nconsumers[2], spmc->nconsumers[3]);
    }

    printf("consumer: %llu, diff: %d, same: %d\n", ustime() - start, diff, same);

    return NULL;
}

#define SHM_KEY 12310

int main_p()
{
    printf("producer: \n");
    spmc = (spmc_t *)spmc_init(SHM_KEY, 4, 10, 1);
    produce(NULL);
    return 0;
}

int main_c()
{
    printf("consumer: \n");
    spmc = (spmc_t *)spmc_init(SHM_KEY, 4, 10, 0);
    // spmc = (spmc_t*)spmc_init(SHM_KEY, 4, 10, 1);
    consume(NULL);
    return 0;
}

int main()
{
    printf("count: %d\n", count);
#ifdef PRODUCER
    return main_p();
#else
    return main_c();
#endif
}
#endif

/*
TODO
1、benchmark
    生产者生产COUNT个的时间
    消费者读取到COUNT的时间
2、加入校验逻辑：benchmark时可以不开
3、对比pthread_rw_lock.try_lock
4、封装成c++版本

# benchmark: 20000000 大约8~9秒

Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic> ./producer & ;./consumer
count: 20000000
producer:
count: 20000000
consumer:
consumer: 8087628, diff: 16818350, same: 1783771
producer: 8120363
Job 1, './producer &' has ended
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic>
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic>
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic> ./producer & ;./consumer
count: 20000000
producer:
count: 20000000
consumer:
consumer: 8900018, diff: 16514058, same: 2108781
producer: 8920919
Job 1, './producer &' has ended
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic>
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic>
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic> ./producer & ;./consumer
count: 20000000
producer:
count: 20000000
consumer:
producer: 9119123
consumer: 9097350, diff: 17187292, same: 1316273
Job 1, './producer &' has ended
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic> ./producer & ;./consumer
count: 20000000
producer:
count: 20000000
consumer:
producer: 8855508
consumer: 8836318, diff: 16714363, same: 1965960
Job 1, './producer &' has ended

spmc9:
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic> ./producer & ;./consumer
count: 20000000
producer:
count: 20000000
consumer:
producer: 4800935
consumer: 4748528, diff: 12006000, same: 1881689
Job 1, './producer &' has ended
Ubuntu@chuqq-matebook13 /m/d/w/t/c/c/20181104_dilu_spmc_atomic> ./producer & ;./consumer
count: 20000000
producer:
count: 20000000
consumer:
consumer: 4487629, diff: 12010354, same: 2026945
producer: 4508854
Job 1, './producer &' has ended



*/