#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include "server.h"

int connectToServer(int pager_id, char * address, int port) {
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0) {
		perror("Socket cannot be created");
		return -1;
	}
	
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (inet_pton(AF_INET, address, &server.sin_addr) != 1) {
		close(socketfd);
		perror("invalid address family or IP address");
		return -1;
	}
	
	if (connect(socketfd, (struct sockaddr *) &server, sizeof(server)) != 0) {
		perror("Unable to connect to server.");
		return -1;
	}
	
	return socketfd;
}

void * messageReceiver(void * socketfd) {
	int sockfd = *(int *)socketfd; 
	FILE *sockFile = (FILE *)fdopen(sockfd, "r");
	int running = 1;

	while (running) {
		int id, message;
		char * buffer;
		int loaded = fscanf(sockFile, "%s %d %d", &buffer, &id, &message)
		if (loaded == 3) {
			// zpracuj zpravu
			printf("%s %d %d\n", buffer, id, message);
		} else {
			printf("Could only load %d", loaded);
		}
		usleep(2000000);
	}
}
