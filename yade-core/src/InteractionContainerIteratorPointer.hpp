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

#ifndef __INTERACTIONCONTAINERITERATORPOINTER__
#define __INTERACTIONCONTAINERITERATORPOINTER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InteractionContainerIterator.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class InteractionContainerIteratorPointer
{
	private : shared_ptr<InteractionContainerIterator> ptr;
	public  : InteractionContainerIterator& getRef() {return *ptr;};
	public  : InteractionContainerIterator& getRef() const {return *ptr;};
	public  : shared_ptr<InteractionContainerIterator> get() {return ptr;};
	public  : shared_ptr<InteractionContainerIterator> get() const {return ptr;};


	public  : InteractionContainerIteratorPointer(const InteractionContainerIteratorPointer& bi) 
	{
		allocate(bi);
		ptr->affect(bi.getRef());
	};

	public  : InteractionContainerIteratorPointer(const shared_ptr<InteractionContainerIterator>& i)
	{
		ptr = i;
	};

	public  : InteractionContainerIteratorPointer() 
	{
		ptr = shared_ptr<InteractionContainerIterator>();
	};

	private : void allocate(const InteractionContainerIteratorPointer& bi)
	{
		if (ptr==0)
			ptr = bi.get()->createPtr();
	}

	public  : bool operator!=(const InteractionContainerIteratorPointer& bi)
	{
		return ptr->isDifferent(bi.getRef());
	};

	public  : InteractionContainerIteratorPointer& operator++()
	{
		ptr->increment();
		return *this;
	};

	public  : InteractionContainerIteratorPointer& operator++(int)
	{
		ptr->increment();
		return *this;
	};

	public  : InteractionContainerIteratorPointer& operator=(const InteractionContainerIteratorPointer& bi)
	{
		allocate(bi);
			
		ptr->affect(bi.getRef());
		return *this;
	};

	public  : shared_ptr<Interaction> operator*()
	{
		return ptr->getValue();
	};	
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __INTERACTIONCONTAINERITERATORPOINTER__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

