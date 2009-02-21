/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/core/PhysicalAction.hpp>
#include<iostream>

// experimental
#ifndef NO_BEX
#	define BEX_CONTAINER
#endif

#ifdef BEX_CONTAINER
	#include<yade/core/BexContainer.hpp>
#endif

using namespace boost;
using namespace std;

class PhysicalActionContainerIterator 
{
	public :
		int currentIndex;

		PhysicalActionContainerIterator() 		{};
		virtual ~PhysicalActionContainerIterator()	{};
	
		virtual bool isDifferent(const PhysicalActionContainerIterator&)	{ throw;};
		virtual void affect(const PhysicalActionContainerIterator&)		{ throw;};
		virtual void increment()						{ throw;};
		virtual shared_ptr<PhysicalAction> getValue()				{ throw;};
		virtual shared_ptr<PhysicalActionContainerIterator> createPtr()		{ throw;};
		virtual int getCurrentIndex()						{ throw;};
};

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



// this container is different: it has ALWAYS data inside, not empty pointers at all. Every field has
// inside the right pointer type so it can be safely (and quickly) static_casted, so that the user himself
// is doing addition, substraction, and whatever he wants.
//
// you should never have to create new PhysicalAction ! (that takes too much time), except for calling prepare, which is done only once
 
class PhysicalActionContainer : public Serializable
{
	public :
		PhysicalActionContainer();
		virtual ~PhysicalActionContainer();

		virtual void clear() 							{throw;};
	
		// doesn't delete all, just resets data
		virtual void reset() 							{throw;};
		virtual unsigned int size() 						{throw;};
		// fills container with resetted fields. argument here, should be all PhysicalAction types that are planned to use
		virtual void prepare(std::vector<shared_ptr<PhysicalAction> >& )			{throw;};
	
		// finds and returns action of given polymorphic type, for body of given Id,
		// should be always succesfull. if it is not - you forgot to call prepare()
		virtual shared_ptr<PhysicalAction>& find(
					  unsigned int /*Body->getId() */
					, int /*Force::getClassIndexStatic()*/)		{throw;};

		typedef PhysicalActionContainerIteratorPointer iterator;
		virtual PhysicalActionContainer::iterator begin()			{throw;};
		virtual PhysicalActionContainer::iterator end()			{throw;};

	REGISTER_ATTRIBUTES(/*no base*/,(action));
	REGISTER_CLASS_AND_BASE(PhysicalActionContainer,Serializable);

	// local storage for uniform serialization of all possible container concrete implementations.
	private	:
		vector<shared_ptr<PhysicalAction> > action;
};

REGISTER_SERIALIZABLE(PhysicalActionContainer);

