#pragma once

#include "wIO/include/base_io.h"

#include <string>
#include <fstream>

namespace w
{

class IOFile : public BaseIO
{
public:
	IOFile(const std::string &filename, std::ios_base::openmode mode = std::ios_base::in, const Interval &timeout = BaseIO::default_timeout_infinity);
	IOFile(const Interval &timeout = BaseIO::default_timeout_infinity);
	IOFile(const IOFile &other);
	~IOFile();

	void Open(const std::string &filename, std::ios_base::openmode mode = std::ios_base::in);
	void Close();

	using BaseIO::Read;
	using BaseIO::Write;
	virtual size_t Write(const char *buffer, size_t length);
	virtual size_t Read(char *buffer, size_t length);

	virtual IOFile *Clone() const;
};

}
