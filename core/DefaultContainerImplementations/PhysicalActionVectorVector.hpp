/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTIONVECTORVECTOR_HPP
#define PHYSICALACTIONVECTORVECTOR_HPP

#include<yade/core/PhysicalActionContainer.hpp>
#include<list>
#include<vector>

class PhysicalAction;

using namespace boost;
using namespace std;

class PhysicalActionVectorVectorIterator : public PhysicalActionContainerIterator
{
	public :
		vector< vector< shared_ptr<PhysicalAction> > >::iterator vvi;
		vector< vector< shared_ptr<PhysicalAction> > >::iterator vviEnd;
			vector< shared_ptr<PhysicalAction> >  ::iterator vi;
		vector< shared_ptr<PhysicalAction> >  ::iterator viEnd;
		vector< bool > * usedIds;

	public :
		PhysicalActionVectorVectorIterator();
		~PhysicalActionVectorVectorIterator();

		virtual bool isDifferent(const PhysicalActionContainerIterator& i);
		virtual void affect(const PhysicalActionContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<PhysicalAction> getValue();
		virtual shared_ptr<PhysicalActionContainerIterator> createPtr();
		virtual int getCurrentIndex();
};



class PhysicalActionVectorVector : public PhysicalActionContainer
{
	private	:
	// this in fact should be also a RedirectionVector in respect to the Body.id
	// this container is memory-consuming, because size of this vector is depending on highest id
	// from all bodies, not on the number of bodies
	
	// in this two-dimensional table:
	// 	- first  dimension is Body->getId() number
	//	- second dimension is PhysicalAction->getClassIndex() number
		vector< vector< shared_ptr<PhysicalAction> > > physicalActions;
			vector< shared_ptr<PhysicalAction> >   actionTypesResetted;
		vector< bool > usedIds;
		unsigned int current_size;
	
	public :
		PhysicalActionVectorVector();
		virtual ~PhysicalActionVectorVector();

		virtual void clear();

		// doesn't delete all, just resets data
		virtual void reset();
		virtual unsigned int size();
		// fills container with resetted fields. argument here, should be all PhysicalAction types that are planned to use
		virtual void prepare(std::vector<shared_ptr<PhysicalAction> >& );
	
		// finds and returns action of given polymorphic type, for body of given Id,
		// should be always succesfull. if it is not - you forgot to call prepare()
		virtual shared_ptr<PhysicalAction>& find(
					  unsigned int /*Body->getId() */
					, int /*ActionForce::getClassIndexStatic()*/);

		virtual PhysicalActionContainer::iterator begin();
		virtual PhysicalActionContainer::iterator end();

/// Serialization
	REGISTER_CLASS_NAME(PhysicalActionVectorVector);
	REGISTER_BASE_CLASS_NAME(PhysicalActionContainer);

};

REGISTER_SERIALIZABLE(PhysicalActionVectorVector);

#endif // __ACTION_VEC_VEC_HPP__

