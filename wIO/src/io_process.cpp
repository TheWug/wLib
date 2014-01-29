#include "wIO/include/io_process.h"
#include "wArgv/include/process_args.h"

#include <iostream>

#include <unistd.h>

using namespace std;

namespace w
{

IOProcess::IOProcess(const Interval &timeout) : pipe_timeout(timeout), in(-1), out(-1), err(-1), started(false)
{
}

IOProcess::~IOProcess()
{
}

// important: this function makes no attempt to close open file descriptors.
// if you have a file descriptor, it WILL be available to the child process unless you flag it close-on-exec.
void IOProcess::Exec(std::string program, std::vector<String> arguments)
{
	Exec(ProcessArgs(program, arguments));
}

void IOProcess::Exec(const ProcessArgs &args)
{
	if (started) throw IOException("Process already started: operation cancelled.", ECANCELED);

	IOPipe my_std_in(pipe_timeout), my_std_out(pipe_timeout), my_std_err(pipe_timeout);
	Descriptor my_in(in), my_out(out), my_err(err);

	if (in.GetNumber() == -1)
	{
		my_std_in.Open();
		my_in = my_std_in.GetReadDescriptor();
	}
	if (out.GetNumber() == -1)
	{
		my_std_out.Open();
		my_out = my_std_out.GetWriteDescriptor();
	}
	if (err.GetNumber() == -1)
	{
		my_std_err.Open();
		my_err = my_std_err.GetWriteDescriptor();
	}

	// if we get here, no exceptions have been thrown, so all of the pipes and descriptors are open

	pid_t forked = fork();
	if (forked == -1) throw IOException::MakeException("Error encountered during fork() call", errno);
	if (forked == 0) // child
	{
		// once we've gotten here, we can't throw exceptions
		// there is no way to communicate back to the parent that we've failed
		// our only option is to exit() and hope for the best

		if (my_in.GetNumber() != 0) dup2(my_in.GetNumber(), 0);
		if (my_out.GetNumber() != 1) dup2(my_out.GetNumber(), 1);
		if (my_err.GetNumber() != 2) dup2(my_err.GetNumber(), 2);

		my_in = Descriptor::invalid;  // close the original file descriptors (0, 1, 2 remain open)
		my_out = Descriptor::invalid; // we have to do this here since they are not destructed the normal way, since we never return
		my_err = Descriptor::invalid;

		my_std_in.Close();
		my_std_out.Close();
		my_std_err.Close();

		execvp(args.GetName(), const_cast<char * const *>(args.GetArgList())); // safe const_cast, see http://stackoverflow.com/questions/19505360/
		_exit(-1); // if we get here, we failed to exec, and we don't want 2 copies of the original program running so just die immediately
	}
	else // parent
	{
		my_std_in.CloseRead();		// parent process can only write to this
		my_std_out.CloseWrite();	// parent process can only read from this
		my_std_err.CloseWrite();	// parent process can only read from this

		if (in.GetNumber() == -1) std_in = my_std_in;
		if (out.GetNumber() == -1) std_out = my_std_out;
		if (err.GetNumber() == -1) std_err = my_std_err;

		started = true;
	}
}

IOPipe &IOProcess::GetStdin()
{
	if (in.GetNumber() != -1) throw IOException("Process input redirected: no such file or directory", ENOENT);
	if (!started) IOException("Process not started: operation cancelled.", ECANCELED);
	return std_in;
}

IOPipe &IOProcess::GetStdout()
{
	if (in.GetNumber() != -1) throw IOException("Process input redirected: no such file or directory", ENOENT);
	if (!started) IOException("Process not started: operation cancelled.", ECANCELED);
	return std_out;
}

IOPipe &IOProcess::GetStderr()
{
	if (in.GetNumber() != -1) throw IOException("Process input redirected: no such file or directory", ENOENT);
	if (!started) IOException("Process not started: operation cancelled.", ECANCELED);
	return std_err;
}

void IOProcess::RedirectStdin(const Descriptor &d)
{
	if (started) throw IOException("Process already started: operation cancelled.", ECANCELED);
	in = d;
}

void IOProcess::RedirectStdout(const Descriptor &d)
{
	if (started) throw IOException("Process already started: operation cancelled.", ECANCELED);
	out = d;
}

void IOProcess::RedirectStderr(const Descriptor &d)
{
	if (started) throw IOException("Process already started: operation cancelled.", ECANCELED);
	err = d;
}

void IOProcess::Close()
{
	std_in.Close();
	std_out.Close();
	std_err.Close();
}

}
