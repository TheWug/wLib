#include "wThread/include/runnable.h"

#include <iostream> // this is only for NULL

namespace w
{

void *Runnable::Start(void *x)
{
	Runnable *self = (Runnable *) x;
	self->Run();
	return NULL;
}

}
