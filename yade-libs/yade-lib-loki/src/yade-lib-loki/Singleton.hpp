/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <boost/thread/mutex.hpp>

/*!

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
//			static boost::shared_ptr<T> ptr_obj; // FIXME - try this - it may work great !!

			if( ! ptr_obj )
			{
				boost::mutex::scoped_lock lock(singleton_constructor_mutex);

				// WARNING : as stated in "Modern C++ Design" this sometimes may not be enough. Some processors do write to memory in bursts, not when the write is executed. It means that other processors will not know that the mutex has been locked. He writes, that in such situation we have to look at certain processor architecture documentation and write a solution specifically for given processor architecture.

				if( ! ptr_obj )
				{
					ptr_obj = new T; // FIXME - this new has no delete. we have a memory leak at exit. I'll just put here a Loki::Singleton, and everything will be OK.
//					ptr_obj = boost::shared_ptr<T>(new T);
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

#define FRIEND_SINGLETON(SomeClass)						\
	friend class Singleton< SomeClass >;					
//	friend class boost::checked_delete< SomeClass >;

#endif // SINGLETON_HPP

