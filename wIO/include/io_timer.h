#pragma once

#include "wIO/include/base_io.h"

namespace w
{

class IOTimer : public BaseIO
{
public:
	IOTimer(const Interval &timer_interval = IOTimer::disarmed, const Interval &repeat_interval = IOTimer::disarmed);
	IOTimer(const IOTimer &other);
	~IOTimer();

	void Open();
	void Close();

	void Set(const Interval &timer_interval = IOTimer::disarmed, const Interval &repeat_interval = IOTimer::disarmed);
	void GetSetting(Interval *timer_interval, Interval *repeat_interval);

	using BaseIO::Read;
	using BaseIO::Write;
	virtual size_t Write(const char *buffer, size_t length);
	virtual size_t Read(char *buffer, size_t length);
	virtual long long Read();

	virtual void ReadService();

	virtual IOTimer *Clone() const;

protected:
	// callbacks specific to this class and subclasses:
	virtual void OnEvent();
	virtual void OnMultipleEvents(long long num_events);

private:
	static const Interval &disarmed;
};

}
