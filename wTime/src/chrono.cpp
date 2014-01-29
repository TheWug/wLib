#include "wTime/include/chrono.h"

#include <time.h>
#include <errno.h>

#include "wTime/include/interval.h"
#include "wTime/include/time.h"

namespace w
{

Time Chrono::Now()
{
	Time time;
	clock_gettime(CLOCK_REALTIME, &time);
	return time;
}

void Chrono::AddTo(const timespec *left, const timespec *right, timespec *output)
{
	output->tv_nsec = left->tv_nsec + right->tv_nsec;
	output->tv_sec = left->tv_sec + right->tv_sec;
	if (output->tv_nsec > 999999999)
	{
		output->tv_nsec -= 1000000000;
		++output->tv_sec;
	}
}

void Chrono::SubtractFrom(const timespec *left, const timespec *right, timespec *output)
{
	output->tv_nsec = left->tv_nsec - right->tv_nsec;
	output->tv_sec = left->tv_sec - right->tv_sec;
	if (output->tv_nsec < 0)
	{
		output->tv_nsec += 1000000000;
		--output->tv_sec;
	}
}

int Chrono::Compare(const timespec *first, const timespec *second)
{
	if (first->tv_sec > second->tv_sec) return 1;
	else if (first->tv_sec == second->tv_sec)
	{
		if (first->tv_nsec > second->tv_nsec) return 1;
		else if (first->tv_nsec == second->tv_nsec) return 0;
	}
	return -1;
}

void Chrono::Sleep(Interval i)
{
	while (clock_nanosleep(CLOCK_MONOTONIC, 0, &i, &i) == EINTR);
	return;
}

void Chrono::SleepUntil(const Time &t)
{
	while (clock_nanosleep(CLOCK_REALTIME, TIMER_ABSTIME, &t, NULL) == EINTR);
	return;
}

} // namespace w
