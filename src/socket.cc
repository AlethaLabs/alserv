#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>

#include <iostream>

#include "server/alsocket.h"

Socket::Socket() : res_(nullptr) {}

Socket::~Socket() {
	if (res_ != nullptr) {
		freeaddrinfo(res_);
		res_ = nullptr;
	}
}

/* 
 * Helpers for get sockopts
*/
// TODO - finish all flags
int translate_flag(SetSockFlag flag) {
	switch (flag) {
		case SetSockFlag::ReuseAddr:
			return SO_REUSEADDR;
		default:
			return -1;
	}
}

// TODO - finish all levels
int translate_level(SetSockLevel level) {
	switch (level) {
		case SetSockLevel::Socket:
			return SOL_SOCKET;
		default:
			return -1;
	}
}

/*
 * Creates a socket:
 * 	- uses getaddrinfo to populate socket paramaters
 *  	- host: address you wish to bind socket - ex:"localhost"
 *  	- service: port or protocol type - ex:"http" or "80"
 * 	- Optionals:
 *		- provide custom hints for address resolution
 *		- provide socket option flags
 * 	- returns -1 on error
 */
int Socket::create(
		const char* host,
		const char* service, 
		const struct addrinfo* hints,
		const SockOpts* opts
		) {
	addrinfo* resolved = nullptr;
	
	int stats = getaddrinfo(host, service, hints, &resolved);
	if (stats != 0) {
		std::cerr << "Address resolution failed: " << gai_strerror(stats) << "\n";
		return -1;
	} 

	if (res_ != nullptr) {
		freeaddrinfo(res_);
		res_ = nullptr;
	}
	res_ = resolved;

	int sockfd = ::socket(resolved->ai_family, resolved->ai_socktype, resolved->ai_protocol);
	if (sockfd < 0) {
		perror("Error creating socket");
		freeaddrinfo(res_);
		res_ = nullptr;
		return -1;
	}
	
	// TODO - handle this better, maybe seperate function
	// Set sockopts	
	if (opts != nullptr) {
		int level = translate_level(opts->level);
		int flag  = translate_flag(opts->flag);
		if (level < 0 || flag < 0) {
			std::cerr << "setsockopt: unknown level or flag\n";
		} else {
			int opt_value = opts->opt_value;
			if (::setsockopt(sockfd, level, flag, &opt_value, sizeof(opt_value)) < 0) {
				perror("setsockopt");
			}
		}
	} else {
		int opt = 1;
		if (::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
			perror("setsockopt");
		}
	}

	return sockfd;
}

/* Binds socket to address
 *	- takes as paramater an already creatd socket - sockfd
 *	- also takes the addrinfo or nullptr
 */
int Socket::bind(int sockfd, const struct addrinfo* res) {
	const struct addrinfo* target = (res != nullptr) ? res : res_;

	if (target == nullptr) {
		errno = EINVAL;
		perror("bind");
		::close(sockfd);
		return -1;
	}

	if (::bind(sockfd, target->ai_addr, target->ai_addrlen) < 0) {
		perror("bind");
		::close(sockfd);
		return -1;
	}
	return sockfd;
}

/* 
 * Begins listening for client connection
 */
int Socket::listen(int sockfd, int backlog, const struct addrinfo*) {
	if (::listen(sockfd, backlog) < 0) {
		perror("listen");
		::close(sockfd);
		return -1;
	}
	return sockfd;
}

/* Accepts client connection
 *	- recommended to put inside server loop
 *	- takes a bound and listening socket
 *	- returns the client socket details
 */
int Socket::accept_connect(int sockfd) {
	sockaddr_storage client{};
	socklen_t len = sizeof(client);

	int clientfd = ::accept(sockfd, (sockaddr*)&client, &len);
	if (clientfd < 0) {
		perror("accept");
		return -1;
	}
	return clientfd;
}

/*
 * Connect to server
 *	- for client use only
 */
int Socket::connect(int sockfd, const struct addrinfo* res) {
	const struct addrinfo* target = (res != nullptr) ? res : res_;

	if (target == nullptr) {
		errno = EINVAL;
		perror("connect");
		::close(sockfd);
		return -1;
	}

	if (::connect(sockfd, target->ai_addr, target->ai_addrlen) < 0) {
		perror("connect");
		::close(sockfd);
		return -1;
	}
	return sockfd;
}
