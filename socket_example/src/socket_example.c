/*
 ============================================================================
 Name        : socket_example.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>


int main(int argc, char **argv) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
	int sock, newsocket, portno, clilength, n;
	char buffer[256];
	struct sockaddr_in server_socket, client_socket;

	if(argc < 2){
		printf("please");
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
	printf("now accepting connections");
	newsocket = accept(sock, (struct sockaddr*) &client_socket, &clilength);
	if(newsocket < 0) {
		printf("error accepting...");
		return 1;
	}
	do {
		memset(buffer, 0, 256);
		n = read(newsocket, buffer, 256);
		if(n > 0){
			printf("client sends am message : %s\n", buffer);
		}

	}while(n > 0);
	close(newsocket);
	close(sock);

	return EXIT_SUCCESS;
}
