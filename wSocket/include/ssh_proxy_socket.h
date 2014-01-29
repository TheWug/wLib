#pragma once

#include "wSocket/include/proxy_socket.h"
#include "wIO/include/io_process.h"

namespace w
{

class SSHProxySocket : public ProxySocket
{
public:
	SSHProxySocket(const Interval &interval = BaseIO::default_timeout_infinity);
	SSHProxySocket(const SSHProxySocket &other);
	virtual ~SSHProxySocket();

	using ProxySocket::BindProxy;
	virtual void BindProxy(const std::string &host, const char *port);
	
	using ProxySocket::Connect;
	virtual void AsyncConnect();
	virtual void Connect(int timeout_millis);
	virtual void Disconnect();

	using BaseIO::Read;
	using BaseIO::Write;
	virtual size_t Write(const char *buffer, size_t length);
	virtual size_t Read(char *buffer, size_t length);

	virtual SSHProxySocket *Clone() const;
	
protected:
	void KillSubprocess();

	IOProcess proxy;
	
	std::string proxy_host;
	std::string proxy_port;
};

} // namespace w
