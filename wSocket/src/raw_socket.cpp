#include "wSocket/include/raw_socket.h"

#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>

#include <cstring>

#include <string>

namespace w
{

RawSocket::RawSocket(const Interval &timeout) : Socket(timeout)
{
}
RawSocket::RawSocket(const RawSocket &other) : Socket(other), address(other.address), hostname(other.hostname), port(other.port)
{
}

RawSocket::~RawSocket()
{
}

void RawSocket::Bind(const std::string &host, const char *port_or_service)
{
	addrinfo *ai = NULL;
	addrinfo hint;
	memset(&hint, 0, sizeof hint);
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	int did_gai_fail = getaddrinfo(host.c_str(), port_or_service, &hint, &ai);
	
	if (did_gai_fail) 
	{
		if (ai) freeaddrinfo(ai);
		throw IOException("Failed to bind socket: Lookup failed.", EBINDFAIL);
	}
	
	if (ai->ai_family != AF_INET && ai->ai_family != AF_INET6)
	{
		freeaddrinfo(ai);
		throw IOException("Failed to bind socket: Protocol was not IPv4 or IPv6.", EBINDFAIL);
	}
	
	memcpy(&address, ai->ai_addr, (ai->ai_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));
	hostname = host;
	if (address.ss_family == AF_INET) port = ((sockaddr_in *)&address)->sin_port;
	else if (address.ss_family == AF_INET6) port = ((sockaddr_in6 *)&address)->sin6_port;
	freeaddrinfo(ai);
	return;
}

void RawSocket::AsyncConnect()
{
	int flags;
	int sock = socket((address.ss_family == AF_INET6) ? AF_INET6 : AF_INET, SOCK_STREAM, 0);
	if (sock < 0) throw IOException::MakeException("Failed to initialize socket", errno);

	Descriptor sockdesc(sock);
	if(flags = fcntl(sock, F_GETFL, NULL) < 0) throw IOException::MakeException("Failed to get socket flags", errno);
	if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) < 0) throw IOException::MakeException("Failed to set socket nonblocking", errno);
	int connectstate = connect(sock, (sockaddr *) &address, (address.ss_family == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6));
	if (connectstate < 0 && errno != EINPROGRESS) throw IOException::MakeException("Failed to connect socket", errno);

	SetReadDescriptor(sockdesc);
	SetWriteDescriptor(sockdesc);
}

void RawSocket::Connect(int timeout_millis)
{
	int flags;
	AsyncConnect();

	try
	{
		if((flags = fcntl(GetReadDescriptor().GetNumber(), F_GETFL, NULL)) < 0) throw IOException::MakeException("Failed to get socket flags", errno);
		if (fcntl(GetReadDescriptor().GetNumber(), F_SETFL, flags & ~O_NONBLOCK) < 0) throw IOException::MakeException("Failed to set socket blocking", errno);
	
		int ready = WaitForSocket(POLLOUT, timeout_millis);
		if (ready == 0) throw IOException::MakeException("Failed to connect", ETIMEDOUT);
		else if (ready < 0) throw IOException::MakeException("Failed to connect", errno);
	}
	catch (std::exception &e)
	{
		Disconnect();
		throw;
	}
	
	return;
}

void RawSocket::Disconnect()
{
	SetReadDescriptor(-1);
	SetWriteDescriptor(-1);
	return;
}

int RawSocket::WaitForSocket(int flags, int timeout)
{
	struct pollfd d;
	d.fd = GetReadDescriptor().GetNumber();
	d.events = flags;
	d.revents = 0;
	errno = 0;

	return poll(&d, 1, timeout);
}

size_t RawSocket::Read(char *buf, size_t bufsize)
{	
	int bytes_read = recv(GetReadDescriptor().GetNumber(), buf, bufsize, 0);
	if (bytes_read < 0)
	{
		if (errno == EINTR) throw IOException::MakeException("Posix is stupid", errno); // what the even fuck? apparently read can just decide to read bytes and return -1 anyway. lets just pretend it doesnt say that and assume it read 0 bytes
		else
		{
			Disconnect();
			throw IOException::MakeException("Error while reading", errno);
		}
	}

	if (bytes_read == 0 && bufsize != 0) Disconnect();
	return bytes_read;
}

size_t RawSocket::Write(const char *buf, size_t bufsize)
{
	int bytes_sent = send(GetWriteDescriptor().GetNumber(), buf, bufsize, 0);
	
	if (bytes_sent < 0)
	{
		if (errno == EINTR) throw IOException::MakeException("Posix is stupid", errno); // what the even fuck? apparently read can just decide to read bytes and return -1 anyway. lets just pretend it doesnt say that and assume it read 0 bytes
		else
		{
			Disconnect();
			throw IOException::MakeException("Error while writing", errno);
		}
	}

	else if (bytes_sent == 0 && bufsize != 0) Disconnect();
	return bytes_sent;
}

RawSocket *RawSocket::Clone() const
{
	return new RawSocket(*this);
}

} // namespace w
