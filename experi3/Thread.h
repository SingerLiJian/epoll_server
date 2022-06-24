#ifndef THREAD_H
#define THREAD_H
#include <pthread.h>

class Thread
{
public:
	Thread();
	~Thread();
	int run(void* (*func)(void *), void* arg);
	int WaitForDeath();
	int ForDeath();
	
private:
	pthread_t m_ThreadID;
};

Thread::Thread()
{
}

Thread::~Thread()
{
	pthread_exit(NULL);
}


int Thread::run(void* (*func)(void *), void* arg)
{	
	int r = pthread_create(&m_ThreadID, 0, func, arg);
	return r;
}

int Thread::WaitForDeath()
{
	int r = pthread_join(m_ThreadID, 0);
	if(r != 0)
	{
		return -1;
	}
	return 0;
}

int Thread::ForDeath()
{
	int r= pthread_detach(m_ThreadID);
	return r;
}
#endif
