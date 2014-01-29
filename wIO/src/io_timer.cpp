#include "wIO/include/io_timer.h"

#include <sys/timerfd.h>
#include <unistd.h>

#include <iostream>

using namespace std;

namespace w
{

const Interval &IOTimer::disarmed = Interval::zero;

IOTimer::IOTimer(const Interval &timer_interval, const Interval &repeat_interval) : BaseIO(BaseIO::default_timeout_infinity)
{
	if (timer_interval == disarmed && repeat_interval == disarmed) return;

	Open();
	Set(timer_interval, repeat_interval);
}

IOTimer::IOTimer(const IOTimer &other) : BaseIO(other)
{
}

IOTimer::~IOTimer()
{
}

void IOTimer::Open()
{
	Descriptor fd(timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK));
	if (fd == Descriptor::invalid) throw IOException::MakeException("Error creating timer", errno);

	SetReadDescriptor(fd);
}

void IOTimer::Close()
{
	SetReadDescriptor(Descriptor::invalid);
}

void IOTimer::Set(const Interval &timer_interval, const Interval &repeat_interval)
{
	itimerspec its;
	if (timer_interval < Interval::zero || repeat_interval < Interval::zero) throw IOException("Error setting timer: negative intervals are not permitted.", EINVAL);
	its.it_interval = repeat_interval;
	its.it_value = timer_interval;
	int rv = timerfd_settime(GetReadDescriptor().GetNumber(), 0, &its, NULL);
	if (rv == -1) throw IOException::MakeException("Error setting timer", errno);
}

void IOTimer::GetSetting(Interval *timer_interval, Interval *repeat_interval)
{
	if (timer_interval == NULL && repeat_interval == NULL) return;
	itimerspec its;
	int rv = timerfd_gettime(GetReadDescriptor().GetNumber(), &its);
	if (rv == -1) throw IOException::MakeException("Error reading timer settings", errno);
	if (timer_interval != NULL) { *timer_interval = Interval::zero; *timer_interval = its.it_value; }
	if (repeat_interval != NULL) { *repeat_interval = Interval::zero; *repeat_interval = its.it_interval; }
	return;
}

size_t IOTimer::Write(const char *buffer, size_t length)
{
	throw IOException("Error writing to timer: writing to this object is not supported.", EPERM);
}

size_t IOTimer::Read(char *buffer, size_t length)
{
	if (GetReadDescriptor() == Descriptor::invalid) throw IOException::MakeException("File not open for reading", EPERM);
	ssize_t readed = read(GetReadDescriptor().GetNumber(), buffer, length);
	if (readed < 0) throw IOException::MakeException("Error during IOFile::Read", errno);
	return readed;
}

long long IOTimer::Read()
{
	long long value;
	try
	{
		Read((char *) &value, sizeof(value));
	}
	catch (IOException &ioe)
	{
		if (ioe.errorcode == EAGAIN) return 0; // not actually an error, indicates that the read would have blocked because the timer has not triggered yet, so return 0
		throw; // otherwise, an actual error occurred, so rethrow it
	}
	return value;
}

void IOTimer::ReadService()
{
	long long quantity = Read();
	if (quantity == 0) return;
	else if (quantity == 1) OnEvent();
	else OnMultipleEvents(quantity);
}

IOTimer *IOTimer::Clone() const
{
	return new IOTimer(*this);
}

void IOTimer::OnEvent() {}

void IOTimer::OnMultipleEvents(long long num_events)
{
	for (long long i = 0; i < num_events; ++i) OnEvent();
}

}
