#include "../include/server/alsocket.h"

#include <cassert>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

int main() {
	addrinfo hints {};
	hints.ai_family   = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	Socket socket;
	int sockfd = socket.create("localhost", "8080", &hints);
	assert(sockfd >= 0);

	int conn = socket.connect(sockfd);
	assert(conn >= 0);

	const char* msg = "Hello from client\n";
	ssize_t sent = ::send(sockfd, msg, strlen(msg), 0);
	assert(sent > 0);

	char buf[256] {};
	ssize_t received = ::recv(sockfd, buf, sizeof(buf) - 1, 0);
	if (received > 0) {
		std::cout << "Server replied: " << buf;
	}

	::close(sockfd);
	return 0;
}
