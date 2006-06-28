/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/*

#ifndef THREADABLE_HPP
#define THREADABLE_HPP

#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>
#include "ThreadSafe.hpp"

using namespace boost;

class ThreadSynchronizer;

template<class Thread>
class Threadable
{
	protected :
		boost::mutex * mutex;
		int * turn;
		shared_ptr<ThreadSynchronizer> synchronizer;

	private :
		bool  * finished;
		bool  * blocked;	
		bool  * singleLoop;	
		shared_ptr<boost::thread> thread;

	public :
		Threadable(shared_ptr<ThreadSynchronizer> s);
		virtual ~Threadable();

		void operator()();
		void createThread();
		
		virtual bool notEnd()  = 0;
		virtual void oneLoop() = 0;

		void sleep(int ms);
		void join();
		void start();
		void stop();
		bool isStopped();
		void doOneLoop();
		void finish();

};

#include "Threadable.tpp"

#endif // THREADABLE_HPP

*/

