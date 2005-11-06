/***************************************************************************
 *   Copyright (C) 2005 by Olivier Galizzi   *
 *   olivier.galizzi@imag.fr   *
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

#ifndef __BODYCONTAINERITERATORPOINTER__
#define __BODYCONTAINERITERATORPOINTER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "BodyContainerIterator.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class BodyContainerIteratorPointer
{
	private : shared_ptr<BodyContainerIterator> ptr;
	public  : BodyContainerIterator& getRef() {return *ptr;};
	public  : BodyContainerIterator& getRef() const {return *ptr;};
	public  : shared_ptr<BodyContainerIterator> get() {return ptr;};
	public  : shared_ptr<BodyContainerIterator> get() const {return ptr;};


	public  : BodyContainerIteratorPointer(const BodyContainerIteratorPointer& bi) 
	{
		allocate(bi);
		ptr->affect(bi.getRef());
	};

	public  : BodyContainerIteratorPointer(const shared_ptr<BodyContainerIterator>& i)
	{
		ptr = i;
	};

	public  : BodyContainerIteratorPointer() 
	{
		ptr = shared_ptr<BodyContainerIterator>();
	};

	private : void allocate(const BodyContainerIteratorPointer& bi)
	{
		if (ptr==0)
			ptr = bi.get()->createPtr();
	}

	public  : bool operator!=(const BodyContainerIteratorPointer& bi)
	{
		return ptr->isDifferent(bi.getRef());
	};

	public  : BodyContainerIteratorPointer& operator++()
	{
		ptr->increment();
		return *this;
	};

//	public  : BodyContainerIteratorPointer& operator++(int)
//	{
		// FIXME - this is bad. because it returns incremented value.
		//         the real solution is to copy whole class (duplicate in memory)
		//         but it is ineffective. so it's better to disable this operator.
//		BodyContainerIteratorPointer& tmp = *this;
//		ptr->increment();
//		return tmp;
//	};

	public  : BodyContainerIteratorPointer& operator=(const BodyContainerIteratorPointer& bi)
	{
		allocate(bi);
			
		ptr->affect(bi.getRef());
		return *this;
	};

	public  : shared_ptr<Body> operator*()
	{
		return ptr->getValue();
	};	
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODYCONTAINERITERATORPOINTER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

