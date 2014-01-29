#include "wIO/include/std_io.h"

#include <unistd.h>

namespace w
{

IOStdStream::IOStdStream(int number) : fileno(number)
{
}

IOStdStream::IOStdStream(const IOStdStream &other) : BaseIO(other), fileno(other.fileno)
{
}

IOStdStream::~IOStdStream()
{
}

void IOStdStream::Update()
{
	if (fileno == -1) return;
	else if (fileno == 0) SetReadDescriptor(Descriptor::stdin);
	else if (fileno == 1) SetWriteDescriptor(Descriptor::stdout);
	else if (fileno == 2) SetWriteDescriptor(Descriptor::stderr);
	fileno = -1;
}

IOStdStream *IOStdStream::Clone() const
{
	return new IOStdStream(*this);
}

size_t IOStdStream::Write(const char *buffer, size_t length)
{
	Update();
	fd_t d = GetWriteDescriptor().GetNumber();
	if (d == -1) throw IOException::MakeException("File not open for writing", EPERM);
	ssize_t written = write(d, buffer, length);
	if (written < 0) throw IOException::MakeException("Error during IOStdStream::Write", errno);
	return written;
}

size_t IOStdStream::Read(char *buffer, size_t length)
{
	Update();
	fd_t d = GetReadDescriptor().GetNumber();
	if (d == -1) throw IOException::MakeException("File not open for reading", EPERM);
	ssize_t readed = read(d, buffer, length);
	if (readed < 0) throw IOException::MakeException("Error during IOStdStream::Read", errno);
	return readed;
}

IOStdStream IOStdStream::win(0);
IOStdStream IOStdStream::wout(1);
IOStdStream IOStdStream::werr(2);

IOStdStream &win(IOStdStream::win);
IOStdStream &wout(IOStdStream::wout);
IOStdStream &werr(IOStdStream::werr);

}
