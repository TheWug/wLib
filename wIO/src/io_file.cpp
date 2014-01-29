#include "wIO/include/io_file.h"

#include <iostream>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

namespace w
{

IOFile::IOFile(const std::string &filename, std::ios_base::openmode mode, const Interval &timeout) : BaseIO(timeout)
{
	Open(filename, mode);
}

IOFile::IOFile(const Interval &timeout) : BaseIO(timeout)
{
}

IOFile::IOFile(const IOFile &other) : BaseIO(other)
{
}

IOFile::~IOFile()
{
}

void IOFile::Open(const std::string &filename, std::ios_base::openmode mode)
{
	char error_desc_buf[256] = {0};
	bool readmode = !!(mode & ios_base::in);
	bool writemode = !!(mode & ios_base::out);
	bool appendmode = !!(mode & ios_base::app);
	bool seekend = !!(mode & ios_base::ate);
	bool truncate = !!(mode & ios_base::trunc);
	if (!readmode && !writemode) readmode = true;

	int flags =     (readmode ? (writemode ? O_RDWR : O_RDONLY) : (writemode ? O_WRONLY : O_RDONLY)) |
			(appendmode ? O_APPEND : 0) |
			(truncate ? O_TRUNC : 0) |
			(O_CREAT);

	Descriptor fileno(open(filename.c_str(), flags));
	if (fileno.GetNumber() == -1) throw IOException::MakeException(filename, errno);

	if (readmode) SetReadDescriptor(fileno); else SetReadDescriptor(Descriptor(-1));
	if (writemode) SetWriteDescriptor(fileno); else SetWriteDescriptor(Descriptor(-1));
}

void IOFile::Close()
{
	SetReadDescriptor(-1);
	SetWriteDescriptor(-1);
}

size_t IOFile::Write(const char *buffer, size_t length)
{
	fd_t d = GetWriteDescriptor().GetNumber();
	if (d == -1) throw IOException::MakeException("File not open for writing", EPERM);
	ssize_t written = write(d, buffer, length);
	if (written < 0) throw IOException::MakeException("Error during IOFile::Write", errno);
	return written;
}

size_t IOFile::Read(char *buffer, size_t length)
{
	fd_t d = GetReadDescriptor().GetNumber();
	if (d == -1) throw IOException::MakeException("File not open for reading", EPERM);
	ssize_t readed = read(d, buffer, length);
	if (readed < 0) throw IOException::MakeException("Error during IOFile::Read", errno);
	return readed;
}

IOFile *IOFile::Clone() const
{
	return new IOFile(*this);
}

}
