#include "ActionVecVec.hpp"
#include "Body.hpp"
#include "Action.hpp"

ActionVecVec::ActionVecVec()
{
	clear();
	currentIndex = -1;
}

ActionVecVec::~ActionVecVec()
{
}

void ActionVecVec::clear()
{
	actions.clear();
	usedIds.clear();
}

// doesn't not delete all, just resets data
void ActionVecVec::reset()
{
	vvi    = actions.begin();
	vviEnd = actions.end();
	for( ; vvi != vviEnd ; ++vvi )
	{
		vi    = (*vvi).begin();
		viEnd = (*vvi).end();
		for( ; vi != viEnd ; ++vi)
		//if(*vi) // FIXME ?? do not check - all fields are NOT empty.
			(*vi)->reset();
	}
}

unsigned int ActionVecVec::size()
{
	return actions.size();
}

// fills container with resetted fields. argument here, should be all Action types that are planned to use
void ActionVecVec::prepare(std::vector<shared_ptr<Action> >& actionTypes)
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
shared_ptr<Action>& ActionVecVec::find(unsigned int id , int actionIndex )
{
	if( actions.size() <= id ) // this is very rarely executed, only at beginning.
	// somebody is accesing out of bounds, make sure he will find, what he needs - a resetted Action of his type
	{
		unsigned int oldSize = actions.size();
		unsigned int newSize = id+1;
		usedIds.resize(newSize,false);
		actions.resize(newSize);
		for(unsigned int i = oldSize ; i < newSize ; ++i )
		{
			unsigned int actionTypesSize = actionTypesResetted.size();
			actions[i].resize(actionTypesSize);
			for( unsigned int j = 0 ; j < actionTypesSize ; ++j )
				actions[i][j] = actionTypesResetted[j]->clone();
		}
	}
	usedIds[id] = true;
	return actions[id][actionIndex];
}

// looping over Bodies, and their Actions
void ActionVecVec::gotoFirst()
{
	currentIndex = 0;
	vvi    = actions.begin();
	vviEnd = actions.end();
	
	while( (! usedIds[currentIndex]) && (vvi != vviEnd) )
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

bool ActionVecVec::notAtEnd()
{
	return vvi != vviEnd;
}

void ActionVecVec::gotoNext()
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

shared_ptr<Action>& ActionVecVec::getCurrent(int& id)
{
	id = currentIndex;
	return (*vi);
}















// OLD SLOWER container

//
// 
// ActionVecVec::ActionVecVec()
// {
// 	clear();
// 	currentActionType = -1;
// 	empty = shared_ptr<Action>();
// 	currentIndex = -1;
// }
// 
// ActionVecVec::~ActionVecVec()
// {
// }
// 
// 
// void ActionVecVec::clear()
// {
// 	actions.clear();
// }
// 
// void ActionVecVec::reset()
// {
// 	vvi    = actions.begin();
// 	vviEnd = actions.end();
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
// unsigned int ActionVecVec::size()
// {
// 	return actions.size();
// }
// 	
// adds Action acting on one body,
// it is mathematically added if Action of this polymorphic type already exists,
// if it doesn't exist, then it is appended to stored list of Actions for that body
// void ActionVecVec::add(const shared_ptr<Action>& newAction, unsigned int id)
// { 
// 	const int& idx = newAction->getClassIndex();
// 	
// 	assert(idx >= 0);
// 
// 	if( actions.size() <= id )
// 		actions.resize(id+1);
// 		
// 	if( actions[id].size() <= (unsigned int)idx )
// 		actions[id].resize(idx+1);
// 	
// 	if( actions[id][idx] )
// 		actions[id][idx]->add(newAction);
// 	else
// 		actions[id][idx]=newAction->clone();
// }
// 
// adds Action that acts on two bodies.
// on first body it is substarcted,
// to second body it is added
// void ActionVecVec::add(const shared_ptr<Action>&, unsigned int , unsigned int)
// {
// 
// }
// 	
// allows to set current polymorphic Action Type on which other functions will work:
// function that use this are: eraseAction, operator[]
// void ActionVecVec::setCurrentActionType(int idx)
// {
// 	currentActionType = idx;
// }
// 		
// deletes Action of given polymorphic type from body that has given Id
// bool ActionVecVec::eraseAction(unsigned int id, int idx)
// {
// 	if( idx >= 0 && (unsigned int)idx < actions[id].size() && actions[id][idx] )
// 	{
// 		actions[id][idx] = shared_ptr<Action>();
// 		return true;
// 	}
// 	else
// 		return false;
// }
// 
// deletes Action of given polymorphic type from body that has given Id,
// the polymorphic type is selected by setCurrentActionType()
// returns true if action existed before deletion
// bool ActionVecVec::eraseAction(unsigned int id)
// {
// 	if(	   currentActionType >= 0 
// 		&& (unsigned int)currentActionType < actions[id].size()
// 		&& actions[id][currentActionType] )
// 	{
// 		actions[id][currentActionType] = shared_ptr<Action>();
// 		return true;
// 	}
// 	else
// 		return false;
// }
// 
// deletes all Actions in a body of given Id
// void ActionVecVec::erase(unsigned int id)
// {
// 	actions[id].clear();
// }
// 	
// finds and returns action of given polymorphic type, for body of given Id,
// returns empty shared_ptr if this Action doesn't exist for chosen body
// shared_ptr<Action> ActionVecVec::find(unsigned int id, int idx) const
// {
// 	if( 	   idx >= 0
// 		&& (unsigned int)idx < actions[id].size() )
// 		return actions[id][idx];
// 	else
// 		return shared_ptr<Action>();
// }
// 
// same as above, polymorphic Action type is selected with setCurrentActionType
// shared_ptr<Action>& ActionVecVec::operator[](unsigned int id)
// {
// 	if(	   currentActionType >=0
// 		&& (unsigned int)currentActionType < actions[id].size() )
// 		return actions[id][currentActionType];
// 	else
// 	{
// 		empty = shared_ptr<Action>();
// 		return empty; // because returning reference is faster
// 	}
// }
// 
// const shared_ptr<Action>& ActionVecVec::operator[](unsigned int id) const
// {
// 	if(	   currentActionType >=0
// 		&& (unsigned int)currentActionType < actions[id].size() )
// 		return actions[id][currentActionType];
// 	else
// 	{
// 		empty = shared_ptr<Action>();
// 		return empty;
// 	}
// }
// 
// void ActionVecVec::gotoFirst()
// {
// 	vvi    = actions.begin();
// 	vviEnd = actions.end();
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
// bool ActionVecVec::notAtEnd()
// {
// 	vector< shared_ptr<Action> >::iterator tmpVi = vi;
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
// void ActionVecVec::gotoNext()
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
// returns Action selected by setCurrentActionType, for current Body
// shared_ptr<Action> ActionVecVec::getCurrent(int & id)
// {
// 	if( currentActionType < (*aii).size() )
// 		return (*aii)[currentActionType];
// 	else
// 		return shared_ptr<Action>();
// 	id = currentIndex;
// 	return (*vi);
// }

