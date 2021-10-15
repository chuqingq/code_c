
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep
#include <pthread.h>
#include <sched.h>
pthread_t createSchedFifoThread(void* (*pThreadFunc)(void*), int priority)
{
        struct sched_param param;
        int policy;
        int ret;
        pthread_t id;
        ret = pthread_create(&id, NULL, pThreadFunc, NULL);
        if(ret != 0)
        {
                perror("pthread_create() error:");
                return 0;
        }
        param.sched_priority = priority;
        policy = SCHED_FIFO;
        ret = pthread_setschedparam(id, policy, &param);
        if(ret != 0)
        {
                perror("pthread_setschedparam() error:");
                return 0;
        }
        return id;
}

void* thread1(void* arg)
{
        sleep(1);
        while(1) {
			printf("thread1\n");
			sleep(1);
		}
}

void* thread2(void *arg)
{
        sleep(1);
        while(1)
        {
                printf("thread2\n");
                sleep(1);
        }
}

int main(void)
{
        pthread_t id;
        id = createSchedFifoThread(thread1, 10);
		sleep(3);
        id = createSchedFifoThread(thread2, 11);
        while(1)
        {
                sleep(1);
        }
        return 0;
}
 
/*
gcc sched_fifo.c -pthread
sudo taskset -c 0 ./a.out
// 结果是thread2不会被打印出来。因为两个线程都绑定了一个核，thread1优先级更高，没有主动让出cpu，不会轮到thread2.
*/

