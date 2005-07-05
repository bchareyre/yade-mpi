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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/PhysicalAction.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionVectorVector::PhysicalActionVectorVector()
{
	clear();
	currentIndex = -1;
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
	vvi    = actionParameters.begin();
	vviEnd = actionParameters.end();
	for( ; vvi != vviEnd ; ++vvi )
	{
		vi    = (*vvi).begin();
		viEnd = (*vvi).end();
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

// looping over Bodies, and their Actions
void PhysicalActionVectorVector::gotoFirst()
{
	currentIndex = 0;
	vvi    = actionParameters.begin();
	vviEnd = actionParameters.end();
	
	while( (vvi != vviEnd) && (! usedIds[currentIndex]) )
	{
		++currentIndex;
		++vvi;
	}
	if(vvi != vviEnd)
	{
		vi     = (*vvi).begin();
		viEnd  = (*vvi).end();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool PhysicalActionVectorVector::notAtEnd()
{
	return vvi != vviEnd;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void PhysicalActionVectorVector::gotoNext()
{
	++vi;
	if(vi == viEnd)
	{
		++vvi;
		++currentIndex;
		while( (! usedIds[currentIndex]) && (vvi != vviEnd) )
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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<PhysicalAction>& PhysicalActionVectorVector::getCurrent(int& id)
{
	id = currentIndex;
	return (*vi);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////













// OLD SLOWER container

//
// 
// PhysicalActionVectorVector::PhysicalActionVectorVector()
// {
// 	clear();
// 	currentActionType = -1;
// 	empty = shared_ptr<PhysicalAction>();
// 	currentIndex = -1;
// }
// 
// PhysicalActionVectorVector::~PhysicalActionVectorVector()
// {
// }
// 
// 
// void PhysicalActionVectorVector::clear()
// {
// 	actionParameters.clear();
// }
// 
// void PhysicalActionVectorVector::reset()
// {
// 	vvi    = actionParameters.begin();
// 	vviEnd = actionParameters.end();
// 	for( ; vvi < vviEnd ; ++vvi )
// 	{
// 		vi    = (*vvi).begin();
// 		viEnd = (*vvi).end();
// 		for( ; vi < viEnd ; ++vi)
// 			if(*vi)
// 				(*vi)->reset();
// 	}
// }
// 
// unsigned int PhysicalActionVectorVector::size()
// {
// 	return actionParameters.size();
// }
// 	
// adds PhysicalAction acting on one body,
// it is mathematically added if PhysicalAction of this polymorphic type already exists,
// if it doesn't exist, then it is appended to stored list of Actions for that body
// void PhysicalActionVectorVector::add(const shared_ptr<PhysicalAction>& newAction, unsigned int id)
// { 
// 	const int& idx = newAction->getClassIndex();
// 	
// 	assert(idx >= 0);
// 
// 	if( actionParameters.size() <= id )
// 		actionParameters.resize(id+1);
// 		
// 	if( actionParameters[id].size() <= (unsigned int)idx )
// 		actionParameters[id].resize(idx+1);
// 	
// 	if( actionParameters[id][idx] )
// 		actionParameters[id][idx]->add(newAction);
// 	else
// 		actionParameters[id][idx]=newAction->clone();
// }
// 
// adds PhysicalAction that acts on two bodies.
// on first body it is substarcted,
// to second body it is added
// void PhysicalActionVectorVector::add(const shared_ptr<PhysicalAction>&, unsigned int , unsigned int)
// {
// 
// }
// 	
// allows to set current polymorphic PhysicalAction Type on which other functions will work:
// function that use this are: eraseAction, operator[]
// void PhysicalActionVectorVector::setCurrentActionType(int idx)
// {
// 	currentActionType = idx;
// }
// 		
// deletes PhysicalAction of given polymorphic type from body that has given Id
// bool PhysicalActionVectorVector::eraseAction(unsigned int id, int idx)
// {
// 	if( idx >= 0 && (unsigned int)idx < actionParameters[id].size() && actionParameters[id][idx] )
// 	{
// 		actionParameters[id][idx] = shared_ptr<PhysicalAction>();
// 		return true;
// 	}
// 	else
// 		return false;
// }
// 
// deletes PhysicalAction of given polymorphic type from body that has given Id,
// the polymorphic type is selected by setCurrentActionType()
// returns true if action existed before deletion
// bool PhysicalActionVectorVector::eraseAction(unsigned int id)
// {
// 	if(	   currentActionType >= 0 
// 		&& (unsigned int)currentActionType < actionParameters[id].size()
// 		&& actionParameters[id][currentActionType] )
// 	{
// 		actionParameters[id][currentActionType] = shared_ptr<PhysicalAction>();
// 		return true;
// 	}
// 	else
// 		return false;
// }
// 
// deletes all Actions in a body of given Id
// void PhysicalActionVectorVector::erase(unsigned int id)
// {
// 	actionParameters[id].clear();
// }
// 	
// finds and returns action of given polymorphic type, for body of given Id,
// returns empty shared_ptr if this PhysicalAction doesn't exist for chosen body
// shared_ptr<PhysicalAction> PhysicalActionVectorVector::find(unsigned int id, int idx) const
// {
// 	if( 	   idx >= 0
// 		&& (unsigned int)idx < actionParameters[id].size() )
// 		return actionParameters[id][idx];
// 	else
// 		return shared_ptr<PhysicalAction>();
// }
// 
// same as above, polymorphic PhysicalAction type is selected with setCurrentActionType
// shared_ptr<PhysicalAction>& PhysicalActionVectorVector::operator[](unsigned int id)
// {
// 	if(	   currentActionType >=0
// 		&& (unsigned int)currentActionType < actionParameters[id].size() )
// 		return actionParameters[id][currentActionType];
// 	else
// 	{
// 		empty = shared_ptr<PhysicalAction>();
// 		return empty; // because returning reference is faster
// 	}
// }
// 
// const shared_ptr<PhysicalAction>& PhysicalActionVectorVector::operator[](unsigned int id) const
// {
// 	if(	   currentActionType >=0
// 		&& (unsigned int)currentActionType < actionParameters[id].size() )
// 		return actionParameters[id][currentActionType];
// 	else
// 	{
// 		empty = shared_ptr<PhysicalAction>();
// 		return empty;
// 	}
// }
// 
// void PhysicalActionVectorVector::gotoFirst()
// {
// 	vvi    = actionParameters.begin();
// 	vviEnd = actionParameters.end();
// 	currentIndex=0;
// 
// 	if (vvi!=vviEnd)
// 	{
// 		vi    = (*vvi).begin();
// 		viEnd = (*vvi).end();
// 	
// 		while( vi == viEnd )
// 		{
// 			++vvi;
// 			++currentIndex;
// 			if(vvi != vviEnd)
// 			{
// 				vi	= (*vvi).begin();
// 				viEnd	= (*vvi).end();
// 			}
// 			else
// 				return;
// 		}
// 		if (!(*vi))
// 			gotoNext();
// 	}
// }
// 
// bool PhysicalActionVectorVector::notAtEnd()
// {
// 	vector< shared_ptr<PhysicalAction> >::iterator tmpVi = vi;
// 	temporaryVvi = vvi;
// 
// 	if( vvi == vviEnd )
// 		return false;
// 
// 	while (tmpVi==viEnd)
// 	{
// 		++temporaryVvi;
// 		if (temporaryVvi==vviEnd)
// 			return false;
// 		tmpVi = (*vvi).begin();
// 		viEnd = (*vvi).end();
// 	}
// 	return true;
// }
// 
// void PhysicalActionVectorVector::gotoNext()
// {
// 	if ( vi != viEnd )
// 		++vi;
// 		
// 	while( vi == viEnd )
// 	{
// 		++vvi;		
// 		++currentIndex;
// 		if(vvi != vviEnd)
// 		{
// 			vi	= (*vvi).begin();
// 			viEnd	= (*vvi).end();
// 		}
// 		else
// 			break;
// 	}	
// 	
// 	while (!(*vi))
// 		++vi;
// 
// }
// 
// 
// 
// 
// 
// returns PhysicalAction selected by setCurrentActionType, for current Body
// shared_ptr<PhysicalAction> PhysicalActionVectorVector::getCurrent(int & id)
// {
// 	if( currentActionType < (*aii).size() )
// 		return (*aii)[currentActionType];
// 	else
// 		return shared_ptr<PhysicalAction>();
// 	id = currentIndex;
// 	return (*vi);
// }

