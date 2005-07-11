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

class BodyIterator;

template<class Type>
class BodyIteratorSharedPtr : public shared_ptr<Type>
{
	public : BodyIteratorSharedPtr(Type* bi) : shared_ptr<Type>(bi) {};
	public : BodyIteratorSharedPtr() : shared_ptr<Type>(new Type) {};

	public : bool operator!=(const BodyIteratorSharedPtr<BodyIterator>& bi)
	{			
		return this->get()->operator!=(*(bi.get()));
	};

	public : BodyIteratorSharedPtr<BodyIterator> operator++()
	{
		this->get()->operator++();
		return *this;
	};

	public : BodyIteratorSharedPtr<BodyIterator> operator++(int)
	{
		this->get()->operator++(0);
		return *this;
	};

	public : BodyIteratorSharedPtr<BodyIterator> operator=(const BodyIteratorSharedPtr<BodyIterator>& bi)
	{
		this->get()->operator=(*(bi.get()));
		return *this;
	};

	public : int operator*()
	{
		return this->get()->getCurrent();
	};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BodyIterator 
{
	public : BodyIterator() {};
	public : virtual ~BodyIterator() {};
	
	public : virtual bool operator!=(const BodyIterator&) { throw;};
	public : virtual void operator=(const BodyIterator&) { throw;};
	public : virtual void operator++()    { throw; };	
	public : virtual void operator++(int) { throw; };
	public : virtual int getCurrent() {throw;};
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BodyContainer
{
	public : typedef BodyIteratorSharedPtr<BodyIterator> iterator;
 
	public : BodyContainer() {};
	public : virtual ~BodyContainer() {};	
	
	public : virtual BodyIteratorSharedPtr<BodyIterator> begin() { throw;} ;
	public : virtual BodyIteratorSharedPtr<BodyIterator> end() { throw;} ;	
	public : virtual BodyIteratorSharedPtr<BodyIterator> emptyIterator() { throw;} ;	
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODYCONTAINER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
