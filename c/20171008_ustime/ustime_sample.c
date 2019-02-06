#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>

static unsigned long long ustime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return ((unsigned long long)tv.tv_sec)*1000000 + tv.tv_usec;
}

int main() {
    long long t1, t2, t3;
    t1 = ustime();
    t3 = ustime();
    sleep(1);
    t2 = ustime();

    printf("time diff1: %lld\n", t3-t1);
    printf("time diff: %lld\n", t2-t1);
    printf("t1: %lld, t2: %lld, t3: %lld\n", t1, t2, t3);
    return 0;
}

/*
@cqqmbpr

chuqq@cqqmbpr /V/w/t/c/c/20171008_ustime> ./a.out
time diff1: 0
time diff: 1003659
t1: 1549430626747865, t2: 1549430627751524, t3: 1549430626747865
*/

