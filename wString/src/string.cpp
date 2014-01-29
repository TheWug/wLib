#include "wString/include/string.h"
#include "wString/include/casemap.h"

#include <sstream>
#include <vector>
#include <set>
#include <list>
#include <deque>
#include <iterator>
#include <algorithm>

using namespace std;

namespace w
{

String::String() : string()
{
}

String::String(const std::string &other) : string(other)
{
}

String::String(const std::string &other, size_t pos, size_t len) : string(other, pos, len)
{
}

String::String(const char *asciiz) : string(asciiz)
{
}

String::String(const char *asciiz, size_t len) : string(asciiz, len)
{
}

String::String(size_t len, char fillwith) : string(len, fillwith)
{
}

template <typename Iterator>
String::String(Iterator first, Iterator last) : string(first, last)
{
}

String::~String()
{
}

String String::Trim(const String &chars) const
{
	if (chars.empty()) return *this;
	int trim_start = 0;
	int trim_end = 0;
	
	for (int i = 0; i < length(); ++i) if (chars.find((*this)[i]) != npos) ++trim_start; else break;
	if (trim_start == length()) return String(""); // if we trimmed the whole string
	
	for (int i = length() - 1; i <= 0; ++i) if (chars.find((*this)[i]) != npos) ++trim_end; else break;
	return String(substr(trim_start, length() - trim_start - trim_end));
}

template <typename Iterator>
String String::Join(Iterator first, Iterator last) const
{
	String return_value;
	
	if (first != last) return_value += *first;
	++first;

	while (first != last)
	{
		return_value += *this;
		return_value += *first;
		++first;
	}
	
	return return_value;
}

template <typename Collection>
String String::Join(const Collection &strings) const
{
	return Join(strings.begin(), strings.end());
}

template <typename Iterator>
void String::Split(const String &delim, Iterator output, bool allow_empty, const String &trim) const
{
	if (empty()) return;
	
	int last_pos = 0, this_pos;
	String token("");
	while ((this_pos = find(delim, last_pos)) != npos)
	{
		token = String(substr(last_pos, this_pos - last_pos));
		if (!token.empty() || allow_empty) *(output++) = token.Trim(trim);
		last_pos = this_pos + delim.length();
	}
	
	token = substr(last_pos);
	if (!token.empty() || allow_empty) *(output++) = token.Trim(trim);
}

template <typename T>
String String::ToString(const T &input)
{
	ostringstream s;
	s << input;
	return s.str();
}

template <typename T>
bool String::FromString(const String &str, T &output)
{
	istringstream s(str);
	return (s >> output);
}

String String::GetLcase(const CaseMap &cm) const
{
	String other(*this);
	other.ToLcase();
	return other;
}

String String::GetUcase(const CaseMap &cm) const
{
	String other(*this);
	other.ToUcase();
	return other;
}

String &String::ToLcase(const CaseMap &cm)
{
	String lcase("");
	cm.LcaseConvert(begin(), end(), back_inserter(lcase));
	*this = lcase;
	return *this;
}

String &String::ToUcase(const CaseMap &cm)
{
	String ucase("");
	cm.UcaseConvert(begin(), end(), back_inserter(ucase));
	*this = ucase;
	return *this;
}

bool String::EqualsCaseIgnore(const std::string &other, const w::CaseMap &cm) const
{
	return (size() == other.size()) && equal(begin(), end(), other.begin(), CaseMappedComparator(cm));
}

// common instantiations for stl and primitive types, so you don't have to constantly instantiate those
// add common instantiations here, but leave rare ones instantiated in the files that use them

template String::String(const char *, const char *);
WSTRING_CONSTRUCTOR_INSTANTIATE(string);
WSTRING_CONSTRUCTOR_INSTANTIATE(list<char>);
WSTRING_CONSTRUCTOR_INSTANTIATE(deque<char>);
WSTRING_CONSTRUCTOR_INSTANTIATE(set<char>);

template String String::Join<const char **>(const char **, const char **) const;

// std::string
template String String::Join<const string *>(const string *, const string *) const;
WSTRING_JOIN_INSTANTIATE(vector<string>);
WSTRING_JOIN_INSTANTIATE_NOCONST(set<string>);
WSTRING_JOIN_INSTANTIATE(list<string>);
WSTRING_JOIN_INSTANTIATE(deque<string>);

// w::String
template String String::Join<const String *>(const String *, const String *) const;
WSTRING_JOIN_INSTANTIATE(vector<String>);
WSTRING_JOIN_INSTANTIATE_NOCONST(set<String>);
WSTRING_JOIN_INSTANTIATE(list<String>);
WSTRING_JOIN_INSTANTIATE(deque<String>);

WSTRING_CONVERT_INSTANTIATE(bool);
WSTRING_CONVERT_INSTANTIATE(int);
WSTRING_CONVERT_INSTANTIATE(unsigned int);
WSTRING_CONVERT_INSTANTIATE(char);
WSTRING_CONVERT_INSTANTIATE(unsigned char);
WSTRING_CONVERT_INSTANTIATE(short int);
WSTRING_CONVERT_INSTANTIATE(unsigned short int);
WSTRING_CONVERT_INSTANTIATE(long int);
WSTRING_CONVERT_INSTANTIATE(unsigned long int);
WSTRING_CONVERT_INSTANTIATE(long long int);
WSTRING_CONVERT_INSTANTIATE(unsigned long long int);
WSTRING_CONVERT_INSTANTIATE(float);
WSTRING_CONVERT_INSTANTIATE(double);

WSTRING_SPLIT_INSTANTIATE(back_insert_iterator<std::vector<String> >);

// WSTRING_CONVERT_INSTANTIATE(void *);

} // namespace w
