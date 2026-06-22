#include "../include/server/alsocket.h"

#include <cassert>
#include <netdb.h>
#include <unistd.h>

int main() {
	addrinfo hints {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	Socket socket;
	int sockfd = socket.create("localhost", "8080", &hints);
	assert(sockfd >= 0);

	int bound = socket.bind(sockfd);
	assert(bound >= 0);

	int listening = socket.listen(sockfd);
	assert(listening >= 0);

	while(1) {
		int clientfd = socket.accept(sockfd);
		if (clientfd < 0) continue;
	}

	::close(sockfd);
	return 0;
}
