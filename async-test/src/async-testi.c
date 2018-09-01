/*
 ============================================================================
 Name        : async-io.c
 Author      :
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/select.h>
#include <string.h>

const char fifo1[] = "/tmp/fifo1";
const char fifo2[] = "/tmp/fifo2";


int child_action();

int main(int argc, char **argv) {
	printf("!!!Hello World!!!\n");
	int pid, fifo[2], flags;
	int n, m;
	char buffer[50];
	fd_set readset;

	mkfifo(fifo1, 0666);
	mkfifo(fifo2, 0666);
	printf("files created\n");

	printf("forking\n");
	pid = fork();
	if(pid < 0) {
		printf("problem creating a child\n");
		return 1;
	}
	if(pid == 0) {
		printf("inside child process\n");
		child_action();
		puts("child returning");
		return 0;
	}

	printf("parent opening fifo1\n");
	fifo[0] = open(fifo1, O_RDONLY, 0);
	printf("parent opening fifo2\n");
	fifo[1] = open(fifo2, O_RDONLY, 0);
	printf("parent, fifos open: %d, %d\n", fifo[0], fifo[1]);
	for(n=0; n<2; n++) {
		flags = fcntl(fifo[n], F_GETFL);
		flags = flags | O_NONBLOCK;
		fcntl(fifo[n], F_SETFL, flags);
	}

	while(1) {
		FD_ZERO(&readset);
		FD_SET(fifo[0], &readset);
		FD_SET(fifo[1], &readset);
		n = select(fifo[1]+1, &readset, 0, 0, 0);
		if(n>0) {
			for(m=0; m<2; m++) {
				memset(buffer, 0, 50);
				if(FD_ISSET(fifo[m], &readset)) {
					n = read(fifo[m], &buffer, 50);
				}
				if(strncmp(buffer, "quit", 4) == 0) {
					printf("quit from child\n");
					return 0;
				}
				printf("message from fifo %d: %s\n", fifo[m], buffer);
			}
		}
	}
}

int child_action() {
	printf("child_action()\n");
	int fifo[2];
	printf("opening /tmp/fifo1\n");
	fifo[0] = open(fifo1, O_WRONLY, 0);
	printf("opening /tmp/fifo2\n");
	fifo[1] = open(fifo2, O_WRONLY, 0);
	printf("child fifos open %d and %d\n", fifo[0], fifo[1]);
	sleep(2);
	int n;
	n = write(fifo[0], "terve\n", 6);
	n = write(fifo[1], "terve taas\n", 11);
	sleep(1);
	n = write(fifo[0], "hello\n", 6);
	n = write(fifo[1], "terve tais\n", 11);
	sleep(1);
	n = write(fifo[0], "tever\n", 6);
	n = write(fifo[1], "herve vaan\n", 6);
	sleep(1);
	n = write(fifo[0], "quit\n", 5);
	n = write(fifo[1], "quit\n", 5);
	sleep(1);
	return 0;
}
