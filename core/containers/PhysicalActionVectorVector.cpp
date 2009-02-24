/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"PhysicalActionVectorVector.hpp"
#include<yade/core/Body.hpp>
#include<yade/core/PhysicalAction.hpp>

PhysicalActionVectorVectorIterator::PhysicalActionVectorVectorIterator() : PhysicalActionContainerIterator()
{
	currentIndex = -1;
}


PhysicalActionVectorVectorIterator::~PhysicalActionVectorVectorIterator()
{

}


bool PhysicalActionVectorVectorIterator::isDifferent(const PhysicalActionContainerIterator& i)
{
	const PhysicalActionVectorVectorIterator& it = static_cast<const PhysicalActionVectorVectorIterator&>(i);
	if (it.vvi==it.vviEnd)
		return !(vvi==vviEnd);
	else
		return (vi != it.vi );
}


void PhysicalActionVectorVectorIterator::increment()
{
	++vi;
	if(vi == viEnd)
	{
		++vvi;
		++currentIndex;
		while(!(*usedIds)[currentIndex] && vvi != vviEnd)
		{
			++currentIndex;
			++vvi;
		}
		if(vvi != vviEnd)
		{
			vi	= (*vvi).begin();
			viEnd	= (*vvi).end();
		}
	}
}


void PhysicalActionVectorVectorIterator::affect(const PhysicalActionContainerIterator& i)
{
	const PhysicalActionVectorVectorIterator& tmpi = static_cast<const PhysicalActionVectorVectorIterator&>(i);
	vi     = tmpi.vi;
	viEnd  = tmpi.viEnd;
	vvi    = tmpi.vvi;
	vviEnd = tmpi.vviEnd;
	currentIndex = tmpi.currentIndex;
	usedIds	= tmpi.usedIds;
}


shared_ptr<PhysicalAction> PhysicalActionVectorVectorIterator::getValue()
{	
	return *vi;
}


int PhysicalActionVectorVectorIterator::getCurrentIndex()
{
	return currentIndex;
}


shared_ptr<PhysicalActionContainerIterator> PhysicalActionVectorVectorIterator::createPtr()
{
	return shared_ptr<PhysicalActionContainerIterator>(new PhysicalActionVectorVectorIterator());
}


/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

PhysicalActionVectorVector::PhysicalActionVectorVector()
{
	clear();
//	currentIndex = -1;
	current_size = 0;
}


PhysicalActionVectorVector::~PhysicalActionVectorVector()
{
}


void PhysicalActionVectorVector::clear()
{
	physicalActions.clear();
	usedIds.clear();
	current_size = physicalActions.size();
}


// doesn't not delete all, just resets data
void PhysicalActionVectorVector::reset()
{
	#if 1
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
	#else
		FOREACH(int idx, usedBexIndices){
			// reset everything
			FOREACH(shared_ptr<PhysicalAction>& pa,physicalActions[idx]){ pa->reset();}
		}
	#endif
}


unsigned int PhysicalActionVectorVector::size()
{
	return current_size;
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
	usedBexIndices.clear();
	for(unsigned int i = 0 ; i < size ; ++i )
	{
		int idx=actionTypes[i]->getClassIndex();
		actionTypesResetted[idx] = actionTypes[i]->clone();
		actionTypesResetted[idx] -> reset();
		usedBexIndices.push_back(idx);
	}
}


// finds and returns action of given polymorphic type, for body of given Id,
// should be always succesfull. if it is not - you forgot to call prepare()
shared_ptr<PhysicalAction>& PhysicalActionVectorVector::find(unsigned int id , int actionIndex )
{
	#ifdef BEX_CONTAINER
		cerr<<"FATAL: This build of yade uses nex BexContainer instead of PhysicalActionContainer.\nFATAL: However, your simulation still uses PhysicalActionContainer.\nFATAL: Update your code, see backtrace (if in debug build) to find where the old container is used."<<endl;
		throw std::runtime_error("Deprecated PhysicalActionContainer is not supported in this build!");
	#endif
	if( current_size <= id ) // this is very rarely executed, only at beginning.
	// somebody is accesing out of bounds, make sure he will find, what he needs - a resetted PhysicalAction of his type
	{
		unsigned int oldSize = physicalActions.size();
		unsigned int newSize = id+1;
		current_size = newSize;
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
	/* Using nested iterator when the first one is end() already would be error, therefore we leave it->vi and it->viEnd alone.
	 * (see PhysicalActionVectorVectorIterator::isDifferent for exact implementation of !=end()) */
	return PhysicalActionContainer::iterator(it);

}


// YADE_PLUGIN();
