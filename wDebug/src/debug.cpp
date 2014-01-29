#include "wDebug/include/debug.h"

#include <string>
#include <iostream>
#include <set>
#include <sstream>

#include <unistd.h>
#include <sys/resource.h>
#include <signal.h>

#ifdef _DEBUF
#define DEBUG_MODE true
#else
#define DEBUG_MODE false
#endif

using namespace w;
using namespace std;

namespace w
{

void DebugTools::AttachDebugger()
{
	return; // not yet implemented
	// basically, set options, fork,
	//	child:	     		 SIGSTOP self
	//	parent:     		 exec gdb, attach to child
}

void DebugTools::Break()
{
	if (!debug_mode) return;
	__asm__("int3");
}

void DebugTools::Log(const string &line)
{
	if (!debug_mode) return;
	DebugTools::Log(line.c_str());
}

void DebugTools::Log(const char *line)
{
	if (!debug_mode) return;
	if (logger == NULL) cerr << line << endl;
	else (*logger) << line << endl;
}
	
void DebugTools::SetLogger(DebugTools::StreamID id)
{
	if (!debug_mode) return;
	if (logger && is_logger_mine) delete logger;
	if (id == STD_COUT) logger = &cout;
	else if (id == STD_CERR) logger = &cerr;
	else logger = NULL;
	is_logger_mine = false;
}
	
void DebugTools::SetLogger(ostream *stream)
{
	if (!debug_mode) return;
	if (logger && is_logger_mine) delete logger;
	logger = stream;
	is_logger_mine = true;
}

void DebugTools::AssertImpl(bool condition, int line, const char *file, const char *name, const char *author)
{
	if (!debug_mode) return;
	if (unignore_all_assertions)
	{
		UnignoreAllAsserts();
		unignore_all_assertions = false;
	}
	
	if (ignored_assertions.find(GetAssertTag(line, file)) != ignored_assertions.end()) return;
	
	AssertAction action = default_action;
	if (action == PROMPT)
	{
		while (true)
		{
			cout << "Assertion failed: " << name << " at " << file << ":" << line << endl;
			cout
			     << "\t(i)gnore:\tignore forever" << endl
			     << "\t(c)ontinue:\tignore once" << endl
			     << "\t(l)og:\t\tprint diagnostic, then continue" << endl
			     << "\t(b)reak:\tenter debugger and break" << endl
			     << "\t(a)bort:\tabort and dump core" << endl
			     << "What to do? ";
			char choice;
			cin >> choice;
			cout << endl;
			if (!MapChoiceToAction(choice, action)) break;
			cout << "Invalid option." << endl;
		}
	}
	
	ostringstream temp;
	switch (action)
	{
	case IGNORE:
		IgnoreAssert(line, file);
		break;
	case CONTINUE:
		return;
	case LOG:
		temp << "Assertion failed: " << name << " at " << file << ":" << line << endl << "Code author: " << author;
		Log(temp.str());
		break;
	case BREAK:
		if (!in_debugger) AttachDebugger();
		else Break();
		break;
	case ABORT:
		DumpCore();
		break;
	case PROMPT: // we just prompted and should never get prompt here.. just continue I guess?
	default:
		return;
	}
	
	return;
}

void DebugTools::IgnoreAssert(int line, const char *file)
{
	
	ignored_assertions.insert(GetAssertTag(line, file));
}

void DebugTools::UnignoreAllAsserts()
{
	ignored_assertions.clear();
}

string DebugTools::GetAssertTag(int line, const char *file)
{
	ostringstream temp;
	temp << file << ":" << line;
	return temp.str();
}

bool DebugTools::MapChoiceToAction(char c, DebugTools::AssertAction &a)
{
	switch (c)
	{
	case 'i':
		a = IGNORE;
		return true;
	case 'c':
		a = CONTINUE;
		return true;
	case 'l':
		a = LOG;
		return true;
	case 'b':
		a = BREAK;
		return true;
	case 'a':
		a = ABORT;
		return true;
	default:
		return false;
	}
	return false;
}

void DebugTools::DumpCore()
{
	rlimit lim;
	getrlimit(RLIMIT_CORE, &lim);
	lim.rlim_cur = lim.rlim_max; // set highest possible limit
	setrlimit(RLIMIT_CORE, &lim);
	raise(SIGQUIT);
}

bool DebugTools::in_debugger = false;
bool DebugTools::debug_mode = DEBUG_MODE;
ostream *DebugTools::logger = NULL;
bool DebugTools::is_logger_mine = false;
DebugTools::AssertAction DebugTools::default_action = PROMPT;
bool DebugTools::unignore_all_assertions = false;
set<string> DebugTools::ignored_assertions;

}
