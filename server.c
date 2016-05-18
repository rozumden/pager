#include <sys/socket.h>

int socket;

int connect(int pager_id, char * address, int port = 55556) {
		socket = socket(AF_INET, SOCK_STREAM, 0);
		return 1;
}
