#pragma once

namespace w
{

// To use this class, subclass it and pass to a thread.
class Runnable
{
public:
	virtual void Run() = 0;

protected:
	static void *Start(void *self);
};

} // namespace w
