#ifndef __ACTION_VEC_VEC_HPP__
#define __ACTION_VEC_VEC_HPP__

#include "ActionContainer.hpp"
#include <list>
#include <vector>

class Action;

using namespace boost;
using namespace std;

class ActionVecVec : public ActionContainer
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Internal data										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	private : int currentIndex;
	
	// this in fact should be also a RedirectionVector in respect to the Body.id
	// this container is memory-consuming, because size of this vector is depending on highest id
	// from all bodies, not on the number of bodies
	
	// in this two-dimensional table:
	// 	- first  dimension is Body->getId() number
	//	- second dimension is Action->getClassIndex() number
	private	  : vector< vector< shared_ptr<Action> > > actions;
	private	  : vector< vector< shared_ptr<Action> > >::iterator vvi;
	private	  : vector< vector< shared_ptr<Action> > >::iterator temporaryVvi;
	private	  : vector< vector< shared_ptr<Action> > >::iterator vviEnd;
	private	  : vector< shared_ptr<Action> >::iterator vi;
	private	  : vector< shared_ptr<Action> >::iterator viEnd;
	//private	  : list< vector< vector< shared_ptr<Action> > >::iterator > iteratorList;
	
	private   : int currentActionType; // current polymorphic Action type - is an Action->getClassIndex();
	private	  : mutable shared_ptr<Action> empty;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public	  : ActionVecVec();
	public	  : virtual ~ActionVecVec();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public    : virtual void clear();
	public    : virtual unsigned int size();
	
	// adds Action acting on one body,
	// it is mathematically added if Action of this polymorphic type already exists,
	// if it doesn't exist, then it is appended to stored list of Actions for that body
	public	  : virtual void add(const shared_ptr<Action>&, unsigned int );
	// adds Action that acts on two bodies.
	// on first body it is substarcted,
	// to second body it is added
	public	  : virtual void add(const shared_ptr<Action>&, unsigned int , unsigned int);
	
	// allows to set current polymorphic Action Type on which other functions will work:
	// function that use this are: eraseAction, operator[]
	public	  : virtual void setCurrentActionType( int /*Action::getClassIndex()*/ );
		
	// deletes Action of given polymorphic type from body that has given Id
	public    : virtual bool eraseAction(	  unsigned int /*Body->getId() */
						, int /*Action::getClassIndex()*/);
	// deletes Action of given polymorphic type from body that has given Id,
	// the polymorphic type is selected by setCurrentActionType()
	// returns true if action existed before deletion
	public    : virtual bool eraseAction(unsigned int);
	// deletes all Actions in a body of given Id
	public    : virtual void erase(unsigned int);
	
	// finds and returns action of given polymorphic type, for body of given Id,
	// returns empty shared_ptr if this Action doesn't exist for chosen body
	public    : virtual shared_ptr<Action> find(
					  unsigned int /*Body->getId() */
					, int /*Action::getClassIndex()*/);
	// same as above, polymorphic Action type is selected with setCurrentActionType
	public    : virtual shared_ptr<Action>& operator[](unsigned int);
	public    : virtual const shared_ptr<Action>& operator[](unsigned int) const;

	// looping over Bodies, and their Actions (with setCurrentActionType)
	public    : virtual void gotoFirst();
	public    : virtual bool notAtEnd();
	public    : virtual void gotoNext();
	public    : virtual shared_ptr<Action> getCurrent(int & id);
	

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(ActionVecVec);
};

REGISTER_SERIALIZABLE(ActionVecVec,false);

#endif // __ACTION_VEC_VEC_HPP__
