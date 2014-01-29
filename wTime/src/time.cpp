#include "wTime/include/time.h"
#include "wTime/include/interval.h"
#include "wTime/include/chrono.h"

namespace w
{

Time::Time()
{
	tv_sec = 0;
	tv_nsec = 0;
}

Time::Time(const Time &other)
{
	*this = other;
}

Time &Time::operator= (const Time &other)
{
	*((timespec *) this) = *((timespec *) &other);
	return *this;
}

bool Time::operator> (const Time &other) const
{
	return (Chrono::Compare(this, &other) == 1);
}

bool Time::operator< (const Time &other) const
{
	return (Chrono::Compare(this, &other) == -1);
}

bool Time::operator>= (const Time &other) const
{
	int comparison = Chrono::Compare(this, &other);
	return (comparison == 0 || comparison == 1);
}

bool Time::operator<= (const Time &other) const
{
	int comparison = Chrono::Compare(this, &other);
	return (comparison == 0 || comparison == -1);
}

bool Time::operator== (const Time &other) const
{
	return (Chrono::Compare(this, &other) == 0);
}

bool Time::operator!= (const Time &other) const
{
	return (Chrono::Compare(this, &other) != 0);
}

Time &Time::operator+= (const Interval &other)
{
	if (other > Interval::zero) Chrono::AddTo(this, &other, this);
	else Chrono::SubtractFrom(this, &other, this);
	return *this;
}

Time &Time::operator-= (const Interval &other)
{
	if (other < Interval::zero) Chrono::AddTo(this, &other, this);
	else Chrono::SubtractFrom(this, &other, this);
	return *this;
}

Time Time::operator+ (const Interval &other) const
{
	return (Time(*this) += other);
}

Time Time::operator- (const Interval &other) const
{
	return (Time(*this) -= other);
}

Interval Time::operator- (const Time &other) const
{
	return Interval(*this, other);
}

} // namespace w
