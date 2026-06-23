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
	assert(socket.create("localhost", "8080", &hints) == 0);
	assert(socket.connect() == 0);

	const char* msg = "Hello from client\n";
	ssize_t sent = ::send(socket.fd(), msg, strlen(msg), 0);
	assert(sent > 0);

	char buf[256] {};
	ssize_t received = ::recv(socket.fd(), buf, sizeof(buf) - 1, 0);
	if (received > 0) {
		std::cout << "Server replied: " << buf;
	}

	return 0;
}
