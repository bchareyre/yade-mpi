#ifndef __ACTION_VEC_VEC_HPP__
#define __ACTION_VEC_VEC_HPP__

#include "ActionContainer.hpp"
#include <list>
#include <vector>

class Action;

using namespace boost;

class ActionVecVec : public ActionContainer
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Internal data										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	// this in fact should be also a RedirectionVector in respect to the Body.id
	// this container is memory-consuming, because size of this vector is depending on highest id
	// from all bodies, not on the number of bodies
	private	: std::vector< std::vector< shared_ptr<Action> > > actions;
	private	: std::vector< std::vector< shared_ptr<Action> > >::iterator aii;
	private	: std::vector< std::vector< shared_ptr<Action> > >::iterator temporaryAii;
	private	: std::vector< std::vector< shared_ptr<Action> > >::iterator aiiEnd;
	private	: std::list< std::vector< std::vector< shared_ptr<Action> > >::iterator > iteratorList;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public	: ActionVecVec();
	public	: virtual ~ActionVecVec();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public    : virtual void clear();
	public    : virtual unsigned int size();
	
	// adds Action acting on one body,
	// it is mathematically added if Action of this polymorphic type already exists,
	// if it doesn't exist, then it is appended to stored list of Actions for that body
	public	  : virtual void add(shared_ptr<Action>&, unsigned int );
	// adds Action that acts on two bodies.
	// on first body it is substarcted,
	// to second body it is added
	public	  : virtual void add(shared_ptr<Action>&, unsigned int , unsigned int);
	
	// allows to set current polymorphic Action Type on which other functions will work:
	// function that use this are: eraseAction, operator[]
	public	  : virtual void setCurrentActionType(shared_ptr<Action>);
		
	// deletes Action of given polymorphic type from body that has given Id
	public    : virtual bool eraseAction(shared_ptr<Action>&, unsigned int);
	// deletes Action of given polymorphic type from body that has given Id,
	// the polymorphic type is selected by setCurrentActionType()
	public    : virtual bool eraseAction(unsigned int);
	// deletes all Actions in a body of given Id
	public    : virtual bool erase(unsigned int);
	
	// finds and returns action of given polymorphic type, for body of given Id,
	// returns empty shared_ptr and false if this Action doesn't exists for chosen body
	public    : virtual bool find(shared_ptr<Action>&, unsigned int) const;
	// same as above, polymorphic Action type is selected with setCurrentActionType
	public    : virtual shared_ptr<Action>& operator[](unsigned int);
	public    : virtual const shared_ptr<Action>& operator[](unsigned int) const;

	// looping over Bodies, and their Actions (with setCurrentActionType)
	public    : virtual void gotoFirst();
	public    : virtual bool notAtEnd();
	public    : virtual void gotoNext();
	// returns Action selected by setCurrentActionType, for current Body
	public    : virtual shared_ptr<Action> getCurrent();
	public    : virtual void pushIterator();
	public    : virtual void popIterator();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(ActionVecVec);
};

REGISTER_SERIALIZABLE(ActionVecVec,false);

#endif // __ACTION_VEC_VEC_HPP__
