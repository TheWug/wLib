#include "wIO/include/io_pipe.h"

#include <iostream>
#include <exception>
#include <stdexcept>

#include <unistd.h>

using namespace std;

namespace w
{

IOPipe::IOPipe(const Interval &timeout) : BaseIO(timeout)
{
}

IOPipe::IOPipe(const IOPipe &other) : BaseIO(other)
{
}

IOPipe::~IOPipe()
{
}

void IOPipe::Open(int flags)
{
	int pipes[2];
	int pipe_result = pipe2(pipes, flags);
	if (pipe_result < 0) throw IOException::MakeException("Anonymous pipe", errno);

	try
	{
		Descriptor readp(pipes[0]);
		Descriptor writep(pipes[1]);

		// these are nothrow
		SetReadDescriptor(readp);
		SetWriteDescriptor(writep);
	}
	catch (bad_alloc &e)
	{
		throw IOException::MakeException("Anonymous pipe", ENOMEM);
	}
}

void IOPipe::Open(IOPipe &other, int flags)
{
	int my_pipes[2], other_pipes[2];
	int pipe_result;

	pipe_result = pipe2(my_pipes, flags);
	if (pipe_result < 0) throw IOException::MakeException("Anonymous double-ended pipe", errno);
	pipe_result = pipe2(other_pipes, flags);
	if (pipe_result < 0) throw IOException::MakeException("Anonymous double-ended pipe", errno);

	try
	{
		Descriptor my_readp(my_pipes[0]);
		Descriptor my_writep(my_pipes[1]);
		Descriptor other_readp(other_pipes[0]);
		Descriptor other_writep(other_pipes[1]);

		// these are nothrow
		SetReadDescriptor(my_readp);
		SetWriteDescriptor(my_writep);
		other.SetReadDescriptor(other_readp);
		other.SetWriteDescriptor(other_writep);
	}
	catch (bad_alloc &e)
	{
		throw IOException::MakeException("Anonymous double-ended pipe", ENOMEM);
	}
}

void IOPipe::Close()
{
	CloseRead();
	CloseWrite();
}

void IOPipe::CloseRead()
{
	SetReadDescriptor(-1);
}

void IOPipe::CloseWrite()
{
	SetWriteDescriptor(-1);
}

size_t IOPipe::Write(const char *buffer, size_t length)
{
	ssize_t written = write(GetWriteDescriptor().GetNumber(), buffer, length);
	if (written < 0) throw IOException::MakeException("Error during IOPipe::Write", errno);
	return written;
}

size_t IOPipe::Read(char *buffer, size_t length)
{
	ssize_t readed = read(GetReadDescriptor().GetNumber(), buffer, length);
	if (readed < 0) throw IOException::MakeException("Error during IOPipe::Read", errno);
	return readed;
}

IOPipe *IOPipe::Clone() const
{
	return new IOPipe(*this);
}

}
