/***************************************************************************
 *   Copyright (C) 2005 by Olivier Galizzi                                 *
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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __BODYCONTAINER__
#define __BODYCONTAINER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
#include <iostream>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace boost;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
class BodyIterator 
{
	public : BodyIterator() {};
	public : virtual ~BodyIterator() {};
	
	public : virtual bool isDifferent(const BodyIterator&) { throw;};
	public : virtual void affect(const BodyIterator&) { throw;};
	public : virtual void increment()    { throw; };	
	public : virtual int getValue() {throw;};
	public : virtual shared_ptr<BodyIterator> createPtr() {throw;};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BodyIteratorPointer
{
	private : shared_ptr<BodyIterator> ptr;
	public  : BodyIterator& getRef() {return *ptr;};
	public  : BodyIterator& getRef() const {return *ptr;};
	public  : shared_ptr<BodyIterator> get() {return ptr;};
	public  : shared_ptr<BodyIterator> get() const {return ptr;};


	public  : BodyIteratorPointer(const BodyIteratorPointer& bi) 
	{
		allocate(bi);
		ptr->affect(bi.getRef());
	};

	public  : BodyIteratorPointer(const shared_ptr<BodyIterator>& i) 
	{
		ptr = i;
	};

	public  : BodyIteratorPointer() 
	{
		ptr = shared_ptr<BodyIterator>();
	};

	public  : bool operator!=(const BodyIteratorPointer& bi)
	{
		return ptr->isDifferent(bi.getRef());
	};

	public  : BodyIteratorPointer& operator++()
	{
		ptr->increment();
		return *this;
	};

	public  : BodyIteratorPointer& operator++(int)
	{
		ptr->increment();
		return *this;
	};

	public  : BodyIteratorPointer& operator=(const BodyIteratorPointer& bi)
	{
		allocate(bi);
			
		ptr->affect(bi.getRef());
		return *this;
	};

	public  : int operator*()
	{
		return ptr->getValue();
	};

	
	private : void allocate(const BodyIteratorPointer& bi)
	{
		if (ptr==0)
			ptr = bi.get()->createPtr();
	}	

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BodyContainer
{
	public : typedef BodyIteratorPointer iterator;
 
	public : BodyContainer() {};
	public : virtual ~BodyContainer() {};	
	
	public : virtual BodyIteratorPointer begin() { throw;} ;
	public : virtual BodyIteratorPointer end() { throw;} ;	
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODYCONTAINER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
