/*
 ============================================================================
 Name        : thread-example.c
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

int global = 0;

void *thread_function(void *arg) {
	int i,j;
	for(i=0;i-20;i++){
		j = global;
		j++;
		printf("In thread, global value: %d\n", global);
		sleep(1);
		global = j;
		printf("In thread, global value = %d\n", global);
	}
	return 0;
}

int main(void) {
	puts("!!!Hello thread-example!!!"); /* prints !!!Hello World!!! */
	pthread_t mythread;
	int i;
	if(pthread_create(&mythread, 0, thread_function, 0)) {
		printf("error");
		return 1;
	}

	for(i=0; i<20; i++){
		printf("main, global value = %d, sleeping\n", global);
		global++;
		sleep(1);
		printf("main, after sleep, global value = %d\n", global);
	}
	if(pthread_join(mythread, 0)){
		printf("error joining");
		return 1;
	}
	printf("main done, exiting");
	return EXIT_SUCCESS;
}
