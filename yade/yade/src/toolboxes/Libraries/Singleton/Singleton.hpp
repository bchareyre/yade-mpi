/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurg@berlios.de                                                     *
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SINGLETON_HPP__
#define __SINGLETON_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/thread/mutex.hpp>

/**

This is a Singleton template. You can make a singleton out of any class that
you provide here as a template argument.

it uses the simplest approach to singletons - Meyers Singleton. So there will
be undefined behaviuour, when destructor of one singleton tries to access
another singleton (which may be already destructed).

maybe later we will improve this, into a Phoenix Singleton (which raises
from ashes if someone tries to access it, and it's destroyed)

*/

namespace {
	boost::mutex singleton_constructor_mutex;
}

template <class T>
class Singleton
{
	public:
		static T& instance()
		{
			static T* ptr_obj;
//			static shared_ptr<T> ptr_obj; // FIXME - try this - it may work great !!

			if( ! ptr_obj )
			{
				boost::mutex::scoped_lock lock(singleton_constructor_mutex);

				// WARNING : as stated in "Modern C++ Design" this sometimes may not be enough. Some processors do write to memory in bursts, not when the write is executed. It means that other processors will not know that the mutex has been locked. He writes, that in such situation we have to look at certain processor architecture documentation and write a solution specifically for given processor architecture.

				if( ! ptr_obj )
				{
					ptr_obj = new T; // FIXME - this new has no delete. we have a memory leak at exit. I'll just put here a Loki::Singleton, and everything will be OK.
				}
			}

			return *ptr_obj;
		}
	protected:
		Singleton() {};
		~Singleton() {
		 // FIXME - maybe this:
		 // delete & (instance()) ; ?? or sth like that....
		};
	private:
		Singleton(const Singleton&);
		Singleton& operator=(const Singleton&);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SINGLETON_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
