#ifndef COND_H
#define COND_H
#include <pthread.h>
#include "mutex.h"

class Cond
{
public:
	Cond();
	virtual ~Cond();

	int Wait(Mutex* m_mutex);
	int Signal();
    int Broadcast();

private:
	Cond(const Cond&);
	Cond& operator=(const Cond&);

private:
	pthread_cond_t m_cond;
};

Cond::Cond()
{
	int  r = pthread_cond_init(&m_cond, 0);
	if(r != 0)
	{
		throw "In Mutex::Mutex(), pthread_mutex_init error";
	}
}

Cond::~Cond()
{
	int r = pthread_cond_destroy(&m_cond);
	if(r != 0)
	{
		throw "In Mutex::~Mutex(), pthread_mutex_destroy error";
	}
}

int Cond::Wait(Mutex* m_mutex)
{
	int r = pthread_cond_wait(&m_cond, m_mutex->getMutex());
	if(r != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int Cond::Signal()
{
	int r = pthread_cond_signal(&m_cond);
	if(r != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int Cond::Broadcast()
{
    int r = pthread_cond_broadcast(&m_cond);
    if(r != 0)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

#endif