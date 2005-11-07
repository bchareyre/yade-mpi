/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef THREADSAFE_HPP
#define THREADSAFE_HPP

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace boost;

class ThreadSafe
{
	private :
		static boost::mutex ioMutex;
	public :
		static void cerr(const string& s);
		static void cout(const string& s);
};

#define LOCK(m) 				\
	boost::mutex::scoped_lock lock(m)

#define LOCK2(m) 				\
	boost::mutex::scoped_lock lock2(m)

#endif // THREADSAFE_HPP

