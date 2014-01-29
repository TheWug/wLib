#include "wIO/wIO.h"
#include "wMonitor/wMonitor.h"

using namespace std;
using namespace w;

class TimerDerp : public IOTimer
{
public:
	TimerDerp() : IOTimer(Interval(5, 0), Interval(1, 0)), i(0) {}
	TimerDerp(const TimerDerp &other) : IOTimer(other), i(other.i) {}

	TimerDerp *Clone() const { return new TimerDerp(*this); }

	void OnEvent()
	{
		if (i == 5) Reset();
		else
		{
			wout.Write("An event occurred!\n");
			++i;
		}
	}

	void OnClose()
	{
		wout.Write("Closed!\n");
	}

	void OnError()
	{
		wout.Write("Error!\n");
	}

	void Reset()
	{
		Set(Interval(5, 0), Interval(1, 0));
		i = 0;
	}

	int i;
};

int main()
{
	TimerDerp timertest;
	Monitor mon;

	mon.Activate();

	mon.AddDataSource(timertest);
	while (true)
	{
		mon.ServiceRoutine(Interval(2, 0));
	}
}
