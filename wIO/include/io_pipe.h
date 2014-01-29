#pragma once

#include "wIO/include/base_io.h"

#include <string>
#include <fstream>

namespace w
{

class IOPipe : public BaseIO
{
public:
	IOPipe(const Interval &timeout = BaseIO::default_timeout_infinity);
	IOPipe(const IOPipe &other);
	~IOPipe();

	void Open(int flags = 0);
	void Open(IOPipe &other, int flags = 0);
	void Close();
	void CloseRead();
	void CloseWrite();

	using BaseIO::Read;
	using BaseIO::Write;
	virtual size_t Write(const char *buffer, size_t length);
	virtual size_t Read(char *buffer, size_t length);

	virtual IOPipe *Clone() const;
};

}
