/*
 ============================================================================
 Name        : socket_client.c
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
#include <error.h>
#include <netdb.h>
#include <errno.h>


int main(int argc, char** argv) {
	puts("!!!Hello client!!!"); /* prints !!!Hello World!!! */

	struct addrinfo hints, *res;
	int n, sock;
	char buffer[256];

	if(argc < 3){
		printf("params");
		return 1;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	if(getaddrinfo(argv[1], argv[2], &hints, &res) != 0) {
		printf("cannot get address from hints");
		return 1;
	}

	if(connect(sock, res->ai_addr, res->ai_addrlen) != 0) {
		printf("problem connecting to host %s\n", strerror(errno));
		return 1;
	}

	do {
		memset(buffer, 0, 255);
		printf("enter message:\n");
		fgets(buffer, 256, stdin);
		n=write(sock, buffer, 256);
	} while(n>0);


	return EXIT_SUCCESS;
}
