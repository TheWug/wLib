#include <iostream>
#include <exception>
#include <vector>
#include <string>

#define protected public

#include "wIO/wIO.h"
#include "wString/wString.h"
#include "wArgv/wArgv.h"

using namespace std;

int main()
{
	try
	{
		w::IOProcess external;
		w::ProcessArgs command;
		command.SetName("sh");
		command.SetArg(1, "-c");
		command.SetArg(2, "sleep 5; ls -l;");
		external.Exec(command);
		char blob[1028] = {0};

		w::IOPipe proc_output = external.GetStdout();
		
		int readed;
		while (readed = proc_output.Read(blob, 1024))
		{
			blob[readed] = '\0';
			cout << blob;
		}
		cout << endl << "End of output." << endl;
	}
	catch (w::IOException e)
	{
		cout << e.what() << endl;
	}
	return 0;
}
