#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <cerrno>

#include <iostream>

#include "server/alsocket.h"

Socket::Socket() : sockfd_(-1), res_(nullptr) {}

Socket::~Socket() {
	if (sockfd_ >= 0) {
		::close(sockfd_);
		sockfd_ = -1;
	}
	if (res_ != nullptr) {
		freeaddrinfo(res_);
		res_ = nullptr;
	}
}

Socket::Socket(Socket&& other) noexcept
	: sockfd_(other.sockfd_), res_(other.res_) {
	other.sockfd_ = -1;
	other.res_ = nullptr;
}

Socket& Socket::operator=(Socket&& other) noexcept {
	if (this != &other) {
		if (sockfd_ >= 0) ::close(sockfd_);
		if (res_ != nullptr) freeaddrinfo(res_);
		sockfd_ = other.sockfd_;
		res_ = other.res_;
		other.sockfd_ = -1;
		other.res_ = nullptr;
	}
	return *this;
}

/* 
 * Helpers for get sockopts:
*/
// TODO - finish all flags
int translate_flag(SetSockFlag flag) {
	switch (flag) {
		case SetSockFlag::ReuseAddr:
			return SO_REUSEADDR;
		case SetSockFlag::Broadcast:
			return SO_BROADCAST;
		case SetSockFlag::Debug:
			return SO_DEBUG;
		case SetSockFlag::KeepUp:
			return SO_KEEPALIVE;
		case SetSockFlag::NoRoute:
			return SO_DONTROUTE;
		case SetSockFlag::Linger:
			return SO_LINGER;
		case SetSockFlag::OutOB:
			return SO_OOBINLINE;
		case SetSockFlag::SndBuf:
			return SO_SNDBUF;
		case SetSockFlag::RcvBuf:
			return SO_RCVBUF;
		case SetSockFlag::MinOut:
			return SO_SNDLOWAT;
		case SetSockFlag::MinIn:
			return SO_RCVLOWAT;
		case SetSockFlag::TimeOut:
			return SO_SNDTIMEO;
		case SetSockFlag::TimeIn:
			return SO_RCVTIMEO;
		case SetSockFlag::NoSigPipe:
			return SO_NOSIGPIPE;
		case SetSockFlag::LingerSec:
			return SO_LINGER_SEC;
#ifdef __linux__
		case SetSockFlag::ABPF:
			return SO_ATTACH_FILTER;
		case SetSockFlag::AeBPF:
			return SO_ATTACH_BPF;
#endif
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
 *  	- service: port or protocol type - ex:"http" or "8080"
 * 	- Optional:
 *		- provide custom hints for address resolution
 * 	- returns -1 on error
 */
int Socket::create(
		const char* host,
		const char* service,
		const struct addrinfo* hints
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
	
	if (sockfd_ >= 0) ::close(sockfd_);
	sockfd_ = sockfd;
	return 0;
}

int Socket::setopts(const SockOpts* opts, int opts_count) {
	if (opts != nullptr && opts_count > 0) {
		for (int i = 0; i < opts_count; i++) {
			int level = translate_level(opts[i].level);
			int flag  = translate_flag(opts[i].flag);
			if (level < 0 || flag < 0) {
				std::cerr << "setsockopt: unknown level or flag\n";
				continue;
			}
			int opt_value = opts[i].opt_value;
			if (::setsockopt(
				sockfd_,
				level,
				flag,
				&opt_value, sizeof(opt_value)) < 0)
			{
				perror("setsockopt");
			}
		}
	} else {
		int opt = 1;
		if (::setsockopt(
			sockfd_, 
			SOL_SOCKET, 
			SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		{
			perror("setsockopt");
		}
	}
	return 0;
}

/* Binds socket to address */
int Socket::bind() {
	if (res_ == nullptr) {
		errno = EINVAL;
		perror("bind");
		return -1;
	}

	if (::bind(sockfd_, res_->ai_addr, res_->ai_addrlen) < 0) {
		perror("bind");
		return -1;
	}
	return 0;
}

/* 
 * Begins listening for client connection
 */
int Socket::listen(int backlog) {
	if (::listen(sockfd_, backlog) < 0) {
		perror("listen");
		return -1;
	}
	return 0;
}

/* Accepts client connection
 *	- recommended to put inside server loop
 *	- returns the client fd
 */
int Socket::accept() {
	sockaddr_storage client{};
	socklen_t len = sizeof(client);

	int clientfd = ::accept(sockfd_, (sockaddr*)&client, &len);
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
int Socket::connect() {
	if (res_ == nullptr) {
		errno = EINVAL;
		perror("connect");
		return -1;
	}

	if (::connect(sockfd_, res_->ai_addr, res_->ai_addrlen) < 0) {
		perror("connect");
		return -1;
	}
	return 0;
}

int Socket::fd() const {
	return sockfd_;
}
