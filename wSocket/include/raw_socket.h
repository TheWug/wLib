#pragma once

#include <unistd.h>
#include <sys/socket.h>

#include <string>

#include "wSocket/include/socket.h"

namespace w
{

class RawSocket : public Socket
{
private:
	sockaddr_storage address;
	std::string hostname;
	int port;
	
public:
	RawSocket(const Interval &timeout = BaseIO::default_timeout_infinity);
	RawSocket(const RawSocket &other);
	virtual ~RawSocket();

	using Socket::Bind;
	virtual void Bind(const std::string &host, const char *port_or_service);
	
	virtual void AsyncConnect();
	virtual void Connect(int timeout_millis);
	virtual void Disconnect();
	
	int WaitForSocket(int flags, int timeout);
	int WaitReady(int timeout);
	
	using BaseIO::Read;
	using BaseIO::Write;
	virtual size_t Write(const char *buffer, size_t length);
	virtual size_t Read(char *buffer, size_t length);

	virtual RawSocket *Clone() const;
};

} // namespace w
