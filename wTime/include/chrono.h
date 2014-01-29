#pragma once

#include "wTime/include/time.h"

namespace w
{

class Chrono
{
private:
	Chrono() {} // uninstantiable
	
public:
	static Time Now();
	static void Sleep(Interval i);
	static void SleepUntil(const Time &i);
	
	static void AddTo(const timespec *target, const timespec *increment_by, timespec *storein);
	static void SubtractFrom(const timespec *target, const timespec *decrement_by, timespec *storein);
	
	static int Compare(const timespec *first, const timespec *second);
};

} // namespace w
