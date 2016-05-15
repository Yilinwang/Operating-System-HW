#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <sched.h>

void* thread_func(void* t) {
	for(int i = 0; i < 3; i++) {
		fprintf(stderr, "Thread %d is running\n", (int)t);
		for(int i = 0; i < 100000000; i++);
	}
}

int main(int argc, char* argv[]) {
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET(1, &set);
	sched_setaffinity(getpid(), sizeof(set), &set);
	struct sched_param param;
	param.sched_priority = 1;
	if(argv[1] != NULL)
		sched_setscheduler(getpid(), SCHED_FIFO, &param);
	pthread_t tid[2];
	for(int i = 0; i < 2; i++) {
		pthread_create(&tid[i], NULL, &thread_func, (void*)i);
		fprintf(stderr, "Thread %d is created\n", i);
	}
	for(int i = 0; i < 2; i++)
		pthread_join(tid[i], NULL);
}
