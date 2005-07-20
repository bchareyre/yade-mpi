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

#ifndef __PHYSICALACTIONCONTAINERITERATORPOINTER__
#define __PHYSICALACTIONCONTAINERITERATORPOINTER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysicalActionContainerIterator.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PhysicalActionContainerIteratorPointer
{
	private : shared_ptr<PhysicalActionContainerIterator> ptr;
	public  : PhysicalActionContainerIterator& getRef() {return *ptr;};
	public  : PhysicalActionContainerIterator& getRef() const {return *ptr;};
	public  : shared_ptr<PhysicalActionContainerIterator> get() {return ptr;};
	public  : shared_ptr<PhysicalActionContainerIterator> get() const {return ptr;};


	public  : PhysicalActionContainerIteratorPointer(const PhysicalActionContainerIteratorPointer& bi) 
	{
		allocate(bi);
		ptr->affect(bi.getRef());
	};

	public  : PhysicalActionContainerIteratorPointer(const shared_ptr<PhysicalActionContainerIterator>& i)
	{
		ptr = i;
	};

	public  : PhysicalActionContainerIteratorPointer() 
	{
		ptr = shared_ptr<PhysicalActionContainerIterator>();
	};

	private : void allocate(const PhysicalActionContainerIteratorPointer& bi)
	{
		if (ptr==0)
			ptr = bi.get()->createPtr();
	}

	public  : bool operator!=(const PhysicalActionContainerIteratorPointer& bi)
	{
		return ptr->isDifferent(bi.getRef());
	};

	public  : PhysicalActionContainerIteratorPointer& operator++()
	{
		ptr->increment();
		return *this;
	};

	public  : PhysicalActionContainerIteratorPointer& operator++(int)
	{
		PhysicalActionContainerIteratorPointer& tmp = *this;
		ptr->increment();
		return tmp;
	};

	public  : PhysicalActionContainerIteratorPointer& operator=(const PhysicalActionContainerIteratorPointer& bi)
	{
		allocate(bi);
			
		ptr->affect(bi.getRef());
		return *this;
	};

	public  : shared_ptr<PhysicalAction> operator*()
	{
		return ptr->getValue();
	};

	public : int getCurrentIndex()
	{
		return ptr->getCurrentIndex();
	};

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __PHYSICALACTIONCONTAINERITERATORPOINTER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

