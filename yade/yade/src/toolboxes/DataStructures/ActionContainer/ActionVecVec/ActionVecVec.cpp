#include "ActionVecVec.hpp"
#include "Body.hpp"


ActionVecVec::ActionVecVec()
{
	clear();
}

ActionVecVec::~ActionVecVec()
{
}


void ActionVecVec::clear()
{
}

unsigned int ActionVecVec::size()
{
}
	
// adds Action acting on one body,
// it is mathematically added if Action of this polymorphic type already exists,
// if it doesn't exist, then it is appended to stored list of Actions for that body
void ActionVecVec::add(shared_ptr<Action>&, unsigned int )
{
}
// adds Action that acts on two bodies.
// on first body it is substarcted,
// to second body it is added
void ActionVecVec::add(shared_ptr<Action>&, unsigned int , unsigned int)
{
}
	
// allows to set current polymorphic Action Type on which other functions will work:
// function that use this are: eraseAction, operator[]
void ActionVecVec::setCurrentActionType(shared_ptr<Action>)
{
}
		
// deletes Action of given polymorphic type from body that has given Id
bool ActionVecVec::eraseAction(shared_ptr<Action>&, unsigned int)
{
}

// deletes Action of given polymorphic type from body that has given Id,
// the polymorphic type is selected by setCurrentActionType()
bool ActionVecVec::eraseAction(unsigned int)
{
}

// deletes all Actions in a body of given Id
bool ActionVecVec::erase(unsigned int)
{
}
	
// finds and returns action of given polymorphic type, for body of given Id,
// returns empty shared_ptr and false if this Action doesn't exists for chosen body
bool ActionVecVec::find(shared_ptr<Action>&, unsigned int) const
{
}

// same as above, polymorphic Action type is selected with setCurrentActionType
shared_ptr<Action>& ActionVecVec::operator[](unsigned int)
{
}

const shared_ptr<Action>& ActionVecVec::operator[](unsigned int) const
{
}

// looping over Bodies, and their Actions (with setCurrentActionType)
void ActionVecVec::gotoFirst()
{
}

bool ActionVecVec::notAtEnd()
{
}

void ActionVecVec::gotoNext()
{
}

// returns Action selected by setCurrentActionType, for current Body
shared_ptr<Action> ActionVecVec::getCurrent()
{
}

void ActionVecVec::pushIterator()
{
}

void ActionVecVec::popIterator()
{
}
















/*
unsigned int ActionVecVec::insert(shared_ptr<Body>& b)
{
//	unsigned int max;
//	if( bodies.begin() != bodies.end() )
//		max = (--(bodies.end()))->first + 1;
//	else
//		max = 0;
//	bodies.insert( Loki::AssocVector::value_type( max , b ));
//	bodies[max]=b;
//	BodyContainer::setId(b,max);
//	return max;

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

void ActionVecVec::clear()
{
	bodies.clear();
}

bool ActionVecVec::erase(unsigned int id)
{

// WARNING!!! AssocVector.erase() invalidates all iterators !!!

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

shared_ptr<Body>& ActionVecVec::operator[](unsigned int id)
{
	// do not modify bii iterator
	temporaryBii = bodies.find(id);
//	if (bii != bodies.end())
		return (*temporaryBii).second;
//	else
//		return shared_ptr<Body>();
}

const shared_ptr<Body>& ActionVecVec::operator[](unsigned int id) const
{
	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(id);
// when commented it is faster, but less secure.
//	if (tmpBii != bodies.end())
		return (*tmpBii).second;
//	else
//		return shared_ptr<Body>();
}

void ActionVecVec::gotoFirst()
{
	bii    = bodies.begin();
	biiEnd = bodies.end();
}

bool ActionVecVec::notAtEnd()
{
	return ( bii != biiEnd );
}

void ActionVecVec::gotoNext()
{
	++bii;
}

shared_ptr<Body> ActionVecVec::getCurrent()
{
	return (*bii).second;
}

void ActionVecVec::pushIterator()
{// FIXME - make sure that this is FIFO (I'm tired now...)
	iteratorList.push_front(bii);
}

void ActionVecVec::popIterator()
{
	bii = iteratorList.front();
	iteratorList.pop_front();
}

unsigned int ActionVecVec::size()
{
	return bodies.size();
}
*/

