/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/*

#ifndef THREADSYNCHRONIZER_HPP
#define THREADSYNCHRONIZER_HPP

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include <set>

using namespace std;
using namespace boost;

class ThreadSynchronizer
{	
	private :
		boost::mutex		mutex;
		boost::condition	cond;
		set<int>		ids;
		int			maxId;
		set<int>::iterator	currentId;

	
	public :
		boost::mutex& getMutex();
		ThreadSynchronizer();
	
		void insertThread(int* myTurn);
		void removeThread(int id);
	
		bool notMyTurn(int turn);
		void setNextCurrentThread();
	
		void wait(boost::mutex::scoped_lock& lock);
		void signal();
};

#endif // THREADSYNCHRONIZER_HPP

*/

