#include "wSocket/include/ssh_proxy_socket.h"
#include "wArgv/include/process_args.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>

#include <string>
#include <sstream>
#include <iostream>

using namespace std;

namespace w
{

SSHProxySocket::SSHProxySocket(const Interval &interval) : ProxySocket(interval)
{
}

SSHProxySocket::SSHProxySocket(const SSHProxySocket &other) : ProxySocket(other), proxy(other.proxy), proxy_host(other.proxy_host), proxy_port(other.proxy_port)
{
}

SSHProxySocket::~SSHProxySocket()
{
}

void SSHProxySocket::BindProxy(const std::string &host, const char *port)
{
	proxy_host = host;
	proxy_port = port;
}

void SSHProxySocket::AsyncConnect()
{
	Connect();
}

void SSHProxySocket::Connect(int timeout_millis)
{
	int timeout_seconds = (timeout_millis + 999) / 1000; // this connect method does not support ms resolution
	ostringstream x;
	x << "ConnectTimeout=" << timeout_seconds;
	string s_tol = x.str();
	string s_target = (endpoint_host + ":" + endpoint_port);

	ProcessArgs args;
	args.SetName("ssh");
	args.SetArg(1, "-e");
	args.SetArg(2, "none");
	args.SetArg(3, "-W");
	args.SetArg(4, s_target);
	args.SetArg(5, "-o");
	args.SetArg(6, s_tol);
	args.SetArg(7, "-p");
	args.SetArg(8, proxy_port);
	args.SetArg(9, proxy_host);

	// for (int i = 0; i < 10; ++i) cout << args.GetArg(i) << " "; cout << endl;
	proxy.Exec(args);

	SetReadDescriptor(proxy.GetStdout().GetReadDescriptor());
	SetWriteDescriptor(proxy.GetStdin().GetWriteDescriptor());
}

void SSHProxySocket::Disconnect()
{
	proxy.Close();
	SetReadDescriptor(-1);
	SetWriteDescriptor(-1);
}

size_t SSHProxySocket::Read(char *buf, size_t bufsize)
{
	size_t bytes_read = read(GetReadDescriptor().GetNumber(), buf, bufsize);
	if (bytes_read == -1)
	{
		if (errno == EINTR) throw IOException::MakeException("Posix is stupid", errno); // what the even fuck? apparently read can just decide to read bytes and return -1 anyway. lets just pretend it doesnt say that and assume it read 0 bytes
		else
		{
			Disconnect();
			throw IOException::MakeException("Error while reading", errno);
		}
		return -1;
	}
	else if (bytes_read == 0 && bufsize != 0) Disconnect();
	return bytes_read;
}

size_t SSHProxySocket::Write(const char *buf, size_t bufsize)
{
	size_t bytes_sent = write(GetWriteDescriptor().GetNumber(), buf, bufsize);
	if (bytes_sent == -1)
	{
		if (errno == EINTR) throw IOException::MakeException("Posix is stupid", errno); // what the even fuck? apparently read can just decide to read bytes and return -1 anyway. lets just pretend it doesnt say that and assume it read 0 bytes
		else
		{
			Disconnect();
			throw IOException::MakeException("Error while writing", errno);
		}
		return -1;
	}
	else if (bytes_sent == 0) Disconnect();
	return bytes_sent;
}

SSHProxySocket *SSHProxySocket::Clone() const
{
	return new SSHProxySocket(*this);
}

} // namespace w
