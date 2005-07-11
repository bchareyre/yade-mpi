/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysicalActionVectorVector.hpp"
#include "PhysicalActionVectorVectorIterator.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/PhysicalAction.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionVectorVector::PhysicalActionVectorVector()
{
	clear();
//	currentIndex = -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionVectorVector::~PhysicalActionVectorVector()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void PhysicalActionVectorVector::clear()
{
	actionParameters.clear();
	usedIds.clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// doesn't not delete all, just resets data
void PhysicalActionVectorVector::reset()
{
	vector< vector< shared_ptr<PhysicalAction> > >::iterator vvi    = actionParameters.begin();
	vector< vector< shared_ptr<PhysicalAction> > >::iterator vviEnd = actionParameters.end();
	for( ; vvi != vviEnd ; ++vvi )
	{
		vector< shared_ptr<PhysicalAction> >::iterator vi    = (*vvi).begin();
		vector< shared_ptr<PhysicalAction> >::iterator viEnd = (*vvi).end();
		for( ; vi != viEnd ; ++vi)
		//if(*vi) // FIXME ?? do not check - all fields are NOT empty.
			(*vi)->reset();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int PhysicalActionVectorVector::size()
{
	return actionParameters.size();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// finds and returns action of given polymorphic type, for body of given Id,
// should be always succesfull. if it is not - you forgot to call prepare()
shared_ptr<PhysicalAction>& PhysicalActionVectorVector::find(unsigned int id , int actionIndex )
{
	if( actionParameters.size() <= id ) // this is very rarely executed, only at beginning.
	// somebody is accesing out of bounds, make sure he will find, what he needs - a resetted PhysicalAction of his type
	{
		unsigned int oldSize = actionParameters.size();
		unsigned int newSize = id+1;
		usedIds.resize(newSize,false);
		actionParameters.resize(newSize);
		for(unsigned int i = oldSize ; i < newSize ; ++i )
		{
			unsigned int actionTypesSize = actionTypesResetted.size();
			actionParameters[i].resize(actionTypesSize);
			for( unsigned int j = 0 ; j < actionTypesSize ; ++j )
				actionParameters[i][j] = actionTypesResetted[j]->clone();
		}
	}
	usedIds[id] = true;
	return actionParameters[id][actionIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionContainer::iterator PhysicalActionVectorVector::begin()
{
	shared_ptr<PhysicalActionVectorVectorIterator> it(new PhysicalActionVectorVectorIterator());
	it->currentIndex = 0;
	it->vvi		 = actionParameters.begin();
	it->vviEnd	 = actionParameters.end();
	while(it->vvi != it->vviEnd)
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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionContainer::iterator PhysicalActionVectorVector::end()
{
	shared_ptr<PhysicalActionVectorVectorIterator> it(new PhysicalActionVectorVectorIterator());
	it->currentIndex = actionParameters.size();
	it->vvi		 = actionParameters.end();
	it->vviEnd	 = actionParameters.end();
	it->vi		 = (*it->vvi).end();
	it->viEnd	 = (*it->vvi).end();

	return PhysicalActionContainer::iterator(it);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

