#include "../include/server/socket.h"

#include <cassert>
#include <netdb.h>
#include <unistd.h>

int main() {
	addrinfo hints {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	Socket socket;
	int sockfd = socket.create("localhost", "0", &hints);
	assert(sockfd >= 0);

	int clientfd = socket.accept_connect(sockfd);
	assert(sockfd > 0);
	::close(clientfd);

	::close(sockfd);
	return 0;
}
