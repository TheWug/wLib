#include "wIO/include/base_io.h"
#include "wString/include/string.h"
#include "wIO/include/std_io.h"

#include <algorithm>
#include <iostream>

#include <unistd.h>
#include <string.h>

using namespace std;

namespace w
{

IOException::IOException(const string &desc, int code) : runtime_error(desc.c_str()), errorcode(code)
{
}

IOException &IOException::operator=(const IOException &other)
{
	errorcode = other.errorcode;
}

IOException::~IOException() throw()
{
}

IOException IOException::MakeException(const string &hint, int errno_val)
{
	char error_desc_buf[512] = {0};
	char *foo = strerror_r(errno_val, error_desc_buf, sizeof(error_desc_buf));
	string message = hint.empty() ? foo : (hint + ": " + foo);
	message += " (";
	message += String::ToString(errno_val);
	message += ")";
	return IOException(message, errno_val);
}

Descriptor::DescriptorImpl::DescriptorImpl(fd_t desc) : descriptor(desc), refcount(0)
{
}

Descriptor::DescriptorImpl::~DescriptorImpl()
{
}

const Descriptor Descriptor::stdin(0);
const Descriptor Descriptor::stdout(1);
const Descriptor Descriptor::stderr(2);
const Descriptor Descriptor::invalid(-1);

Descriptor::Descriptor(fd_t fileno) : actual(NULL)
{
	if (fileno >= 0)
	{
		DescriptorImpl *d = new DescriptorImpl(fileno);
		Reference(d);
	}
}

Descriptor::Descriptor(const Descriptor &other) : actual(NULL)
{
	Reference(other.actual);
}

Descriptor &Descriptor::operator=(const Descriptor &other)
{
	Reference(other.actual);
}

bool Descriptor::operator==(const Descriptor &other) const
{
	return GetNumber() == other.GetNumber();
}

Descriptor::~Descriptor()
{
	Reference(NULL);
}

fd_t Descriptor::GetNumber() const
{
	return actual ? actual->descriptor : -1;
}

void Descriptor::Reference(DescriptorImpl *p) const
{
	DescriptorImpl *deleteme = actual;
	actual = p;
	if (actual != NULL)
	{
		++actual->refcount;
	}
	if (deleteme != NULL)
	{
		--deleteme->refcount;
		if (deleteme->refcount < 1)
		{
			if (deleteme->descriptor != -1) close(deleteme->descriptor);
			delete deleteme;
		}
	}
}

BaseIO::BaseIO(const w::Interval& time_out) : timeout(time_out), read_descriptor(-1), write_descriptor(-1), readline_buffer_max(0)
{
}

BaseIO::BaseIO(const BaseIO& other) : read_descriptor(other.GetReadDescriptor()), write_descriptor(other.GetWriteDescriptor()), 
				      readline_buffer(other.readline_buffer), readline_buffer_max(other.readline_buffer_max), timeout(other.timeout)
{
}

BaseIO::~BaseIO()
{
}

void BaseIO::SetReadDescriptor(const Descriptor &r_desc)
{
	read_descriptor = r_desc;
}

void BaseIO::SetWriteDescriptor(const Descriptor &w_desc)
{
	write_descriptor = w_desc;
}

const Descriptor &BaseIO::GetReadDescriptor() const
{
	return read_descriptor;
}

const Descriptor &BaseIO::GetWriteDescriptor() const
{
	return write_descriptor;
}

void BaseIO::SetTimeout(const w::Interval& time)
{
	timeout = time;
}

const w::Interval& BaseIO::GetTimeout()
{
	return timeout;
}
void BaseIO::SetBufSize(ssize_t size)
{
	readline_buffer_max = size;
}

ssize_t BaseIO::GetBufSize()
{
	return readline_buffer_max;
}

void BaseIO::OnClose() {}
void BaseIO::OnError(const IOException &exception) {}
void BaseIO::OnReadHangup() {}
void BaseIO::OnTimeout() {}
void BaseIO::OnRead(char *buffer, int length) {}
void BaseIO::OnReadLine(char *buffer, int length) {}

size_t BaseIO::Write(const std::string &buffer)
{
	return Write(buffer.data(), buffer.size());
}

size_t BaseIO::Read(std::string &buffer)
{
	return Read(buffer.size() == 0 ? NULL : &buffer[0], buffer.size());
}



void BaseIO::ReadService()
{
	static const char eol[2] = {'\r', '\n'};
	vector<char> readbuffer(4096);
	int bytes_read = Read(readbuffer.data(), 4096);
	if (bytes_read == 0) throw IOException("End of file.", ENODATA);
	if (bytes_read < 0) throw IOException::MakeException("Error during read service", errno);
	try
	{
		OnRead(readbuffer.data(), bytes_read);
	}
	catch (...) {} // ignore any exception thrown by this
	if (GetBufSize() == BaseIO::buffer_none) return;
	vector<char>::iterator rb_cursor = readbuffer.begin();
	vector<char>::iterator rb_end = rb_cursor + bytes_read;
	while (true)
	{
		vector<char>::iterator newline_pos = find_first_of(rb_cursor, rb_end, eol, eol + 2);
		readline_buffer.insert(readline_buffer.end(), rb_cursor, newline_pos);
		if (newline_pos != rb_end) 
		{
			try
			{
				OnReadLine(readline_buffer.data(), readline_buffer.size());
			}
			catch (...) {} // ignore any exception thrown by this
			readline_buffer.clear();
		}
		else break;
		rb_cursor = newline_pos;
		if (*rb_cursor == '\r') ++rb_cursor;
		if (rb_cursor != rb_end && *rb_cursor == '\n') ++rb_cursor;
	}
	if (readline_buffer.size() > readline_buffer_max && readline_buffer_max != -1)
	{
		readline_buffer.clear();
		throw IOException::MakeException("Readline buffer overrun", EMSGSIZE);
	}
}

const Interval BaseIO::default_timeout_infinity = Interval(1, 0, true);

}
