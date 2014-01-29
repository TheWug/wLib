#pragma once

#include "wIO/include/io_file.h"

namespace w
{

class IOStdStream : public BaseIO
{
private:
	IOStdStream(int number);
	void Update();

public:
	IOStdStream(const IOStdStream &other);
	virtual ~IOStdStream();

	virtual IOStdStream *Clone() const;

	using BaseIO::Read;
	using BaseIO::Write;
	virtual size_t Write(const char *buffer, size_t length);
	virtual size_t Read(char *buffer, size_t length);

	static IOStdStream win;
	static IOStdStream wout;
	static IOStdStream werr;

private:
	int fileno;
};

extern IOStdStream &win;
extern IOStdStream &wout;
extern IOStdStream &werr;

}
