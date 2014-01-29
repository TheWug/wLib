#pragma once

#include "wTime/include/time_type.h"

#include <time.h>

namespace w
{

class Time;

class Interval : public timespec
{
	friend class Time;
	
public:
	Interval();
	Interval(const Interval &other);
	Interval(const Time &first, const Time &second);
	Interval(time_t seconds, long nseconds, bool negative = false);
	// Interval(TimeType unit, long long quantity);

	Interval &operator= (const Interval &other);
	Interval &operator= (const timespec &other);
	
	bool operator> (const Interval &other) const;
	bool operator< (const Interval &other) const;
	bool operator>= (const Interval &other) const;
	bool operator<= (const Interval &other) const;
	bool operator== (const Interval &other) const;
	bool operator!= (const Interval &other) const;
	
	Interval operator-() const;
	
	Interval &operator+= (const Interval &other);
	Interval &operator-= (const Interval &other);
	
	Interval operator+ (const Interval &other) const;
	Interval operator- (const Interval &other) const;
	
	Time operator+ (const Time &other) const;
	
	bool IsNegative() const;

	long long ToMillis() const;
	
public:
	static const Interval zero;

private:
	bool is_negative;
};

} // namespace w
