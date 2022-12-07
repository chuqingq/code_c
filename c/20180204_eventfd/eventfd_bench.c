// gcc eventfd_sample.c
#include <sys/eventfd.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

long long int starttime;

static unsigned long long nstime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((unsigned long long)ts.tv_sec) * 1e9 + ts.tv_nsec;
}

int fd;
int fd2;
uint64_t buffer;
uint64_t buf = 1;

const uint64_t count = 10000;

//线程函数
void *threadFunc(void *arg)
{
    int ret;
    for (uint64_t i = 0; i < count; i++)
    {
        ret = read(fd, &buffer, sizeof(buffer));
        if (ret != sizeof(buffer))
        {
            printf("读取成功\n");
        }

        ret = write(fd2, &buf, sizeof(buf));
        if (ret != sizeof(buf))
        {
            printf("写错误： %d\n", ret);
            perror("write eventfd error");
        }
    }
    return NULL;
}

int main(void)
{
    // init
    int ret;
    pthread_t tid;

    //创建事件驱动的文件描述符
    if ((fd = eventfd(0, 0)) == -1)
    {
        printf("创建失败\n");
    }

    if ((fd2 = eventfd(0, 0)) == -1)
    {
        printf("创建失败\n");
    }

    //创建线程
    if (pthread_create(&tid, NULL, threadFunc, NULL) < 0)
    {
        printf("线程创建失败\n");
    }

    struct sched_param param;
    param.sched_priority = 10;
    int policy = SCHED_FIFO;
    ret = pthread_setschedparam(tid, policy, &param);
    if (ret != 0)
        printf("pthread_setschedparam error\n");

    // run
    starttime = nstime();
    for (uint64_t i = 0; i < count; i++)
    {
        ret = write(fd, &buf, sizeof(buf)); // 向eventfd写，其实是在count上累加
        if (ret != sizeof(buf))
        {
            printf("写错误： %d\n", ret);
            perror("write eventfd error");
        }

        ret = read(fd2, &buffer, sizeof(buffer)); //阻塞等待fd可读，及通知事件发生
        if (ret != sizeof(buffer))
        {
            printf("读失败\n");
        }
    }
    printf("%lld ns/loop\n", (nstime() - starttime) / count);

    pthread_join(tid, NULL);
    close(fd);
    close(fd2);
    return 0;
}
// gcc -o eventfd_bench{,.c} -Wall
// 27022 ns/loop