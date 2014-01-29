#pragma once

#include <string>

#include "wString/include/casemap.h"

namespace w
{

class String : public std::string
{
	public: // constructors, fall back on std::string constructors
	String();
	String(const std::string &other);
	String(const std::string &other, size_t pos, size_t len = npos);
	String(const char *asciiz);
	String(const char *asciiz, size_t len);
	String(size_t len, char fillwith);
	template <typename Iterator> String(Iterator first, Iterator last);
	virtual ~String();
	
	public: // new functionality
	String Trim(const String &chars) const;
	
	template <typename Iterator> String Join(Iterator first, Iterator last) const;
	template <typename Collection> String Join(const Collection &strings) const;
	
	template <typename Iterator> void Split(const String &delim, Iterator output, bool allow_empty = false, const String &trim = "") const;
	
	template <typename T> static String ToString(const T &input);
	template <typename T> static bool FromString(const String &str, T &output);
	
	String GetLcase(const w::CaseMap &cm = w::CaseMap::default_map) const; // produces a new string
	String GetUcase(const w::CaseMap &cm = w::CaseMap::default_map) const;
	
	String &ToLcase(const w::CaseMap &cm = w::CaseMap::default_map);	// edits existing string, returns *this
	String &ToUcase(const w::CaseMap &cm = w::CaseMap::default_map);

	bool EqualsCaseIgnore(const std::string &other, const w::CaseMap &cm = w::CaseMap::default_map) const;
};

} // namespace w

// declare templates of string methods with these:
#define WSTRING_CONSTRUCTOR_INSTANTIATE(type)	template String::String(type::iterator, type::iterator);

#define WSTRING_JOIN_INSTANTIATE_NOCONST(type)	template String String::Join<type::iterator>(type::iterator, type::iterator) const; \
						template String String::Join(const type &strings) const;
						
#define WSTRING_JOIN_INSTANTIATE(type)		template String String::Join<type::const_iterator>(type::const_iterator, type::const_iterator) const; \
						WSTRING_JOIN_INSTANTIATE_NOCONST(type);
						
#define WSTRING_CONVERT_INSTANTIATE(type)	template String String::ToString<type>(const type &); \
						template bool String::FromString<type>(const String &, type &output);
						
#define WSTRING_SPLIT_INSTANTIATE(type)		template void String::Split<type>(const String &, type, bool, const String &) const;
