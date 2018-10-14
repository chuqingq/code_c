// gcc pthread_spinlock_delay.c -O3 -pthread
// ./a.out

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <time.h>
#include <unistd.h>

static unsigned long long nstime(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ((unsigned long long)ts.tv_sec)*1000000000 + ts.tv_nsec;
}

pthread_spinlock_t spinlock;

long long int starttime;

void* testThreadPool(int *t) {
	printf("thread start: %d\n", *t);
	// for (;;) {
		pthread_spin_lock(&spinlock);
		// printf("worker mutex\n");
		// pthread_cond_wait(&cond, &mutex);
		// printf("pthread_cond_wait time: %llu\n", nstime());
		printf("diff ns: %lld\n", nstime()-starttime);
		pthread_spin_unlock(&spinlock);
		// sleep(3);
	// }
	return (void*) 0;
}

int main() {
	int thread_num = 1;
	pthread_t *mythread = (pthread_t*) malloc(thread_num* sizeof(*mythread));

	// pthread_mutex_lock(&mutex);
	pthread_spin_init(&spinlock, 0);

	int t;
	for (t = 0; t < thread_num; t++) {
		int *i=(int*)malloc(sizeof(int));
		*i=t;

		pthread_spin_lock(&spinlock);
		if (pthread_create(&mythread[t], NULL, (void*)testThreadPool, (void*)i) != 0) {
			printf("pthread_create");
		}
	}
	printf("create thread\n");
	sleep(1);

	for (t = 0; t < thread_num; t++) {
		// printf("pthread_cond_signal time: %llu\n", nstime());
		starttime = nstime();
		pthread_spin_unlock(&spinlock);
		// 
		// a = t;
		// pthread_cond_signal(&cond);
		printf("after unlock\n");
		// sleep(2);
	}

	// sleep(2);
        for (t = 0; t < thread_num; t++) {
            pthread_join(mythread[t], NULL);
        }
}

/*
chuqq@mbpr-ub ~/t/c/c/20171010_pthread_mutex_cond> ./a.out 
create thread
thread start: 0
after unlock
diff ns: 1004
*/
