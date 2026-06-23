#include "../include/server/alsocket.h"

#include <cassert>
#include <netdb.h>
#include <unistd.h>
#include <iostream>
#include <format>

int main() {
	addrinfo hints {};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	Socket socket;
	assert(socket.create("localhost", "8080", &hints) == 0);

	SockOpts opts;
	opts.flag = SetSockFlag::ReuseAddr;
	opts.level = SetSockLevel::Socket;
	opts.opt_value = 0;
	assert(socket.setopts(&opts) == 0);

	assert(socket.bind() == 0);
	assert(socket.listen() == 0);

	while(1) {
		int clientfd = socket.accept();
		if (clientfd > 0) {
			std::cout << std::format("ACCPTED CONNECTION: {}", 
				clientfd) << std::endl;
		} else {
			continue;
		}
		
		const char *msg = "Hello from server\n";
		ssize_t sent = ::send(clientfd, msg, strlen(msg), 0);
		assert(sent > 0);

		char buf[256]{};
		ssize_t received = ::recv(clientfd, buf, sizeof(buf) - 1, 0);
		if (received > 0) {
			std::cout << "Client replied: " << buf;
		}
		::close(clientfd);
	}

	return 0;
}
