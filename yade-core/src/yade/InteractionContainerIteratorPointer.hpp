/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONCONTAINERITERATORPOINTER_HPP
#define INTERACTIONCONTAINERITERATORPOINTER_HPP

#include "InteractionContainerIterator.hpp"

class InteractionContainerIteratorPointer
{
	private :
		shared_ptr<InteractionContainerIterator> ptr;
		void allocate(const InteractionContainerIteratorPointer& bi)
		{
			if (ptr==0)
				ptr = bi.get()->createPtr();
		}


	public  :
		InteractionContainerIterator&			getRef()	{ return *ptr; };
		InteractionContainerIterator&			getRef() const	{ return *ptr; };
		shared_ptr<InteractionContainerIterator>	get()		{ return  ptr; };
		shared_ptr<InteractionContainerIterator>	get() const	{ return  ptr; };

		InteractionContainerIteratorPointer(const InteractionContainerIteratorPointer& bi) 
		{
			allocate(bi);
			ptr->affect(bi.getRef());
		};

		InteractionContainerIteratorPointer(const shared_ptr<InteractionContainerIterator>& i) { ptr = i; };
		InteractionContainerIteratorPointer()  { ptr = shared_ptr<InteractionContainerIterator>(); };

		bool operator!=(const InteractionContainerIteratorPointer& bi) { return ptr->isDifferent(bi.getRef()); };
		shared_ptr<Interaction>			operator*() { return ptr->getValue(); };	
		InteractionContainerIteratorPointer&	operator++() { ptr->increment(); return *this; };
		InteractionContainerIteratorPointer&	operator++(int); // disabled
		InteractionContainerIteratorPointer&	operator=(const InteractionContainerIteratorPointer& bi)
		{
			allocate(bi);
			ptr->affect(bi.getRef());
			return *this;
		};

};

#endif // INTERACTIONCONTAINERITERATORPOINTER_HPP

