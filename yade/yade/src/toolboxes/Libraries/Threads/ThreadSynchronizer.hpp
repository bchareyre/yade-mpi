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

#ifndef __THREADSYNCHRONIZER_HPP__
#define __THREADSYNCHRONIZER_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include <set>
#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace boost;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class ThreadSynchronizer
{
	private : boost::mutex mutex;
	public  : boost::mutex* getMutex();
	
	private : boost::condition cond;
	
	//private : int i;
	public  : bool notMyTurn(int turn);
	public  : void setNextCurrentThread();

	//private : int prevI;
	//private : int * nbThreads;
	//private : vector<int> redirectionId;
	private : set<int> ids;
	private : set<int>::iterator currentId;
	private : int maxId;
	
	public  : ThreadSynchronizer();

	public  : int getNbThreads();
	
	public  : int insertThread();
	public  : void removeThread(int id);
	
	public  : void wait(boost::mutex::scoped_lock& lock);
	public  : void signal();
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __THREADSYNCHRONIZER_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
