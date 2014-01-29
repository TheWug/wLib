#pragma once

#include "wIO/include/base_io.h"

#include <set>
#include <vector>

#include <sys/epoll.h>

namespace w
{

class Interval;
class IOProcess;
class Socket;

class Monitor
{
public:
	Monitor(size_t max = 16);
	~Monitor();

	void Activate();
	void Deactivate();

	BaseIO *AddDataSource(const BaseIO &io);
	Socket *ConnectDataSocket(const Socket &sock);

	int ServiceRoutine(const Interval &timeout);

	void SetMaxEvents(size_t max);
	size_t GetMaxEvents();

	void RemoveSocket(Socket *sock);
	void RemoveObject(BaseIO *io);

	void Check();

private:
	// regular io callback helpers
	void IOClose(BaseIO *io);
	void IOReadHangup(BaseIO *io);
	void IOError(BaseIO *io, const IOException &ioe);
	void IOTimeout(BaseIO *io);

	// socket callback helpers
	void SocketConnect(Socket *sock);

	std::set<BaseIO *> active_sources;
	std::set<Socket *> connecting_sockets;

	std::vector<epoll_event> ready_events;

	Descriptor epoll_fd;
};

}
