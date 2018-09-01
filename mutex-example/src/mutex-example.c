/*
 ============================================================================
 Name        : mutex-example.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mymutex;
int global = 0;

void *increment_thread(void *arg) {
	int i;
	for(i=0; i<10; i++) {
		printf("in increment thread: before mutex: global = %d\n", global);
		pthread_mutex_lock(&mymutex);
		global++;
		printf("in increment thread: inside mutex: global = %d\n", global);
		sleep(2);
		pthread_mutex_unlock(&mymutex);
		printf("in increment thread: after mutex: global = %d\n", global);
		sleep(1);
	}
	return 0;
}

int main(void) {
	puts("!!!Hello mutex!!!"); /* prints !!!Hello World!!! */
	pthread_t mythread;
	int i;

	pthread_mutex_init(&mymutex, 0);
	if(pthread_create(&mythread, 0, increment_thread, 0)){
		printf("error creating thread");
		exit(1);
	}

	for(i=0;i<10;i++){
		printf("in main: before mutex lock: global = %d\n", global);
		pthread_mutex_lock(&mymutex);
		global ++;
		sleep(1);
		printf("in main: in mutex lock: global = %d\n", global);
		pthread_mutex_unlock(&mymutex);
		printf("in main: after mutex: global = %d\n", global);
		sleep(2);
	}
	printf("\nexiting main loop\n");

	return EXIT_SUCCESS;
}
