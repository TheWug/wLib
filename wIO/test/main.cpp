#include <iostream>
#include <exception>

#include "wIO/wIO.h"

using namespace std;

int main()
{
	try
	{
		w::IOPipe foo;
		foo.Open();
		foo.Write("this is a test.\n");
		char str[256] = {0};
		foo.Read(str, 256);
		cout << str;
		foo.Close();
	}
	catch (w::IOException e)
	{
		cout << e.what() << endl;
	}
	return 0;
}
