
#include <stdio.h>
#include <stdlib.h>

#include <sys/time.h>  
#include <unistd.h> // sleep
#include <pthread.h>
#include <sched.h>

static unsigned long long ustime(void) {                                        
    struct timeval tv;                                                          
    gettimeofday(&tv, NULL);                                                    
    return ((unsigned long long)tv.tv_sec)*1000000 + tv.tv_usec;                
}

static unsigned long long nstime(void) {                                        
    struct timespec ts;                                                         
    clock_gettime(CLOCK_REALTIME, &ts);                                         
    return ((unsigned long long)ts.tv_sec)*1e9 + ts.tv_nsec;                    
}

pthread_t createSchedFifoThread(void* (*pThreadFunc)(void*), int priority) {
    struct sched_param param;
    int policy;
    int ret;
    pthread_t id;
    ret = pthread_create(&id, NULL, pThreadFunc, NULL);
    if(ret != 0) {
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

void* thread1(void* arg) {
    unsigned long long loop = 1; // 循环次数
    struct timespec tv;
    tv.tv_sec = 0;
    // unsigned long long start = nstime();
    unsigned long long start = nstime();
    printf("====start: %llu\n", start);
    unsigned long long target;
    unsigned long long now;
    long long d;
    while(1) {
        now = nstime();
        if (loop++ % 1000 == 0) {
            printf("%lld\n", now);
        }

        // 模拟业务开始
        usleep(loop % 500); // 模拟500us以内的业务
        // 模拟业务结束

        target = start + loop*1e6; // 提前睡醒，再自旋
        // tv.tv_nsec = target - now;
        // select(0, NULL, NULL, NULL, &tv);
        // nanosleep(next - nstime());
        // if (target > now) {
        //     nanosleep(&tv, NULL);
        // }
        //  else {
        //     printf("target <= now: %lld\n", now - target);
        // }
        while ((d = (target-80*1e3-nstime())) > 0) usleep(d/1e3); // pause?
        while (nstime() < target) ; // pause?
    }
}

int main(void)
{
    pthread_t id;
    id = createSchedFifoThread(thread1, 10);
    while(1) {
        sleep(1);
    }
    return 0;
}
 
/*
gcc -static -o test_sched sched_fifo_timer_ns_final.c -pthread
sudo taskset -c 0 ./test_sched

结论：
1.在没有实时线程的情况下，代码写好（可以先usleep降低cpu，然后提前醒来进行自选），可以控制到每1ms触发一次，误差是ns以内。
2.用select和usleep、nanosleep精度差不多。说是select对于信号、中断等更安全
3.没有验证高负载情况。猜测高负载下精度应该会受影响，可以通过上实时线程来解决。

结果：
chuqq@chuqq-r7000p/m/d/t/p/c/c/20200527_sched_fifo $ sudo taskset -c 0 ./a.out
====start: 1665541206242488832
1665541207242488832
1665541208242488832
1665541209242488832
1665541210242488832
1665541211242488832
1665541212242488832
1665541213242488832
1665541214242488832

cpu占用率：单核7%
    PID USER      PR  NI    VIRT    RES    SHR S  %CPU  %MEM     TIME+ COMMAND
   3621 root      20   0   76212    880    788 S   7.0   0.0   0:01.82 a.out
*/
