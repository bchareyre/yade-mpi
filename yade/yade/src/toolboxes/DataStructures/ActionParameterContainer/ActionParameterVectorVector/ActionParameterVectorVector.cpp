#include "ActionParameterVectorVector.hpp"
#include "Body.hpp"
#include "ActionParameter.hpp"

ActionParameterVectorVector::ActionParameterVectorVector()
{
	clear();
	currentIndex = -1;
}

ActionParameterVectorVector::~ActionParameterVectorVector()
{
}

void ActionParameterVectorVector::clear()
{
	actions.clear();
	usedIds.clear();
}

// doesn't not delete all, just resets data
void ActionParameterVectorVector::reset()
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

unsigned int ActionParameterVectorVector::size()
{
	return actions.size();
}

// fills container with resetted fields. argument here, should be all ActionParameter types that are planned to use
void ActionParameterVectorVector::prepare(std::vector<shared_ptr<ActionParameter> >& actionTypes)
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
shared_ptr<ActionParameter>& ActionParameterVectorVector::find(unsigned int id , int actionIndex )
{
	if( actions.size() <= id ) // this is very rarely executed, only at beginning.
	// somebody is accesing out of bounds, make sure he will find, what he needs - a resetted ActionParameter of his type
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
void ActionParameterVectorVector::gotoFirst()
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

bool ActionParameterVectorVector::notAtEnd()
{
	return vvi != vviEnd;
}

void ActionParameterVectorVector::gotoNext()
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

shared_ptr<ActionParameter>& ActionParameterVectorVector::getCurrent(int& id)
{
	id = currentIndex;
	return (*vi);
}















// OLD SLOWER container

//
// 
// ActionParameterVectorVector::ActionParameterVectorVector()
// {
// 	clear();
// 	currentActionType = -1;
// 	empty = shared_ptr<ActionParameter>();
// 	currentIndex = -1;
// }
// 
// ActionParameterVectorVector::~ActionParameterVectorVector()
// {
// }
// 
// 
// void ActionParameterVectorVector::clear()
// {
// 	actions.clear();
// }
// 
// void ActionParameterVectorVector::reset()
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
// unsigned int ActionParameterVectorVector::size()
// {
// 	return actions.size();
// }
// 	
// adds ActionParameter acting on one body,
// it is mathematically added if ActionParameter of this polymorphic type already exists,
// if it doesn't exist, then it is appended to stored list of Actions for that body
// void ActionParameterVectorVector::add(const shared_ptr<ActionParameter>& newAction, unsigned int id)
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
// adds ActionParameter that acts on two bodies.
// on first body it is substarcted,
// to second body it is added
// void ActionParameterVectorVector::add(const shared_ptr<ActionParameter>&, unsigned int , unsigned int)
// {
// 
// }
// 	
// allows to set current polymorphic ActionParameter Type on which other functions will work:
// function that use this are: eraseAction, operator[]
// void ActionParameterVectorVector::setCurrentActionType(int idx)
// {
// 	currentActionType = idx;
// }
// 		
// deletes ActionParameter of given polymorphic type from body that has given Id
// bool ActionParameterVectorVector::eraseAction(unsigned int id, int idx)
// {
// 	if( idx >= 0 && (unsigned int)idx < actions[id].size() && actions[id][idx] )
// 	{
// 		actions[id][idx] = shared_ptr<ActionParameter>();
// 		return true;
// 	}
// 	else
// 		return false;
// }
// 
// deletes ActionParameter of given polymorphic type from body that has given Id,
// the polymorphic type is selected by setCurrentActionType()
// returns true if action existed before deletion
// bool ActionParameterVectorVector::eraseAction(unsigned int id)
// {
// 	if(	   currentActionType >= 0 
// 		&& (unsigned int)currentActionType < actions[id].size()
// 		&& actions[id][currentActionType] )
// 	{
// 		actions[id][currentActionType] = shared_ptr<ActionParameter>();
// 		return true;
// 	}
// 	else
// 		return false;
// }
// 
// deletes all Actions in a body of given Id
// void ActionParameterVectorVector::erase(unsigned int id)
// {
// 	actions[id].clear();
// }
// 	
// finds and returns action of given polymorphic type, for body of given Id,
// returns empty shared_ptr if this ActionParameter doesn't exist for chosen body
// shared_ptr<ActionParameter> ActionParameterVectorVector::find(unsigned int id, int idx) const
// {
// 	if( 	   idx >= 0
// 		&& (unsigned int)idx < actions[id].size() )
// 		return actions[id][idx];
// 	else
// 		return shared_ptr<ActionParameter>();
// }
// 
// same as above, polymorphic ActionParameter type is selected with setCurrentActionType
// shared_ptr<ActionParameter>& ActionParameterVectorVector::operator[](unsigned int id)
// {
// 	if(	   currentActionType >=0
// 		&& (unsigned int)currentActionType < actions[id].size() )
// 		return actions[id][currentActionType];
// 	else
// 	{
// 		empty = shared_ptr<ActionParameter>();
// 		return empty; // because returning reference is faster
// 	}
// }
// 
// const shared_ptr<ActionParameter>& ActionParameterVectorVector::operator[](unsigned int id) const
// {
// 	if(	   currentActionType >=0
// 		&& (unsigned int)currentActionType < actions[id].size() )
// 		return actions[id][currentActionType];
// 	else
// 	{
// 		empty = shared_ptr<ActionParameter>();
// 		return empty;
// 	}
// }
// 
// void ActionParameterVectorVector::gotoFirst()
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
// bool ActionParameterVectorVector::notAtEnd()
// {
// 	vector< shared_ptr<ActionParameter> >::iterator tmpVi = vi;
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
// void ActionParameterVectorVector::gotoNext()
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
// returns ActionParameter selected by setCurrentActionType, for current Body
// shared_ptr<ActionParameter> ActionParameterVectorVector::getCurrent(int & id)
// {
// 	if( currentActionType < (*aii).size() )
// 		return (*aii)[currentActionType];
// 	else
// 		return shared_ptr<ActionParameter>();
// 	id = currentIndex;
// 	return (*vi);
// }

