#include "mutex_lock.h"
#include "stack_lock.h"

#include <iostream>

void * func(void *v)
{
	int id = (int) v;
	static MutexLock l;
	static int x = 0;
	for (int i = 0; i < 5; ++i)
	{
		StackLock s(l);
		std::cout << "loljunk(" << id << ") " << ++x << std::endl;
	}
	return NULL;
}

int main()
{
	int count = 1500;
	pthread_t *threads = new pthread_t[count];
	pthread_attr_t settings;
	pthread_attr_init(&settings);
	pthread_attr_setstacksize(&settings, 1);
	for (int i = 0; i < count; ++i) pthread_create(threads + i, &settings, func, (void *) i);
	for (int i = 0; i < count; ++i) pthread_join(threads[i], NULL);
	return 0;
}
