/*
 ============================================================================
 Name        : server-logger.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>

#define LOGFILE "/tmp/logfile"
#define FIFOFILE "/tmp/fifo"

int startLogging(void) {
	int fifofd,fifon;
	int logfile;
	char fifobuffer[255];
	logfile = open(LOGFILE,O_WRONLY|O_APPEND|O_CREAT,0777);
	fifon = mkfifo(FIFOFILE, 0777);
	printf("logging started");
	while(1) {
		fifofd=open(FIFOFILE,O_RDONLY);
		printf("logging continuing");
		do {
			memset(fifobuffer,0,255);
			fifon = read(fifofd,fifobuffer,255);
			if (fifon>0) {
				write(logfile,fifobuffer,strlen(fifobuffer));
			}
		} while(fifon > 0);
		close(fifofd);
	}
	printf("logging stopped");
	return 0;
}

int writeToLog(char* buffer) {
	int fifofd;
	printf("writing to fifo");

	fifofd=open(FIFOFILE,O_WRONLY);
	write(fifofd,buffer,strlen(buffer));
	close(fifofd);

	printf("writing to fifo stopped");
	return 0;
}

int createLogger(void) {
	pid_t pid1 = 0;

	pid1 = fork();
	if(pid1 < 0) {
		printf("Error!\n");
		exit(1);
	}
	if(pid1 == 0) {
		startLogging();
		exit(0);
	}
	return 0;
}

int main(int argc, char **argv) {

	puts("!!!Hello from server-logger!!!"); /* prints !!!Hello World!!! */

	//fork logger
	createLogger();

	printf("continuing...");

	//accept
	int sock, newsocket, portno, clilength, n;
	char buffer[256];
	struct sockaddr_in server_socket, client_socket;

	if(argc < 2){
		printf("please check cmd line params");
		return 1;
	}
	portno = atoi(argv[1]);
	sock = socket(AF_INET, SOCK_STREAM, 0);
	server_socket.sin_family = AF_INET;
	server_socket.sin_addr.s_addr = INADDR_ANY;
	server_socket.sin_port = htons(portno);
	int a = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int));

	if(bind(sock, (struct sockaddr *)&server_socket, sizeof(server_socket)) <0) {
		printf("error %s\n", strerror(errno));
		return 1;
	}

	listen(sock, 5);
	clilength = sizeof(client_socket);
	for(;;){
		printf("\nnow blocking on accept\n");
		newsocket = accept(sock, (struct sockaddr*) &client_socket, &clilength);
		printf("\naccept returned\n");
		if(newsocket < 0) {
			printf("error accepting...");
			return 1;
		}
		pid_t pid;
		pid = fork();
		if (pid < 0) {
			printf("Cannot create child,%s\n",strerror(errno));
			exit(1);
		}
		if (pid == 0) {
			printf("\nchild connection process started\n");

			do {
				memset(buffer, 0, 256);
				n = read(newsocket, buffer, 256);
				if(n > 0){
					printf("\nchild: client sends a message : %s\n", buffer);
					printf("\nchild: writing to fifo");
					writeToLog(buffer);
				}

			}while(n > 0);
			printf("\nchild: client disconnected, closing socket\n");
			close(newsocket);
			exit(0);
		}

	}
	printf("\nexiting from for loop\n");
	printf("\nparent: closing accepting socket connections\n");
	close(sock);


	return EXIT_SUCCESS;
}
