#pragma once

#include "wIO/include/io_pipe.h"
#include "wString/include/string.h"

#include <vector>

namespace w
{

class ProcessArgs;

class IOProcess
{
public:
	IOProcess(const Interval &timeout = BaseIO::default_timeout_infinity);
	~IOProcess();

	void Exec(std::string program, std::vector<String> arguments = std::vector<String>());
	void Exec(const ProcessArgs &args);

	void Kill(int signum = 9);
	void Close();

	IOPipe &GetStdin();
	IOPipe &GetStdout();
	IOPipe &GetStderr();

	void RedirectStdin(const Descriptor &d);
	void RedirectStdout(const Descriptor &d);
	void RedirectStderr(const Descriptor &d);

private:
	Descriptor in;
	Descriptor out;
	Descriptor err;

	IOPipe std_in;
	IOPipe std_out;
	IOPipe std_err;

	Interval pipe_timeout;

	bool started;
};

}
