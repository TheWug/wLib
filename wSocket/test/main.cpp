#include "wSocket/ssh_proxy_socket.h"

#include <string>
#include <iostream>
#include <cstring>

using namespace std;

int main()
{
	char *buf = new char[1024 * 1024];
	memset(buf, 0, 1024);
	ProxySocket *s = new SSHProxySocket();
	Socket::Status stat;
	stat = s->Bind("127.0.0.1", 6667);
	stat = s->BindProxy("127.0.0.1", 22);
	stat = s->Connect(5000);
	string irc_commands = "NICK :WugBot\r\nUSER WugBot * * :Real name\r\nJOIN :#derp\r\nPRIVMSG #derp :Success! Socket seems to be working as expected.\r\n";
	s->Send(irc_commands.c_str(), irc_commands.length());
	for (int i = 0; i < 5; ++i)
	{
		int x = s->Recieve(buf, 1024 * 1024 - 1);
		if (x < 0) x = 0;
		buf[x] = 0;
		cout << buf;
		sleep(1);
	}
	sleep(5);
	s->Disconnect();
	delete s;
	return 0;
}
