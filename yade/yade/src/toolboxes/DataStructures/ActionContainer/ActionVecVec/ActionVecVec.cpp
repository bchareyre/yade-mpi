#include "ActionVecVec.hpp"
#include "Body.hpp"
#include "Action.hpp"

ActionVecVec::ActionVecVec()
{
	clear();
	currentActionType = -1;
	empty = shared_ptr<Action>();
	currentIndex = -1;
}

ActionVecVec::~ActionVecVec()
{
}


void ActionVecVec::clear()
{
	actions.clear();
}

unsigned int ActionVecVec::size()
{
	return actions.size();
}
	
// adds Action acting on one body,
// it is mathematically added if Action of this polymorphic type already exists,
// if it doesn't exist, then it is appended to stored list of Actions for that body
void ActionVecVec::add(const shared_ptr<Action>& newAction, unsigned int id)
{ 
	const int& idx = newAction->getClassIndex();
	
	assert(idx >= 0);

	if( actions.size() <= id )
		actions.resize(id+1);
		
	if( actions[id].size() <= (unsigned int)idx )
		actions[id].resize(idx+1);
	
	if( actions[id][idx] )
		actions[id][idx]->add(newAction);
	else
		actions[id][idx]=newAction->clone();
}

// adds Action that acts on two bodies.
// on first body it is substarcted,
// to second body it is added
void ActionVecVec::add(const shared_ptr<Action>&, unsigned int , unsigned int)
{

}
	
// allows to set current polymorphic Action Type on which other functions will work:
// function that use this are: eraseAction, operator[]
void ActionVecVec::setCurrentActionType(int idx)
{
	currentActionType = idx;
}
		
// deletes Action of given polymorphic type from body that has given Id
bool ActionVecVec::eraseAction(unsigned int id, int idx)
{
	if( idx >= 0 && (unsigned int)idx < actions[id].size() && actions[id][idx] )
	{
		actions[id][idx] = shared_ptr<Action>();
		return true;
	}
	else
		return false;
}

// deletes Action of given polymorphic type from body that has given Id,
// the polymorphic type is selected by setCurrentActionType()
// returns true if action existed before deletion
bool ActionVecVec::eraseAction(unsigned int id)
{
	if(	   currentActionType >= 0 
		&& (unsigned int)currentActionType < actions[id].size()
		&& actions[id][currentActionType] )
	{
		actions[id][currentActionType] = shared_ptr<Action>();
		return true;
	}
	else
		return false;
}

// deletes all Actions in a body of given Id
void ActionVecVec::erase(unsigned int id)
{
	actions[id].clear();
}
	
// finds and returns action of given polymorphic type, for body of given Id,
// returns empty shared_ptr if this Action doesn't exist for chosen body
shared_ptr<Action> ActionVecVec::find(unsigned int id, int idx) const
{
	if( 	   idx >= 0
		&& (unsigned int)idx < actions[id].size() )
		return actions[id][idx];
	else
		return shared_ptr<Action>();
}

// same as above, polymorphic Action type is selected with setCurrentActionType
shared_ptr<Action>& ActionVecVec::operator[](unsigned int id)
{
	if(	   currentActionType >=0
		&& (unsigned int)currentActionType < actions[id].size() )
		return actions[id][currentActionType];
	else
	{
		empty = shared_ptr<Action>();
		return empty; // because returning reference is faster
	}
}

const shared_ptr<Action>& ActionVecVec::operator[](unsigned int id) const
{
	if(	   currentActionType >=0
		&& (unsigned int)currentActionType < actions[id].size() )
		return actions[id][currentActionType];
	else
	{
		empty = shared_ptr<Action>();
		return empty;
	}
}

void ActionVecVec::gotoFirst()
{
	vvi    = actions.begin();
	vviEnd = actions.end();
	currentIndex=0;

	if (vvi!=vviEnd)
	{
		vi    = (*vvi).begin();
		viEnd = (*vvi).end();
	
		while( vi == viEnd )
		{
			++vvi;
			++currentIndex;
			if(vvi != vviEnd)
			{
				vi	= (*vvi).begin();
				viEnd	= (*vvi).end();
			}
			else
				return;
		}
		if (!(*vi))
			gotoNext();
	}
}

bool ActionVecVec::notAtEnd()
{
	vector< shared_ptr<Action> >::iterator tmpVi = vi;
	temporaryVvi = vvi;

	if( vvi == vviEnd )
		return false;

	while (tmpVi==viEnd)
	{
		++temporaryVvi;
		if (temporaryVvi==vviEnd)
			return false;
		tmpVi = (*vvi).begin();
		viEnd = (*vvi).end();
	}
	return true;
}

void ActionVecVec::gotoNext()
{
	if ( vi != viEnd )
		++vi;
		
	while( vi == viEnd )
	{
		++vvi;		
		++currentIndex;
		if(vvi != vviEnd)
		{
			vi	= (*vvi).begin();
			viEnd	= (*vvi).end();
		}
		else
			break;
	}	
	
	while (!(*vi))
		++vi;

}





// returns Action selected by setCurrentActionType, for current Body
shared_ptr<Action> ActionVecVec::getCurrent(int & id)
{
// 	if( currentActionType < (*aii).size() )
// 		return (*aii)[currentActionType];
// 	else
// 		return shared_ptr<Action>();
	id = currentIndex;
	return (*vi);
}
















/*
unsigned int ActionVecVec::insert(shared_ptr<Body>& b)
{
	unsigned int position = b->getId();

	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(position);
	if( tmpBii != bodies.end() )
	{
		unsigned int newPosition = position;
		// finds the first free key, which is bigger than id.
		while( bodies.find(newPosition) != bodies.end() )
			++newPosition;
		//cerr << "WARNING: body id=\"" << position << "\" is already used. Using first free id=\"" << newPosition << "\", beware - if you are loading a file, this will break interactions for this body!\n";
		position = newPosition;
	}
	BodyContainer::setId(b,position);
	bodies[position]=b;
	return position;
}

unsigned int ActionVecVec::insert(shared_ptr<Body>& b, unsigned int newId)
{
	BodyContainer::setId(b,newId);
	return insert(b);
}


bool ActionVecVec::erase(unsigned int id)
{
	bii = bodies.find(id);

	if( bii != bodies.end() )
	{
		bodies.erase(bii);
		return true;
	}
	else
		return false;
}

bool ActionVecVec::find(unsigned int id , shared_ptr<Body>& b) const
{
	// do not modify the interanl iterator
	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(id);

	if (tmpBii != bodies.end())
	{
		b = (*tmpBii).second;
		return true;
	}
	else
		return false;
}


*/

