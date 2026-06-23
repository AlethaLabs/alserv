#pragma once

#include <sys/socket.h>
#include <netdb.h>

// TODO - Add all sockopt flags - *** man 7 socket for linux ***
// Some sockopt flags only work for linux
enum class SetSockFlag : int
{
	Debug,	   // SO_DEBUG       enables recording of debugging information
	ReuseAddr, // SO_REUSEADDR   enables local address reuse
	ReusePort, // SO_REUSEPORT   enables duplicate address and port bindings
	KeepUp,	   // SO_KEEPALIVE   enables keep connections alive
	NoRoute,   // SO_DONTROUTE   enables routing bypass for outgoing messages
	Linger,	   // SO_LINGER	     linger on close if data present
	Broadcast, // SO_BROADCAST   enables permission to transmit broadcast messages
	OOBIN,	   // SO_OOBINLINE   enables reception of out-of-band data in ban
	SndBuf,	   // SO_SNDBUF	     set buffer size for output
	RcvBuf,	   // SO_RCVBUF      set buffer size for input
	MinOut,	   // SO_SNDLOWAT    set minimum count for output
	MinIn,	   // SO_RCVLOWAT    set minimum count for input
	TimeOut,   // SO_SNDTIMEO    set timeout value for output
	TimeIn,	   // SO_RCVTIMEO    set timeout value for input
	NoSigPipe, // SO_NOSIGPIPE   do not generate SIGPIPE, instead return EPIPE
	Linger,	   // SO_LINGER_SEC  linger on close if data present with timeout in seconds

#ifdef __linux__
	ABPF,  // SO_ATTACH_FILTER - takes structure sock_fprog
	AeBPF, // SO_ATTACH_BPF - same as above
#endif
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
*	int bound = socket.bind();
*	assert(bound >= 0);
*/
class Socket {
	public:
		Socket();
		~Socket();
		
		// No duplicating sockets
		Socket(const Socket&) = delete;
		Socket& operator=(const Socket&) = delete;

		// Move sockets
		Socket(Socket&&) noexcept;
		Socket& operator=(Socket&&) noexcept;

		// Creates socket
		int create(
			const char* host,
			const char* service,
			const struct addrinfo* hints = nullptr
		);

		int setopts(const SockOpts* opts = nullptr, int opts_count = 0);	
		int bind();
		int listen(int backlog = 5);
		int accept();
		int connect();

		int fd() const;

	private:
		int sockfd_;
		struct addrinfo* res_;
};
