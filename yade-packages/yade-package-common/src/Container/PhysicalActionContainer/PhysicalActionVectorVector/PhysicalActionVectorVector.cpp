/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalActionVectorVector.hpp"
#include "PhysicalActionVectorVectorIterator.hpp"
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/PhysicalAction.hpp>


PhysicalActionVectorVector::PhysicalActionVectorVector()
{
	clear();
//	currentIndex = -1;
}


PhysicalActionVectorVector::~PhysicalActionVectorVector()
{
}


void PhysicalActionVectorVector::clear()
{
	physicalActions.clear();
	usedIds.clear();
}


// doesn't not delete all, just resets data
void PhysicalActionVectorVector::reset()
{
	vector< vector< shared_ptr<PhysicalAction> > >::iterator vvi    = physicalActions.begin();
	vector< vector< shared_ptr<PhysicalAction> > >::iterator vviEnd = physicalActions.end();
	for( ; vvi != vviEnd ; ++vvi )
	{
		vector< shared_ptr<PhysicalAction> >::iterator vi    = (*vvi).begin();
		vector< shared_ptr<PhysicalAction> >::iterator viEnd = (*vvi).end();
		for( ; vi != viEnd ; ++vi)
		//if(*vi) // FIXME ?? do not check - all fields are NOT empty.
			(*vi)->reset();
	}
}


unsigned int PhysicalActionVectorVector::size()
{
	return physicalActions.size();
}


// fills container with resetted fields. argument here, should be all PhysicalAction types that are planned to use
void PhysicalActionVectorVector::prepare(std::vector<shared_ptr<PhysicalAction> >& actionTypes)
{
	unsigned int size = actionTypes.size();
	int maxSize = 0;
	for(unsigned int i = 0 ; i < size ; ++i)
		maxSize = max(maxSize , actionTypes[i]->getClassIndex() );
	++maxSize;
	actionTypesResetted.resize(maxSize);
	for(unsigned int i = 0 ; i < size ; ++i )
	{
		actionTypesResetted[actionTypes[i]->getClassIndex()] = actionTypes[i]->clone();
		actionTypesResetted[actionTypes[i]->getClassIndex()] -> reset();
	}
}


// finds and returns action of given polymorphic type, for body of given Id,
// should be always succesfull. if it is not - you forgot to call prepare()
shared_ptr<PhysicalAction>& PhysicalActionVectorVector::find(unsigned int id , int actionIndex )
{
	if( physicalActions.size() <= id ) // this is very rarely executed, only at beginning.
	// somebody is accesing out of bounds, make sure he will find, what he needs - a resetted PhysicalAction of his type
	{
		unsigned int oldSize = physicalActions.size();
		unsigned int newSize = id+1;
		usedIds.resize(newSize,false);
		physicalActions.resize(newSize);
		for(unsigned int i = oldSize ; i < newSize ; ++i )
		{
			unsigned int actionTypesSize = actionTypesResetted.size();
			physicalActions[i].resize(actionTypesSize);
			for( unsigned int j = 0 ; j < actionTypesSize ; ++j )
				physicalActions[i][j] = actionTypesResetted[j]->clone();
		}
	}
	usedIds[id] = true;
	return physicalActions[id][actionIndex];
}


PhysicalActionContainer::iterator PhysicalActionVectorVector::begin()
{
	shared_ptr<PhysicalActionVectorVectorIterator> it(new PhysicalActionVectorVectorIterator());
	it->currentIndex = 0;
	it->usedIds	 = &usedIds;
	it->vvi		 = physicalActions.begin();
	it->vviEnd	 = physicalActions.end();
	while(it->vvi!=it->vviEnd && !usedIds[it->currentIndex])
	{
		++(it->currentIndex);
		++(it->vvi);
	}
	if(it->vvi != it->vviEnd)
	{
		it->vi     = (*it->vvi).begin();
		it->viEnd  = (*it->vvi).end();
	}

	return PhysicalActionContainer::iterator(it);
}


PhysicalActionContainer::iterator PhysicalActionVectorVector::end()
{
	shared_ptr<PhysicalActionVectorVectorIterator> it(new PhysicalActionVectorVectorIterator());
	it->currentIndex = physicalActions.size();
	it->vvi		 = physicalActions.end();
	it->vviEnd	 = physicalActions.end();
	it->vi		 = (*it->vvi).end();
	it->viEnd	 = (*it->vvi).end();

	return PhysicalActionContainer::iterator(it);

}


