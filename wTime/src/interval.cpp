#include "wTime/include/time.h"
#include "wTime/include/interval.h"
#include "wTime/include/chrono.h"

namespace w
{

const Interval Interval::zero(0, 0, false);

Interval::Interval()
{
	*this = zero;
}

Interval::Interval(const Interval &other)
{
	*this = other;
}

Interval::Interval(time_t seconds, long nseconds, bool negative)
{
	tv_sec = seconds;
	tv_nsec = nseconds;
	is_negative = negative;
	
	// this is safe to use even when constructing Interval::zero.
	// regardless of whether this compares equal or not (which it should anyway),
	// is_negative will always be set to false.
	if (!Chrono::Compare(this, &Interval::zero)) is_negative = false;
}

Interval::Interval(const Time &first, const Time &second)
{
	if (first > second)
	{
		is_negative = false;
		tv_sec = first.tv_sec - second.tv_sec;
		tv_nsec = first.tv_nsec - second.tv_nsec;
	}
	else
	{
		is_negative = true;
		tv_sec = second.tv_sec - first.tv_sec;
		tv_nsec = second.tv_nsec - first.tv_nsec;
	}
	
	if (tv_nsec < 0)
	{
		--tv_sec;
		tv_nsec += 1000000000;
	}
}

/* Interval::Interval(TimeType unit, long long quantity)
{
	
} */

Interval &Interval::operator= (const Interval &other)
{
	tv_sec = other.tv_sec;
	tv_nsec = other.tv_nsec;
	is_negative = other.is_negative;
	return *this;
}

Interval &Interval::operator= (const timespec &other)
{
	tv_sec = other.tv_sec;
	tv_nsec = other.tv_nsec;
	return *this;
}

// this function is copypasta of operator<
// changes here should be made there too
bool Interval::operator> (const Interval &other) const
{
	if (!is_negative && other.is_negative) return true;
	else if (is_negative && !other.is_negative) return false;
	int comparison = Chrono::Compare(this, &other);
	if (comparison == 1) return !is_negative;
	else if (comparison == 0) return false;
	else return is_negative;
}

// this function is copypasta of operator>
// changes here should be made there too
bool Interval::operator< (const Interval &other) const
{
	if (!is_negative && other.is_negative) return false;
	else if (is_negative && !other.is_negative) return true;
	int comparison = Chrono::Compare(this, &other);
	if (comparison == -1) return !is_negative;
	else if (comparison == 0) return false;
	else return is_negative;
}

bool Interval::operator>= (const Interval &other) const
{
	return (*this == other || *this > other);
}

bool Interval::operator<= (const Interval &other) const
{
	return (*this == other || *this < other);
}

bool Interval::operator== (const Interval &other) const
{
	return (!Chrono::Compare(this, &other) && is_negative == other.is_negative);
}

bool Interval::operator!= (const Interval &other) const
{
	return !(*this == other);
}

Interval Interval::operator-() const
{
	return Interval(tv_sec, tv_nsec, !is_negative);
}

Interval &Interval::operator+= (const Interval &other)
{
	bool are_same_polarity = !(is_negative ^ other.is_negative); // true if both are + or both are -, false otherwise
	if (are_same_polarity) Chrono::AddTo(this, &other, this);
	else
	{
		int comparison = Chrono::Compare(this, &other);
		if (comparison == 0) *this = Interval::zero;
		else if (comparison == 1) Chrono::SubtractFrom(this, &other, this);
		else if (comparison == -1)
		{
			Chrono::SubtractFrom(&other, this, this);
			is_negative = other.is_negative;
		}
	}
	return *this;
}

Interval &Interval::operator-= (const Interval &other)
{
	Interval x = -other;
	return *this += x;
}

Interval Interval::operator+ (const Interval &other) const
{
	return (Interval(*this) -= other);
}

Interval Interval::operator- (const Interval &other) const
{
	return (Interval(*this) += other);
}

Time Interval::operator+ (const Time &other) const
{
	return other + *this;
}

bool Interval::IsNegative() const
{
	return is_negative;
}

long long Interval::ToMillis() const
{
	long long millis = tv_sec * 1000 + tv_nsec / 1000000;
	return (is_negative ? -millis : millis);
}

} // namespace w
