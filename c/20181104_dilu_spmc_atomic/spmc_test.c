// gcc -o spmc12 -I. spmc_test.c -pthread -Wall
// ./spmc12

#include <spmc12.c> // TODO

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/shm.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

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

#define SHM_KEY 12311

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
    if (fork()) {
        // parent
        return main_p();
    } else {
        // child
        return main_c();
    }
}

/*
TODO
1、benchmark
    生产者生产COUNT个的时间：越小越好
    消费者读取到COUNT的时间：越小越好
    消费者读取到不同值的次数：越大越好
    消费者读取到相同值的次数：越小越好
2、加入校验逻辑：benchmark时可以不开
3、对比pthread_rw_lock.try_lock
4、封装成c++版本

# spmc12结果
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc12
count: 20000000
producer:
consumer:
consumer: 1453266, diff: 14934187, same: 1543960
producer: 1453327
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc12
count: 20000000
producer:
consumer:
producer: 1835902
consumer: 1835773, diff: 13065069, same: 1689737
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc12
count: 20000000
producer:
consumer:
producer: 1689075
consumer: 1688866, diff: 13680881, same: 1513019
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc12
count: 20000000
producer:
consumer:
producer: 1974799
consumer: 1974584, diff: 12801434, same: 1758868
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc12
count: 20000000
producer:
consumer:
producer: 1713916
consumer: 1713713, diff: 14106394, same: 1769611
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc12
count: 20000000
producer:
consumer:
producer: 1772951
consumer: 1772837, diff: 14029246, same: 1848695

# spmc10结果
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc10
count: 20000000
producer:
consumer:
producer: 1596674
consumer: 1596560, diff: 12250309, same: 1032610
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc10
count: 20000000
producer:
consumer:
producer: 1764479
consumer: 1764419, diff: 14071021, same: 1300751
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc10
count: 20000000
producer:
consumer:
producer: 1742373
consumer: 1742299, diff: 13952489, same: 1227722
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc10
count: 20000000
producer:
consumer:
producer: 1588670
consumer: 1588571, diff: 12110450, same: 1020999
chuqq@cqq-surfacebook2~/t/c/c/20181104_dilu_spmc_atomic $ ./spmc10
count: 20000000
producer:
consumer:
producer: 1778665
consumer: 1778508, diff: 12041193, same: 946573
*/