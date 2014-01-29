#include "wSocket/include/socket.h"
#include "wString/include/string.h"

#include <string>
#include <sstream>

using namespace std;

namespace w
{

Socket::Socket(const Interval &timeout) : BaseIO(timeout)
{
}

Socket::Socket(const Socket &other) : BaseIO(other)
{
}

Socket::~Socket()
{
}

void Socket::Connect()
{
	return Connect(10000);
}

void Socket::Bind(const std::string &h, int p)
{
	Bind(h, String::ToString(p));
}

void Socket::Bind(const std::string &host, const std::string &port_or_service)
{
	Bind(host, port_or_service.c_str());
}

void Socket::OnEstablish() {}

} // namespace w
