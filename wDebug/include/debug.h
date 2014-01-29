#pragma once

#include <string>
#include <set>
#include <iostream>

namespace w
{

class DebugTools
{
public:
	enum StreamID
	{
		STD_COUT, // use cout
		STD_CERR, // use cerr
		STD_NULL // placeholder, use NULL
	};
	
	enum AssertAction
	{
		PROMPT, // read choice from cin.  choice can be one of the other options
		IGNORE, // never stop at this assert again
		CONTINUE, // do nothing this one time
		LOG, // print error message using Log()
		BREAK, // optionally enter debugger, then break for debugging
		ABORT // terminate immediately with abort()
	};
	
	static void AttachDebugger();
	static void Break();
	
	static void Log(const std::string &line);
	static void Log(const char *line);
	
	static void SetLogger(DebugTools::StreamID id);
	static void SetLogger(std::ostream *stream);
	
	static void AssertImpl(bool condition, int line, const char *file, const char *name, const char *author);
	static void UnignoreAllAssertions();
	
	static std::string GetAssertTag(int line, const char *file);
	static void IgnoreAssert(int line, const char *file);
	static void UnignoreAllAsserts();
	static bool MapChoiceToAction(char c, AssertAction &a);
	
	static void DumpCore();
	
	#define Assert(CONDITION) AssertImpl(CONDITION, __LINE__, __FILE__, #CONDITION, "[unspecified]");
	
	#define AssertEx(CONDITION, AUTHOR, ASSERTTAG)	std::string ASSERTTAG(DebugTools::GetAssertTag(__LINE__, __FILE__)); \
							AssertImpl(CONDITION, __LINE__, __FILE__, #CONDITION, AUTHOR);
	
private:
	static AssertAction default_action;

	static bool in_debugger;
	static bool debug_mode;
	
	static std::ostream *logger;
	static bool is_logger_mine;
	
	static bool unignore_all_assertions;
	
	static std::set<std::string> ignored_assertions;
};

}
