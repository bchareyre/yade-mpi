#include "ThreadSynchronizer.hpp"

ThreadSynchronizer::ThreadSynchronizer(bool bo) : b(bo)
{ 

}

ThreadSynchronizer::~ThreadSynchronizer()
{ 

}
	
void ThreadSynchronizer::wait1()
{
	boost::mutex::scoped_lock lock(mutex);
	while (b)
		cond.wait(lock);
}

void ThreadSynchronizer::wait2()
{
	boost::mutex::scoped_lock lock(mutex);
	while (!b)
		cond.wait(lock);
}

void ThreadSynchronizer::go()
{
	b=!b;
	cond.notify_one();
}
