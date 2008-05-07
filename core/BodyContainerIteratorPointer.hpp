/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BODYCONTAINERITERATORPOINTER_HPP
#define BODYCONTAINERITERATORPOINTER_HPP

#include "BodyContainerIterator.hpp"

class BodyContainerIteratorPointer
{
	private :
		shared_ptr<BodyContainerIterator> ptr;
		void allocate(const BodyContainerIteratorPointer& bi)
		{
			if (ptr==0) ptr = bi.get()->createPtr();
		}


	public :
		BodyContainerIterator&			getRef()	{ return *ptr; };
		BodyContainerIterator&			getRef() const	{ return *ptr; };
		shared_ptr<BodyContainerIterator>	get()		{ return  ptr; };
		shared_ptr<BodyContainerIterator>	get() const	{ return  ptr; };

		BodyContainerIteratorPointer(const shared_ptr<BodyContainerIterator>& i)	{ ptr = i; };

		BodyContainerIteratorPointer()		{ ptr = shared_ptr<BodyContainerIterator>(); };

		BodyContainerIteratorPointer(const BodyContainerIteratorPointer& bi) 
		{
			allocate(bi);
			ptr->affect(bi.getRef());
		};

		bool operator!=(const BodyContainerIteratorPointer& bi)	{ return ptr->isDifferent(bi.getRef());	};
		bool operator==(const BodyContainerIteratorPointer& bi)	{ return !ptr->isDifferent(bi.getRef());	};
		shared_ptr<Body>			operator*()	{ return ptr->getValue(); };	
		BodyContainerIteratorPointer&		operator++()	{ ptr->increment(); return *this; };
		BodyContainerIteratorPointer&		operator++(int); // disabled 
		BodyContainerIteratorPointer& operator=(const BodyContainerIteratorPointer& bi)
		{
			allocate(bi);
			ptr->affect(bi.getRef());
			return *this;
		};

};

#endif // BODYCONTAINERITERATORPOINTER_HPP

