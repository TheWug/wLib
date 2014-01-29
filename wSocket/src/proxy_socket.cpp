#include "wSocket/include/proxy_socket.h"
#include "wString/include/string.h"

#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>

#include <sstream>

namespace w
{

ProxySocket::ProxySocket(const Interval &interval) : Socket(interval)
{
}

ProxySocket::ProxySocket(const ProxySocket &other) : Socket(other), endpoint_port(other.endpoint_port), endpoint_host(other.endpoint_host)
{
}

ProxySocket::~ProxySocket()
{
}

void ProxySocket::Bind(const std::string &host, const char *port_or_service)
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
	
	if (((sockaddr_storage *)ai->ai_addr)->ss_family == AF_INET) Bind(host, ((sockaddr_in *)ai->ai_addr)->sin_port);
	else if (((sockaddr_storage *)ai->ai_addr)->ss_family == AF_INET6) Bind(host, ((sockaddr_in6 *)ai->ai_addr)->sin6_port);
	freeaddrinfo(ai);
	return;
}

void ProxySocket::Bind(const std::string &host, int port)
{
	endpoint_host = host;
	endpoint_port = String::ToString(port);
}

void ProxySocket::BindProxy(const std::string &host, int p)
{
	std::ostringstream str_port;
	str_port << p;
	return BindProxy(host, str_port.str());
}

void ProxySocket::BindProxy(const std::string &host, const std::string &p)
{
	return BindProxy(host, p.c_str());
}

} // namespace w
