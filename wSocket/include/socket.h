#pragma once

#include "wIO/include/base_io.h"

namespace w
{

class Socket : public BaseIO
{
	friend class Monitor;
public:
	Socket(const Interval &timeout = BaseIO::default_timeout_infinity);
	Socket(const Socket &other);
	virtual ~Socket();
	
	virtual void Bind(const std::string &host, int port);
	virtual void Bind(const std::string &host, const char *port_or_service) = 0;
	virtual void Bind(const std::string &host, const std::string &port_or_service);
	
	virtual void Connect(int timeout_millis) = 0;
	virtual void Connect();
	virtual void AsyncConnect() = 0;
	virtual void Disconnect() = 0;

	using BaseIO::Read;
	using BaseIO::Write;
	virtual size_t Write(const char *buffer, size_t length) = 0;
	virtual size_t Read(char *buffer, size_t length) = 0;

	virtual Socket *Clone() const = 0;

protected:
	//callbacks, override these to provide functionality when using the Monitor class
	virtual void OnEstablish(); // called when a connection completes and becomes ready (specific to Sockets)

	//virtual void OnReadHangup();  // called when the reading end of an iobase closes
	//virtual void OnClose();       // called when an iobase is terminated
	//virtual void OnError(const IOException &exception); // called when an error occurs that terminates an iobase (OnClose is NOT called)
	//virtual void OnTimeout();     // called when a timeout occurs and an iobase is discarded
};

} // namespace w

#define EBINDFAIL	0x1001 /* Bind failed, could not bind address to socket */
