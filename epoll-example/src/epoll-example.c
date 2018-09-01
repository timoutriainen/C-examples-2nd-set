
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>

#define MAX_EVENTS 64

void *client_handler(void *arg);

struct epoll_event *ready_list;



int main(int argc, char **argv) {
	printf("!!!Hello World!!!");
	int sockfd, newsockfd, portno, clilen, epfd;
	struct sockaddr_in server_address, client_address;
	struct epoll_event event;
	pthread_t helper_thread;
	pthread_attr_t attrb;
	printf("checking parameters\n");
	if(argc < 2) {
		printf("port missing\n");
		return 1;
	}
	printf("parameters ok\n");
	pthread_attr_init(&attrb);
	pthread_attr_setdetachstate(&attrb, PTHREAD_CREATE_DETACHED);
	epfd = epoll_create(MAX_EVENTS);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 1) {
		printf("no socket");
		return 1;
	}

	memset((char*) &server_address, 0, sizeof(server_address));
	portno = atoi(argv[1]);
	ready_list = (struct epoll_event*) calloc(MAX_EVENTS, sizeof(event));
	server_address.sin_family = AF_INET;
	server_address.sin_port = htons(portno);
	server_address.sin_addr.s_addr = INADDR_ANY;
	int a = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &a, sizeof(int));
	if(bind(sockfd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
		printf("bind failed\n");
		return 1;
	}
	listen(sockfd, 5);
	clilen = sizeof(client_address);
	printf("creating thread\n");
	pthread_create(&helper_thread, &attrb, &client_handler, &epfd);

	while(1) {
		printf("calling accept\n");
		newsockfd = accept(sockfd, (struct sockaddr*) &client_address, (socklen_t*) &clilen);
		if(newsockfd < 0) {
			printf("Problem creating socket\n");
		}
		event.events = EPOLLIN;
		event.data.fd = newsockfd;
		epoll_ctl(epfd, EPOLL_CTL_ADD, newsockfd, &event);
	}
	printf("main exiting");
	return EXIT_SUCCESS;
}

void* client_handler(void* arg) {
	char buffer[256];
	int i, n, a;
	int epfd = *((int*)arg);
	while(1) {
		n = epoll_wait(epfd, ready_list, MAX_EVENTS, -1);
		for(i = 0; i < n; i++) {
			memset(buffer, 0, sizeof(buffer));
			a = read(ready_list[i].data.fd, buffer, 255);
			if( a <= 0 ) {
				epoll_ctl(epfd, EPOLL_CTL_DEL, ready_list[i].data.fd, NULL);
				printf("client terminated, closing socket %d\n", ready_list[i].data.fd);
				close(ready_list[i].data.fd);

			} else {
				printf("client %d sent: %s", ready_list[i].data.fd, buffer);
			}
		}
	}
}

