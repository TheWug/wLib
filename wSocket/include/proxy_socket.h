#pragma once

#include "wSocket/include/socket.h"

namespace w
{

class ProxySocket : public Socket
{
public:
	ProxySocket(const Interval &timeout = BaseIO::default_timeout_infinity);
	ProxySocket(const ProxySocket &other);
	ProxySocket();
	virtual ~ProxySocket();

	using Socket::Bind;
	virtual void Bind(const std::string &host, int port);
	virtual void Bind(const std::string &host, const char *port_or_service);

	virtual void BindProxy(const std::string &host, int port);
	virtual void BindProxy(const std::string &host, const char *port) = 0;
	virtual void BindProxy(const std::string &host, const std::string &port);
	
	using Socket::Connect;
	virtual void Connect(int timeout_millis) = 0;
	virtual void Disconnect() = 0;

	virtual ProxySocket *Clone() const = 0;

protected:
	std::string endpoint_port;
	std::string endpoint_host;
};

} // namespace w
