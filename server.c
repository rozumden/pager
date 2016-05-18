#include <sys/socket.h>

int socket;

int connect(int pager_id, char * address, int port = 55556) {
	socket = socket(AF_INET, SOCK_STREAM, 0);
	if (socket < 0) {
		perror("Socket cannot be created");
		return -1;
	}
	
	struct sockaddr_in server;
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	if (inet_pton(AF_INET, address, &server.sin_addr) != 1) {
		close(socket);
		perror("invalid address family or IP address");
		return -1;
	}
	
	if (connect(socket, (struct sockaddr *) &server, sizeof(server)) != 0) {
		perror("Unable to connect to server.");
		return -1;
	}
	
	return socket;
}

