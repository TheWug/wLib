#pragma once

#include "wTime/include/interval.h"

#include <string>
#include <vector>
#include <exception>
#include <stdexcept>

#include <sys/types.h>
#include <errno.h>

namespace w
{

typedef int fd_t;
typedef int ErrorCode;

class IOException : public std::runtime_error
{
public:
	IOException(const std::string &desc, int code = -1);
	IOException &operator=(const IOException &other);
	virtual ~IOException() throw();

	static IOException MakeException(const std::string &hint, int errno_val);

	ErrorCode errorcode;
};

class Descriptor
{
public:
	class DescriptorImpl
	{
	public:
		DescriptorImpl(fd_t desc);
		~DescriptorImpl();
		fd_t descriptor;
		int refcount;
	};

	Descriptor(int fileno = -1);
	Descriptor(const Descriptor &other);
	Descriptor &operator=(const Descriptor &other);
	~Descriptor();

	bool operator==(const Descriptor &other) const;

	int GetNumber() const;

	static const Descriptor stdin;
	static const Descriptor stdout;
	static const Descriptor stderr;

	static const Descriptor invalid;

private:
	void Reference(DescriptorImpl *p) const;
	mutable DescriptorImpl *actual;
};

class BaseIO
{
	friend class Monitor;
protected:
	BaseIO(const w::Interval& time_out = default_timeout_infinity);
	BaseIO(const BaseIO& other);

	// callbacks.  override these to provide functionality
	virtual void OnReadHangup();
	virtual void OnClose();
	virtual void OnError(const IOException &exception);
	virtual void OnTimeout();

	virtual void OnRead(char *buffer, int length);
	virtual void OnReadLine(char *buffer, int length);

	// initialization
	void SetReadDescriptor(const Descriptor &r_desc);
	void SetWriteDescriptor(const Descriptor &w_desc);

	std::vector<char> readline_buffer;
	size_t readline_buffer_max;

public:
	// this will be called by the io manager, if you use one
	// must not block if there is no data available
	virtual void ReadService();

	virtual ~BaseIO();

	// this creates a duplicate of the object on the heap.  You MUST overload this method for all instantiable subclasses.
	virtual BaseIO *Clone() const = 0;

	// use these functions to read from and write to the IO object.
	// if an io manager is being used for an object, it will handle reading but not writing.
	size_t Write(const std::string &buffer);
	size_t Read(std::string &buffer);
	virtual size_t Write(const char *buffer, size_t length) = 0;
	virtual size_t Read(char *buffer, size_t length) = 0;

	// be careful with these.  you can use them to do some mean stuff.
	// nonetheless they need to be exposed, since system calls all rely on having the actual file descriptor, and these are the only way to get it
	const Descriptor &GetReadDescriptor() const;
	const Descriptor &GetWriteDescriptor() const;

	// change the timeout on existing baseios
	void SetTimeout(const w::Interval& time);
	const w::Interval& GetTimeout();

	// change the buffer size on existing baseios
	void SetBufSize(ssize_t size);
	ssize_t GetBufSize();

	static const Interval default_timeout_infinity;

private:
	Descriptor read_descriptor;
	Descriptor write_descriptor;
	Interval timeout;

	// special values:
	static const size_t buffer_infinity = -1;
	static const size_t buffer_none = 0;
};

} // namespace w
