/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTIONCONTAINER_HPP
#define PHYSICALACTIONCONTAINER_HPP

#include<yade/lib-serialization/Serializable.hpp>
#include "PhysicalActionContainerIteratorPointer.hpp"

class PhysicalAction;

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

/// Serialization										///
	protected :
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(PhysicalActionContainer);
	REGISTER_BASE_CLASS_NAME(Serializable);

	// local storage for uniform serialization of all possible container concrete implementations.
	private	:
		vector<shared_ptr<PhysicalAction> > action;
};

REGISTER_SERIALIZABLE(PhysicalActionContainer,false);

#endif // __ACTIONCONTAINER_HPP__

