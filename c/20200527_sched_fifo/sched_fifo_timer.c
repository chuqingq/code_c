
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
    // param.sched_priority = priority;
    // policy = SCHED_FIFO;
    // ret = pthread_setschedparam(id, policy, &param);
    // if(ret != 0)
    // {
    //     perror("pthread_setschedparam() error:");
    //     return 0;
    // }
    return id;
}

void* thread1(void* arg) {
    unsigned long long loop = 1; // 循环次数
    struct timeval tv;
    unsigned long long start = ustime();
    unsigned long long target;
    unsigned long long now;
    while(1) {
        target = start + loop*1000;
        // 模拟业务开始
        usleep(loop % 500); // 模拟500us以内的业务
        if (loop++ % 1000 == 0) {
            printf("%lld\n", now);
        }
        // 模拟业务结束
        now = ustime();
        tv.tv_sec = 0;
        tv.tv_usec = target - now;
        // select(0, NULL, NULL, NULL, &tv);
        // nanosleep(next - nstime());
        usleep((target > now) ? (target - now) : 0);
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
gcc sched_fifo.c -pthread
sudo taskset -c 0 ./a.out

结论：
1.在没有实时线程的情况下，代码写好，可以控制到每1ms触发一次，误差是50ns以内。
2.用select和usleep、nanosleep精度差不多。说是select对于信号、中断等更安全
3.没有验证高负载情况。猜测高负载下精度应该会受影响，可以通过上实时线程来解决。

结果：
chuqq@chuqq-r7000p ~/w/t/p/c/c/20200527_sched_fifo (master)> ./a.out
1639047142393492
1639047143393490
1639047144393488
1639047145393485
1639047146393495
1639047147393481
1639047148393487
1639047149393487
1639047150393484
1639047151393487
1639047152393488
1639047153393485
1639047154393531
1639047155393492

*/

