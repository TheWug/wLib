#pragma once

#include "wString/include/string.h"

#include <vector>

namespace w
{

class ProcessArgs
{
public:
	ProcessArgs(int argc, const char * const *argv);

	ProcessArgs(const std::string &name, const std::vector<std::string> &args, bool skipname = false);
	ProcessArgs(const std::string &name, const std::vector<String> &args, bool skipname = false);
	ProcessArgs();

	~ProcessArgs();

	const char *GetName() const;
	const char * const *GetArgList() const;

	void SetName(const std::string &newname);

	void SetArg(int position, const std::string &newarg);
	void TruncateArgs(int position = 0);
	void SetArgs(const std::vector<std::string> newargs, bool skipname = false);
	void SetArgs(const std::vector<String> newargs, bool skipname = false);

	const std::string &GetExecutableName() const;
	const std::string &GetArg(int i);

private:
	void RebuildRawArgs() const;

	std::vector<std::string> arglist;
	std::string name;

	mutable std::vector<const char *> arglist_raw;
	mutable bool needs_rebuild;
};

}
