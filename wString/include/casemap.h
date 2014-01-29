#pragma once

namespace w
{

class CaseMap
{
public:
	static const CaseMap default_map;
	static const CaseMap utf8_map; // dummy, for future utf8 support
	static const CaseMap irc_map;
	
private:
	CaseMap(const CaseMap &other); // no copy constructor
	CaseMap(int i); // used to initialize static members
	
	const char *lowercase;
	const char *uppercase;

	int init;
	
public:
	CaseMap(); // no-op default constructor
	virtual ~CaseMap();
	
	template <typename Iterator, typename OutIterator> void UcaseConvert(Iterator start, Iterator end, OutIterator store) const;
	template <typename Iterator, typename OutIterator> void LcaseConvert(Iterator start, Iterator end, OutIterator store) const;

	bool AreEqual(char c1, char c2) const;

	void Dump() const;
};

class CaseMappedComparator
{
public:
	CaseMappedComparator(const CaseMap &ncm = CaseMap::default_map);

	bool operator() (char c1, char c2) const;

	const CaseMap &cm;
};

#define WCASEMAP_CONVERT_INSTANTIATE(input, output)	template void CaseMap::UcaseConvert<input, output>(input, input, output) const; \
							template void CaseMap::LcaseConvert<input, output>(input, input, output) const;

} // namespace w
