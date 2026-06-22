#pragma once

#include <sys/socket.h>
#include <netdb.h>

// TODO - Add all sockopt flags - *** man 7 socket ***
enum class SetSockFlag : int {
	ReuseAddr, // SO_REUSEADDR
	ABPF, // SO_ATTACH_FILTER - takes structure sock_fprog
	AeBPF, // SO_ATTACH_BPF - same as above
};

// TODO - Add all socket levels
enum class SetSockLevel : int {
	Socket, // SOL_SOCKET - all types - default
};

// TODO - Add all readonly flags
enum class GetSockInfo : int {
	IsListen, // SO_ACCEPTCONN 
};

typedef struct {
	SetSockFlag flag;
	SetSockLevel level;
	int opt_value;
} SockOpts;

/*
* Socket:
*	--- Example ---
*	Socket socket;
*
*	addrinfo hints {};
* 	hints.ai_family = AF_INET;
*	hints.ai_socktype = SOCK_STREAM;
*	hints.ai_flags = AI_PASSIVE;
*
*	int sockfd = socket.create("localhost", "8080", &hints);
*	assert(sockfd >= 0);
*
*	int bound = socket.bind(sockfd);
*	assert(bound >= 0);
*/
class Socket {
	public:
		Socket();
		~Socket();
		
		// Creates socket
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
		int accept(int sockfd);
		
		// For client 
		int connect(int sockfd, const struct addrinfo* res = nullptr);

	private:
		struct addrinfo* res_;
};
