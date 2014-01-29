#include "string.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace w;

int main()
{
	String delimiter(", ");
	
	vector<String> tokens;
	tokens.push_back(String::ToString(5));
	tokens.push_back(String::ToString(1.5));
	tokens.push_back("ELEVEN");
	tokens.push_back(String::ToString(true));
	
	for (int i = 0; i < tokens.size(); ++i)
		cout << tokens[i].length() << " \"" << tokens[i] << "\"" << endl;
	
	String line = delimiter.Join(tokens);
	cout << line << endl;
}
