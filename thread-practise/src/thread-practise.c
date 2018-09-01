/*
 ============================================================================
 Name        : thread-practise.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t worker_thread;
pthread_mutex_t control_thread;
pthread_cond_t worker_cond;
pthread_cond_t control_cond;

int global = 0;

void *worker_function(void *arg) {
	printf("worker thread: %d, joined game\n", *((int*)arg));
	for(;;) {
		printf("worker thread: %d, waiting to be waken up\n", *((int*)arg));
		pthread_mutex_lock(&worker_thread);
		pthread_cond_wait(&worker_cond, &worker_thread);
		pthread_mutex_unlock(&worker_thread);
		printf("worker thread: %d, game starts now\n", *((int*)arg));
		int sleepAmount = *((int*)arg) / 100;
		printf("worker thread: %d, sleeping %d\n", *((int*)arg), sleepAmount);
		sleep(sleepAmount);

		srand(time(NULL));
		int sleepTime = rand() % 10;
		printf("worker thread: %d going for sleep for %d\n",*((int*)arg),sleepTime);
		sleep(sleepTime);
		printf("worker thread: %d, woke up, before mutex lock: global = %d\n", *((int*)arg), global);
		pthread_mutex_lock(&worker_thread);
		sleep(2);
		global++;
		printf("worker thread: %d, in mutex lock: global = %d\n", *((int*)arg), global);
		pthread_mutex_unlock(&worker_thread);
		if(global == 2){
			printf("worker thread: %d, condition good, waking up control\n", *((int*)arg));
			pthread_cond_broadcast(&control_cond);
		}
	}
	return 0;
}


int main(void) {
	puts("!!!Hello thread-practise!!!"); /* prints !!!Hello World!!! */

	pthread_t worker1, worker2;
	int worker1_id=100, worker2_id=200;
	pthread_mutex_init(&worker_thread, 0);
	pthread_mutex_init(&control_thread, 0);
	pthread_cond_init(&control_cond, 0);
	pthread_cond_init(&worker_cond, 0);

	if(pthread_create(&worker1, 0, worker_function, &worker1_id)){
		printf("error create worker1");
		exit(1);
	}

	if(pthread_create(&worker2, 0, worker_function, &worker2_id)){
		printf("error create worker2");
		exit(1);
	}



	for(;;){
		printf("main - sleeping\n");
		sleep(1);
		printf("main - sleeping done\n");

		printf("main - waking up workers\n");
		pthread_cond_broadcast(&worker_cond);
		printf("main - done\n");

		printf("main - going to wait for game start condition\n");

		while(global<2){
			printf("main - global %d\n", global);
			pthread_mutex_lock(&control_thread);
			pthread_cond_wait(&control_cond, &control_thread);
		}
		printf("main thread, game condition reached, players: %d\n", global);
		global = 0;
		pthread_mutex_unlock(&control_thread);
		printf("main thread - throwing a question\n");

	}
	printf("main thread, after mutex unlock, global = %d\n", global);
	pthread_exit(0);
}


