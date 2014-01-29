#include "wSocket/wSocket.h"
#include "wString/wString.h"
#include "wMonitor/wMonitor.h"
#include "wTime/wTime.h"
#include "wIO/wIO.h"

#include <cstdlib>

using namespace w;
using namespace std;

class CommandReader : public IOStdStream
{
public:
	CommandReader() : IOStdStream(win), ircsock(NULL)
	{
		SetBufSize(1024);
	}

	CommandReader(const CommandReader &other) : IOStdStream(other), ircsock(other.ircsock), channel(other.channel)
	{
	}

	~CommandReader()
	{
		delete ircsock;
	}

	virtual CommandReader *Clone() const
	{
		return new CommandReader(*this);
	}

	Socket *ircsock;
	String channel;

protected:
	virtual void OnReadLine(char *buffer, int length)
	{
		String line(buffer, length);
		if (line.find('/') == 0 || channel == "")
		{
			if (line.find('/') == 0) line = line.substr(1);
			if (line.find("chan") == 0)
			{
				channel = line.substr(5);
				wout.Write("Set channel to " + channel + "\n");
			}
			else
			{
				wout.Write("==> " + line + "\n");
				ircsock->Write(line + "\r\n");
			}
		}
		else
		{
			wout.Write("==> PRIVMSG " + channel + " :" + line + "\n");
			ircsock->Write("PRIVMSG " + channel + " :" + line + "\r\n");
		}
	}

	virtual void OnError(const IOException &ioe)
	{
		wout.Write(String("BAD ERROR ") + String::ToString(ioe.errorcode) + " " + ioe.description + "\n");
	}
};

class WugSSH : public SSHProxySocket
{
public:
	WugSSH(const WugSSH &other) : SSHProxySocket(other)
	{
	}

	WugSSH()
	{
		SetBufSize(1024);
	}

	virtual WugSSH *Clone() const
	{
		return new WugSSH(*this);
	}

protected:
	virtual void OnReadLine(char *line, int length)
	{
		wout.Write("<== " + String(line, length) + "\n");
	}

	virtual void OnError(const IOException &ioe)
	{
		wout.Write(String("BAD ERROR ") + String::ToString(ioe.errorcode) + " " + ioe.description + "\n");
	}

	virtual void OnClose()
	{
		wout.Write(String("Closed for some reason\n"));
	}

	virtual void OnEstablish()
	{
		wout.Write(String("Connected to " + endpoint_host + ":" + endpoint_port + " via " + proxy_host + ":" + proxy_port + "\n"));
	}
};

int main()
{
	try
	{
		win.Read(NULL, 0);
		WugSSH ircsox;
		ircsox.BindProxy("wuggl.es", 22);
		ircsox.Bind("chat.freenode.net", 6667);

		CommandReader ui;

		Monitor iomon;
		iomon.Activate();
		CommandReader *cr = (CommandReader *) iomon.AddDataSource(ui);
		WugSSH *wssh = (WugSSH *) iomon.ConnectDataSocket(ircsox);
		cr->ircsock = wssh;

		while (true) iomon.ServiceRoutine(Interval(1000, 0));
	}
	catch (IOException &ioe)
	{
		wout.Write(String::ToString(ioe.errorcode) + "\n" + ioe.description + "\n");
	}
	return 0;
}
