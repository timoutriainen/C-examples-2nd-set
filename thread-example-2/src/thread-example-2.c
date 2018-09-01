/*
 ============================================================================
 Name        : thread-example-2.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

pthread_mutex_t worker_thread;
pthread_mutex_t control_thread;
pthread_cond_t control_cond;

int global = 0;

void *worker_function(void *arg) {
	printf("worker thread: %d, before mutex lock: global = %d\n", *((int*)arg), global);
	pthread_mutex_lock(&worker_thread);
	sleep(2);
	global++;
	printf("worker thread: %d, in mutex lock: global = %d\n", *((int*)arg), global);
	pthread_mutex_unlock(&worker_thread);
	if(global == 2){
		printf("worker thread: %d, condition good, waking up control\n", *((int*)arg));
		pthread_cond_broadcast(&control_cond);
	}
	return 0;
}

void *control_function(void *arg) {
	printf("control thread: %d, before condition: global = %d\n", *((int*)arg), global);
	while(global<2){
		pthread_mutex_lock(&control_thread);
		pthread_cond_wait(&control_cond, &control_thread);
	}
	printf("control thread: %d, after condition: global = %d\n", *((int*)arg), global);
	global++;
	sleep(3);
	printf("control thread: %d, before mutex unlock global = %d\n", *((int*)arg), global);
	pthread_mutex_unlock(&control_thread);
	return 0;
}


int main(void) {
	puts("!!!Hello thread-example-2!!!"); /* prints !!!Hello World!!! */

	pthread_t worker1, worker2, control;
	int worker1_id=100, worker2_id=200, control_id=300;
	pthread_mutex_init(&worker_thread, 0);
	pthread_mutex_init(&control_thread, 0);
	pthread_cond_init(&control_cond, 0);

	if(pthread_create(&worker1, 0, worker_function, &worker1_id)){
		printf("error create worker1");
		exit(1);
	}

	if(pthread_create(&worker2, 0, worker_function, &worker2_id)){
		printf("error create worker2");
		exit(1);
	}

	if(pthread_create(&control, 0, control_function, &control_id)){
		printf("error create control");
		exit(1);
	}

	printf("main, done creating workers and ctrl, going condition\n");

	while(global<2){
		pthread_mutex_lock(&control_thread);
		pthread_cond_wait(&control_cond, &control_thread);
	}
	printf("main thread, after condition, global: %d\n", global);
	global++;
	sleep(3);
	pthread_mutex_unlock(&control_thread);
	printf("main thread, after mutex unlock, global = %d\n", global);
	pthread_exit(0);
}
