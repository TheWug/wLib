#include "wMonitor/include/monitor.h"
#include "wSocket/include/socket.h"

#include "wIO/wIO.h"

#include <iostream>

using namespace std;

namespace w
{

Monitor::Monitor(size_t max) : epoll_fd(Descriptor::invalid), ready_events(max)
{
}

Monitor::~Monitor()
{
}

void Monitor::Activate()
{
	int desc = epoll_create1(0);
	if (desc == -1) throw IOException::MakeException("Failed to create epoll descriptor", errno);
	epoll_fd = Descriptor(desc);
}

void Monitor::Deactivate()
{
	epoll_fd = Descriptor::invalid;
	
}

BaseIO *Monitor::AddDataSource(const BaseIO &io)
{
	if (io.GetReadDescriptor() == Descriptor::invalid) throw IOException("Failed to add IO object to monitor: Object invalid.", EBADFD);
	BaseIO *iob = io.Clone();
	try
	{
		active_sources.insert(iob);
		epoll_event epe;
		epe.events = EPOLLIN | EPOLLRDHUP | EPOLLHUP;
		epe.data.ptr = iob;
		int response = epoll_ctl(epoll_fd.GetNumber(), EPOLL_CTL_ADD, io.GetReadDescriptor().GetNumber(), &epe);
		if (response == -1) throw IOException::MakeException("Failed to add object to epoll descriptor", errno);
	}
	catch (IOException &ioe)
	{
		active_sources.erase(iob);
		delete iob;
		throw;
	}
	return iob;
}

Socket *Monitor::ConnectDataSocket(const Socket &sock)
{
	Socket *ios = sock.Clone();
	
	try
	{
		connecting_sockets.insert(ios);
		epoll_event epe;
		epe.events = EPOLLOUT | EPOLLRDHUP;
		epe.data.ptr = ios;
		ios->AsyncConnect();
		int response = epoll_ctl(epoll_fd.GetNumber(), EPOLL_CTL_ADD, ios->GetWriteDescriptor().GetNumber(), &epe);
		if (response == -1) throw IOException::MakeException("Failed to add socket to epoll descriptor", errno);
	}
	catch (IOException &ioe)
	{
		connecting_sockets.erase(ios);
		delete ios;
		throw;
	}
	return ios;
}

int Monitor::ServiceRoutine(const Interval &timeout)
{
	int num_ready = epoll_wait(epoll_fd.GetNumber(), &ready_events[0], ready_events.size(), timeout.ToMillis());
	if (num_ready == -1)
	{
		if (errno == EINTR) return false; // this condition would only be possible if I used epoll_pwait, and would happen if a signal interrupted the wait
		else throw IOException::MakeException("Error while waiting for IO", errno);
	}

	if (num_ready == 0) return false;

	for (int i = 0; i < num_ready; ++i)
	{
		if (ready_events[i].events & (EPOLLOUT)) // if we get "ready for output", this is a socket and we were waiting for a connect, so move it to the main group
		{
			Socket *sock = (Socket *) ready_events[i].data.ptr;
			ready_events[i].events = EPOLLIN | EPOLLPRI | EPOLLRDHUP | EPOLLHUP;
			try {
				if (epoll_ctl(epoll_fd.GetNumber(), EPOLL_CTL_DEL, sock->GetWriteDescriptor().GetNumber(), &ready_events[i]) == -1)
					throw IOException::MakeException("Failed to adjust socket in epoll descriptor", errno);
				if (epoll_ctl(epoll_fd.GetNumber(), EPOLL_CTL_ADD, sock->GetReadDescriptor().GetNumber(), &ready_events[i]) == -1)
					throw IOException::MakeException("Failed to adjust socket in epoll descriptor (socket dropped)", errno);
				connecting_sockets.erase(sock);
				active_sources.insert(sock);
			}
			catch (IOException &ioe)
			{
				IOError(sock, ioe);
				RemoveSocket(sock);
				throw;
			}
			SocketConnect(sock);
		}
		else if (ready_events[i].events & (EPOLLIN)) // we got regular data
		{
			BaseIO *io = (BaseIO *) ready_events[i].data.ptr;
			try { io->ReadService(); }
			catch (IOException &ioe)
			{
				if (ioe.errorcode == ENODATA) IOClose(io); // end of file, not an actual error 
				else IOError(io, ioe); // some error occurred while trying to read
				RemoveObject(io);
			}
		}
		else if (ready_events[i].events & (EPOLLRDHUP)) // other end hung up on its sending end and there is nothing left to read
		{
			BaseIO *io = (BaseIO *) ready_events[i].data.ptr;
			IOReadHangup(io);
		}
		else if (ready_events[i].events & (EPOLLHUP | EPOLLERR)) // hangup
		{
			BaseIO *io = (BaseIO *) ready_events[i].data.ptr;
			IOClose(io);
			RemoveObject(io);
		}
	}
	return true;
}

size_t Monitor::GetMaxEvents()
{
	return ready_events.size();
}

void Monitor::SetMaxEvents(size_t max)
{
	return ready_events.resize(max);
}

void Monitor::IOClose(BaseIO *io)
{
	io->OnClose();
}

void Monitor::IOReadHangup(BaseIO *io)
{
	io->OnReadHangup();
}

void Monitor::IOError(BaseIO *io, const IOException &ioe)
{
	io->OnError(ioe);
}

void Monitor::IOTimeout(BaseIO *io)
{
	io->OnTimeout();
}

void Monitor::SocketConnect(Socket *sock)
{
	sock->OnEstablish();
}

void Monitor::RemoveSocket(Socket *sock)
{
	connecting_sockets.erase(sock);
	epoll_ctl(epoll_fd.GetNumber(), EPOLL_CTL_DEL, sock->GetWriteDescriptor().GetNumber(), NULL);
	RemoveObject(sock);
}

void Monitor::RemoveObject(BaseIO *io)
{
	active_sources.erase(io);
	epoll_ctl(epoll_fd.GetNumber(), EPOLL_CTL_DEL, io->GetReadDescriptor().GetNumber(), NULL);
	delete io;
}

void Monitor::Check()
{
	wout.Write("epoll fd: " + String::ToString(epoll_fd.GetNumber()) + "\n");
}

}
