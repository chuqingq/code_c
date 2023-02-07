#include <stdio.h>
#include <arpa/inet.h>   //for inet_ntoa , struct sockaddr_in
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <sys/times.h>

int main(int argc , char *argv[])
{
    int sysHz = 0;
    sysHz = sysconf(_SC_CLK_TCK);
    printf("sysHz = %d\n", sysHz );
    printf("tick count in ms: %lu\n", (unsigned long) times( NULL));
    sleep(1);
    printf("tick count in ms: %lu\n", (unsigned long) times( NULL));
    sleep(1);
    printf("tick count in ms: %lu\n", (unsigned long) times( NULL));
    sleep(1);
    printf("tick count in ms: %lu\n", (unsigned long) times( NULL));
    sleep(1);
    printf("tick count in ms: %lu\n", (unsigned long) times( NULL));
}

/*
$ gcc -o ticks_times{,.c}
$ ./ticks_times 
sysHz = 100
tick count in ms: 1761561402
tick count in ms: 1761561502
tick count in ms: 1761561602
tick count in ms: 1761561702
tick count in ms: 1761561802
*/