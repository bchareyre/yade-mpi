/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/*

#include "ThreadSynchronizer.hpp"
#include "ThreadSafe.hpp"

ThreadSynchronizer::ThreadSynchronizer() : 	maxId(0),
						currentId(ids.begin())
{

}


void ThreadSynchronizer::insertThread(int* myTurn)
{ 
	boost::mutex::scoped_lock lock(mutex);
	
	ids.insert(maxId);
	currentId = ids.begin();
	*myTurn = maxId++;
	signal();
}


void ThreadSynchronizer::removeThread(int id)
{ 
	boost::mutex::scoped_lock lock(mutex);	
	
	ids.erase(id);
	currentId = ids.begin();
	if (id==maxId)
		maxId--;
	signal();	
}


boost::mutex& ThreadSynchronizer::getMutex()
{	
	return mutex;
}
	

bool ThreadSynchronizer::notMyTurn(int turn)
{
	return (turn != *currentId);
}


void ThreadSynchronizer::setNextCurrentThread()
{	
	++currentId;
	if (currentId==ids.end())
		currentId = ids.begin();
//	ThreadSafe::cout("Next turn : "+lexical_cast<string>(*currentId));	
}


void ThreadSynchronizer::wait(boost::mutex::scoped_lock& lock)
{
	cond.wait(lock);
}


void ThreadSynchronizer::signal()
{
	cond.notify_all();
}

*/

