#include "wArgv/include/process_args.h"

#include <algorithm>

using namespace std;

namespace w
{

ProcessArgs::ProcessArgs(int argc, const char * const *argv) : needs_rebuild(true)
{
	if (argc > 0) name = argv[0];
	for (int i = 0; i < argc; ++i) arglist.push_back(argv[i]);
}

ProcessArgs::ProcessArgs(const string &newname, const vector<string> &args, bool skipname) : name(newname), needs_rebuild(true)
{
	arglist.resize(args.size() + (skipname ? 1 : 0));
	copy(args.begin(), args.end(), arglist.begin() + (skipname ? 1 : 0));
	if (skipname) arglist[0] = newname;
}

ProcessArgs::ProcessArgs(const string &newname, const vector<String> &args, bool skipname) : needs_rebuild(true)
{
	arglist.resize(args.size() + (skipname ? 1 : 0));
	copy(args.begin(), args.end(), arglist.begin() + (skipname ? 1 : 0));
	if (skipname) arglist[0] = newname;
}

ProcessArgs::ProcessArgs() : arglist_raw(1, (const char *) NULL), needs_rebuild(false)
{
}

ProcessArgs::~ProcessArgs()
{
}

const char *ProcessArgs::GetName() const
{
	return name.c_str();
}

const char * const *ProcessArgs::GetArgList() const
{
	if (needs_rebuild) RebuildRawArgs();
	return &arglist_raw[0];
}

void ProcessArgs::SetName(const string &newname)
{
	name = newname;
	SetArg(0, name);
}

void ProcessArgs::SetArg(int position, const string &newarg)
{
	if (arglist.size() <= position) arglist.resize(position + 1);
	arglist[position] = newarg;
	needs_rebuild = true;
}

void ProcessArgs::TruncateArgs(int position)
{
	if (arglist.size() > position) arglist.resize(position);
	needs_rebuild = true;
}

void ProcessArgs::SetArgs(const vector<string> newargs, bool skipname)
{
	arglist.resize(newargs.size() + (skipname ? 1 : 0));
	copy(newargs.begin(), newargs.end(), arglist.begin() + (skipname ? 1 : 0));
	needs_rebuild = true;
}

void ProcessArgs::SetArgs(const vector<String> newargs, bool skipname)
{
	arglist.resize(newargs.size() + (skipname ? 1 : 0));
	copy(newargs.begin(), newargs.end(), arglist.begin() + (skipname ? 1 : 0));
	needs_rebuild = true;
}

const string &ProcessArgs::GetExecutableName() const
{
	return name;
}

const string &ProcessArgs::GetArg(int i)
{
	return arglist.at(i);
}

void ProcessArgs::RebuildRawArgs() const
{
	arglist_raw.resize(arglist.size() + 1);
	for (int i = 0; i < arglist.size(); ++i) arglist_raw[i] = arglist[i].c_str();
	arglist_raw[arglist.size()] = NULL;
	needs_rebuild = false;
}

}
