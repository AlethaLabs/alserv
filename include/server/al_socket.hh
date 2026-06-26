#pragma once

#include <sys/socket.h>
#include <netdb.h>
#include <sys/uio.h>
#include <filesystem>

// Some sockopt flags only work for linux
enum class SetSockFlag : int
{
	Debug,	   // SO_DEBUG       	enables recording of debugging information
	ReuseAddr, // SO_REUSEADDR   	enables local address reuse
	ReusePort, // SO_REUSEPORT  	enables duplicate address and port bindings
	KeepUp,	   // SO_KEEPALIVE   	enables keep connections alive
	NoRoute,   // SO_DONTROUTE   	enables routing bypass for outgoing messages
	Linger,	   // SO_LINGER	     	linger on close if data present
	Broadcast, // SO_BROADCAST   	enables permission to transmit broadcast messages
	OutOB,	   // SO_OOBINLINE   	enables reception of out-of-band data in ban
	SndBuf,	   // SO_SNDBUF	     	set buffer size for output
	RcvBuf,	   // SO_RCVBUF      	set buffer size for input
	MinOut,	   // SO_SNDLOWAT    	set minimum count for output
	MinIn,	   // SO_RCVLOWAT    	set minimum count for input
	TimeOut,   // SO_SNDTIMEO    	set timeout value for output
	TimeIn,	   // SO_RCVTIMEO	set timeout value for input

#if defined(__linux__) || defined(__LINUX__)
	ABPF,  // SO_ATTACH_FILTER - takes structure sock_fprog
	AeBPF, // SO_ATTACH_BPF - same as above

#elif defined(__APPLE__)
	NoSigPipe, // SO_NOSIGPIPE   do not generate SIGPIPE, instead return EPIPE
	LingerSec, // SO_LINGER_SEC  linger on close if data present with timeout in seconds
#endif
};

// TODO - Add all error flags
enum class Errors {

};

// TODO - Add all socket levels
enum class SetSockLevel : int {
	Socket, // SOL_SOCKET - all types - default
};

// TODO - Add all readonly flags
enum class GetSockInfo : int {
	IsListen, // SO_ACCEPTCONN 
	// SO_TYPE         get the type of the socket (get only)
	// SO_ERROR        get and clear error on the socket (get only)
	// SO_NREAD        number of bytes to be read (get only)
	// SO_NWRITE       number of bytes written not yet sent by the protocol (get only)
};

// TODO - Need to exted this to accept things such as bpf
struct SockOpts {
	SetSockFlag flag;
	SetSockLevel level;
	int opt_value;
};

class Socket {
public:
	Socket();
	~Socket();

	// No duplicating sockets
	Socket(const Socket &) = delete;
	Socket &operator=(const Socket &) = delete;

	// Move sockets
	Socket(Socket &&) noexcept;
	Socket &operator=(Socket &&) noexcept;

	/* ------------ Set up socket ------------ */
	int create(
	    const char *host,
	    const char *service,
	    const struct addrinfo *hints = nullptr);

	int setopts(const SockOpts *opts = nullptr, int opts_count = 0);
	int bind();
	int listen(int backlog = 5);
	int accept();
	int connect();

	int fd() const;

	/* ------------ Send data through socket ----------- */
#if defined(__linux__) || defined(__LINUX__)
	auto send_linux_file(const char *filepath);
#elif defined(__APPLE__)
	bool send_apple_file(std::filesystem::path filepath, struct sf_hdtr *meta = nullptr);
#endif
private:
	int sockfd_;
	struct addrinfo *res_;
};
