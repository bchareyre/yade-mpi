/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTIONCONTAINERITERATORPOINTER_HPP
#define PHYSICALACTIONCONTAINERITERATORPOINTER_HPP

#include "PhysicalActionContainerIterator.hpp"

class PhysicalActionContainerIteratorPointer
{
	private :
		shared_ptr<PhysicalActionContainerIterator> ptr;
		void allocate(const PhysicalActionContainerIteratorPointer& bi)
		{
			if (ptr==0) ptr = bi.get()->createPtr();
		}

	public :
		PhysicalActionContainerIterator& getRef() {return *ptr;};
		PhysicalActionContainerIterator& getRef() const {return *ptr;};
		shared_ptr<PhysicalActionContainerIterator> get() {return ptr;};
		shared_ptr<PhysicalActionContainerIterator> get() const {return ptr;};

		PhysicalActionContainerIteratorPointer(const PhysicalActionContainerIteratorPointer& bi)
		{
			allocate(bi);
			ptr->affect(bi.getRef());
		};

		PhysicalActionContainerIteratorPointer(const shared_ptr<PhysicalActionContainerIterator>& i) { ptr = i; };
		PhysicalActionContainerIteratorPointer()  { ptr = shared_ptr<PhysicalActionContainerIterator>(); };
		bool operator!=(const PhysicalActionContainerIteratorPointer& bi) { return ptr->isDifferent(bi.getRef()); };

		PhysicalActionContainerIteratorPointer& operator=(const PhysicalActionContainerIteratorPointer& bi)
		{
			allocate(bi);
			ptr->affect(bi.getRef());
			return *this;
		};

		PhysicalActionContainerIteratorPointer& operator++()	{ ptr->increment(); return *this; };
		PhysicalActionContainerIteratorPointer& operator++(int);  // disabled
		shared_ptr<PhysicalAction> operator*()			{ return ptr->getValue(); };
		int getCurrentIndex()					{ return ptr->getCurrentIndex(); };

};

#endif // __PHYSICALACTIONCONTAINERITERATORPOINTER__


