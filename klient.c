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
#include "klient.h"
#include "pci.h"

#define BUFFERSIZE 256

int connectToServer(char * address, int port) {
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

	while (threadsRunning) {
		int first, second, third, message;
		char buffer[BUFFERSIZE];
		char token[BUFFERSIZE];
        char response[BUFFERSIZE];

        //gets(buffer);
        fgets(buffer, sizeof(buffer), sockFile);
        printf("Server sent: %s\n", buffer);

        // Extract token
        int loaded = sscanf(buffer, "%s %d %d %d %d", token, &first, &second, &third, &message);

		if (strcmp(token, "querrymessage_r") == 0) {    // <token><pager_id><message_id><sender_id><message>
            if (loaded == 2) {
                // server has no message for us
            } else {
                sprintf(response, "confirmmessage %d %d\n", first, second);
                write(sockfd, response, strlen(response));

                uploadMessage(third, message);
            }
		}
		if (strcmp(token, "confirmmessage_r") == 0) {   // <token><pager_id><message_id>
            printf("Message %d for %d confirmed\n", second, first);
		}
		if (strcmp(token, "sendmessage_r") == 0) {      // <token><pager_id><recipient_id><message_id>
            printf("Message received our message for %d.", second);
		}

		usleep(1000000*5);
	}
}


		//int loaded = /*f*/scanf(/*sockFile, */"%s %d %d %d %d %d", buffer, &first, &second, &third, &fourth, &fifth);
		/*if (loaded == 4) {
			// zpracuj zpravu
			printf("%s %d %d %d %d \n", buffer, id, first, second);
			if (strcmp(buffer, "sendmessage_r")) {
                printf("Match.\n");
			}
		} else {
			printf("Could only load %d\n", loaded);
		}*/

/*
int main() {
    int socket = connectToServer("127.0.0.1", 55556);
    write(socket, "querrymessage 42\n", 17);
    messageReceiver(&socket);



    return 0;
}
/**/
