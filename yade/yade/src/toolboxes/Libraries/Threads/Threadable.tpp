/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/thread/xtime.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ThreadSynchronizer.hpp"
#include "ThreadSafe.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
 
template<class Thread>
Threadable<Thread>::Threadable(shared_ptr<ThreadSynchronizer> s)
{
	synchronizer = s;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Thread>
Threadable<Thread>::~Threadable()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Thread>
void Threadable<Thread>::stop()
{ 
	saveTurn = turn;
	turn = -2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Thread>
void Threadable<Thread>::start()
{
	turn = saveTurn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

//#define THREAD_DEBUG
template<class Thread>
void Threadable<Thread>::operator()()
{
#ifndef THREAD_DEBUG
	if (synchronizer)
	{
		while (notEnd())
		{
			{
				boost::mutex::scoped_lock lock(*(synchronizer->getMutex()));
	
				while (synchronizer->notMyTurn(turn))
					synchronizer->wait(lock);
				
				oneLoop();
				
				synchronizer->setNextCurrentThread();
					
				synchronizer->signal();
			}
		}
		synchronizer->removeThread(turn);
	}
	else
	{
		while (notEnd())
			oneLoop();
	}
#else
	if (synchronizer)
	{
		while (notEnd())
		{
			{
				ThreadSafe::cerr("mark:  20 " + string(typeid(*this).name()) );
				boost::mutex::scoped_lock lock(*(synchronizer->getMutex()));
	
				ThreadSafe::cerr("mark:  21 " + string(typeid(*this).name()) );
				while ( synchronizer->redirectionId[turn] != synchronizer->i)
					synchronizer->cond.wait(lock);
				
				ThreadSafe::cerr("mark:  21 " + string(typeid(*this).name()) );
				oneLoop();
				
				ThreadSafe::cerr("mark:  22 " + string(typeid(*this).name()) );
				synchronizer->i=(synchronizer->i+1) % synchronizer->nbThreads;
				
				ThreadSafe::cerr("mark:  2" " + string(typeid(*this).name()) );
				while(synchronizer->redirectionId[synchronizer->i] == -1)
					synchronizer->i=(synchronizer->i+1) % synchronizer->nbThreads;
		
				ThreadSafe::cerr("mark:  24 " + string(typeid(*this).name()) );
				synchronizer->cond.notify_all();
			}
		}
		synchronizer->removeThread(turn);
	}
	else
	{
		while (notEnd())
			oneLoop();
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Thread>
int Threadable<Thread>::getTurn()
{
	return turn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Thread>
void Threadable<Thread>::createThread(shared_ptr<ThreadSynchronizer> s)
{
	synchronizer = s;
	if (synchronizer)
	{	
		turn = synchronizer->insertThread();
		saveTurn = turn;
		//turn = -2;
	}
	thread = shared_ptr<boost::thread>(new boost::thread(*(dynamic_cast<Thread*>(this))));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Thread>
void Threadable<Thread>::sleep(int ms)
{
	boost::xtime xt;
	boost::xtime_get(&xt, boost::TIME_UTC);
	xt.nsec += ms*1000000;
	
	boost::thread::sleep(xt);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
