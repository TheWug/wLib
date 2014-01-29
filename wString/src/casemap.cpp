#include "wString/include/casemap.h"

#include <stdexcept>
#include <iostream>

#include "wString/include/string.h"

using namespace std;

namespace w
{

const CaseMap CaseMap::default_map(1);
const CaseMap CaseMap::utf8_map(2);
const CaseMap CaseMap::irc_map(3);

CaseMap::CaseMap(int x) : init(x)
{
	char *umap = new char[256];
	char *lmap = new char[256];
	
	lowercase = lmap;
	uppercase = umap;
	
	for (int i = 0; i < 256; ++i)
	{
		umap[i] = (char) i;
		lmap[i] = (char) i;
	}
	
	switch (x)
	{
	case 3:
	case 1:
		for (int i = (unsigned) 'A'; i <= (unsigned) 'Z'; ++i)
		{
			umap[i + ('a' - 'A')] 	= (char) i;			// map uppercase equiv of lowercase letters to ucase
			lmap[i] 		= ('a' - 'A') + (char) i;	// map lowercase equiv of uppercase letters to lcase
		}
		
		if (x == 1) break;
		
		umap[(unsigned) '{'] = '[';
		umap[(unsigned) '}'] = ']';
		umap[(unsigned) '|'] = '\\';
		lmap[(unsigned) '['] = '{';
		lmap[(unsigned) ']'] = '}';
		lmap[(unsigned) '\\'] = '|';
		
		break;
	case 2: // utf8 not supported now
		break;
	default: throw runtime_error("CaseMap::init: got invalid identifier for builtin casemap (this should never happen!)");
	}
}


CaseMap::CaseMap() : uppercase(NULL), lowercase(NULL) {}

CaseMap::~CaseMap()
{
	if (uppercase) delete[] uppercase;
	if (lowercase) delete[] lowercase;
}

template <typename Iterator, typename OutIterator>
void CaseMap::UcaseConvert(Iterator start, Iterator end, OutIterator store) const
{
	while (start != end) *store = uppercase[(unsigned) *(start++)];
}

template <typename Iterator, typename OutIterator>
void CaseMap::LcaseConvert(Iterator start, Iterator end, OutIterator store) const
{
	while (start != end) *store = lowercase[(unsigned) *(start++)];
}

bool CaseMap::AreEqual(char c1, char c2) const
{
	return lowercase[(unsigned) c1] == lowercase[(unsigned) c2];
}

void CaseMap::Dump() const
{
	cout << "CaseMap " << init << " " << this << ":" << endl;
	for (int i = '0'; i < 256; ++i)
	{
		cout << (char) i << " " << lowercase[i] << " " << uppercase[i] << endl;
	}
}

CaseMappedComparator::CaseMappedComparator(const CaseMap &ncm) : cm(ncm)
{
}

bool CaseMappedComparator::operator() (char c1, char c2) const
{
	return cm.AreEqual(c1, c2);
}

WCASEMAP_CONVERT_INSTANTIATE(const char *, char *);
WCASEMAP_CONVERT_INSTANTIATE(std::string::const_iterator, back_insert_iterator<std::string>);
WCASEMAP_CONVERT_INSTANTIATE(std::string::const_iterator, back_insert_iterator<String>);
WCASEMAP_CONVERT_INSTANTIATE(std::string::iterator, back_insert_iterator<std::string>);
WCASEMAP_CONVERT_INSTANTIATE(std::string::iterator, back_insert_iterator<String>);

} // namespace w
