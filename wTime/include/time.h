#pragma once

#include <time.h>

namespace w
{

class Interval;

class Time : public timespec
{
public:
	Time();
	Time(const Time &other);
	
	Time &operator= (const Time &other);
	
	bool operator> (const Time &other) const;
	bool operator< (const Time &other) const;
	bool operator>= (const Time &other) const;
	bool operator<= (const Time &other) const;
	bool operator== (const Time &other) const;
	bool operator!= (const Time &other) const;
	
	Time &operator+= (const Interval &other);
	Time &operator-= (const Interval &other);
	
	Time operator+ (const Interval &other) const;
	Time operator- (const Interval &other) const;
	
	Interval operator- (const Time &other) const;
};

} // namespace w
