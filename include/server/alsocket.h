#pragma once

#include <sys/socket.h>
#include <netdb.h>

// TODO - Add all sockopt flags
enum class SockFlag : int {
	ReuseAddr
};

// TODO - Add all socket levels
enum class SockLevel : int {
	Socket,
};

typedef struct {
	SockFlag flag;
	SockLevel level;
	int opt_value;
} SockOpts;

class Socket {
	public:
		Socket();
		~Socket();
		
		// Creates and binds socket
		int create(
			const char* host,
			const char* service,
			const struct addrinfo* hints = nullptr,
			const SockOpts* opts = nullptr
			);
		
		// Binds socket
		int bind(int sockfd, const struct addrinfo* res = nullptr);

		// Listens for connection
		int listen(int sockfd, int backlog = 5, const struct addrinfo* res = nullptr);
			
		// Must be ran in a while loop
		int accept_connect(int clientfd);

	private:
		struct addrinfo* res_;
};
