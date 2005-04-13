#ifndef __ACTION_VEC_VEC_HPP__
#define __ACTION_VEC_VEC_HPP__

#include "ActionParameterContainer.hpp"
#include <list>
#include <vector>

class ActionParameter;

using namespace boost;
using namespace std;

// NEW CONTAINER version - faster

class ActionParameterVectorVector : public ActionParameterContainer
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
	//	- second dimension is ActionParameter->getClassIndex() number
	private	  : vector< vector< shared_ptr<ActionParameter> > > actions;
	private	  :         vector< shared_ptr<ActionParameter> >   actionTypesResetted;
	private	  : vector< bool > usedIds;
	
	private	  : vector< vector< shared_ptr<ActionParameter> > >::iterator vvi;
	private	  : vector< vector< shared_ptr<ActionParameter> > >::iterator vviEnd;
	private	  :         vector< shared_ptr<ActionParameter> >  ::iterator vi;
	private	  :         vector< shared_ptr<ActionParameter> >  ::iterator viEnd;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public	  : ActionParameterVectorVector();
	public	  : virtual ~ActionParameterVectorVector();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public    : virtual void clear();
	// doesn't not delete all, just resets data
	public    : virtual void reset();
	public    : virtual unsigned int size();
	// fills container with resetted fields. argument here, should be all ActionParameter types that are planned to use
	public    : virtual void prepare(std::vector<shared_ptr<ActionParameter> >& );
	
	// finds and returns action of given polymorphic type, for body of given Id,
	// should be always succesfull. if it is not - you forgot to call prepare()
	public    : virtual shared_ptr<ActionParameter>& find(
					  unsigned int /*Body->getId() */
					, int /*ActionForce::getClassIndexStatic()*/);

	// looping over Bodies, and their Actions
	public    : virtual void gotoFirst();
	public    : virtual bool notAtEnd();
	public    : virtual void gotoNext();
	public    : virtual shared_ptr<ActionParameter>& getCurrent(int& );

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(ActionParameterVectorVector);
};

REGISTER_SERIALIZABLE(ActionParameterVectorVector,false);


//
// class ActionParameterVectorVector : public ActionParameterContainer
// {
// 
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// /// Internal data										///
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// 	
// 	
// 	private : int currentIndex;
// 	
// 	// this in fact should be also a RedirectionVector in respect to the Body.id
// 	// this container is memory-consuming, because size of this vector is depending on highest id
// 	// from all bodies, not on the number of bodies
// 	
// 	// in this two-dimensional table:
// 	// 	- first  dimension is Body->getId() number
// 	//	- second dimension is ActionParameter->getClassIndex() number
// 	private	  : vector< vector< shared_ptr<ActionParameter> > > actions;
// 	private	  : vector< vector< shared_ptr<ActionParameter> > >::iterator vvi;
// 	private	  : vector< vector< shared_ptr<ActionParameter> > >::iterator temporaryVvi;
// 	private	  : vector< vector< shared_ptr<ActionParameter> > >::iterator vviEnd;
// 	private	  : vector< shared_ptr<ActionParameter> >::iterator vi;
// 	private	  : vector< shared_ptr<ActionParameter> >::iterator viEnd;
// 	//private	  : list< vector< vector< shared_ptr<ActionParameter> > >::iterator > iteratorList;
// 	
// 	private   : int currentActionType; // current polymorphic ActionParameter type - is an ActionParameter->getClassIndex();
// 	private	  : mutable shared_ptr<ActionParameter> empty;
// 
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// /// Constructor/Destructor									///
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 	public	  : ActionParameterVectorVector();
// 	public	  : virtual ~ActionParameterVectorVector();
// 
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// /// Methods											///
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 	public    : virtual void clear();
// 	public    : virtual void reset();
// 	public    : virtual unsigned int size();
// 	
// 	// adds ActionParameter acting on one body,
// 	// it is mathematically added if ActionParameter of this polymorphic type already exists,
// 	// if it doesn't exist, then it is appended to stored list of Actions for that body
// 	public	  : virtual void add(const shared_ptr<ActionParameter>&, unsigned int );
// 	// adds ActionParameter that acts on two bodies.
// 	// on first body it is substarcted,
// 	// to second body it is added
// 	public	  : virtual void add(const shared_ptr<ActionParameter>&, unsigned int , unsigned int);
// 	
// 	// allows to set current polymorphic ActionParameter Type on which other functions will work:
// 	// function that use this are: eraseAction, operator[]
// 	public	  : virtual void setCurrentActionType( int /*ActionParameter::getClassIndex()*/ );
// 		
// 	// deletes ActionParameter of given polymorphic type from body that has given Id
// 	public    : virtual bool eraseAction(	  unsigned int /*Body->getId() */
// 						, int /*ActionParameter::getClassIndex()*/);
// 	// deletes ActionParameter of given polymorphic type from body that has given Id,
// 	// the polymorphic type is selected by setCurrentActionType()
// 	// returns true if action existed before deletion
// 	public    : virtual bool eraseAction(unsigned int);
// 	// deletes all Actions in a body of given Id
// 	public    : virtual void erase(unsigned int);
// 	
// 	// finds and returns action of given polymorphic type, for body of given Id,
// 	// returns empty shared_ptr if this ActionParameter doesn't exist for chosen body
// 	public    : virtual shared_ptr<ActionParameter> find(
// 					  unsigned int /*Body->getId() */
// 					, int /*ActionParameter::getClassIndex()*/) const;
// 	
// 	// same as above, polymorphic ActionParameter type is selected with setCurrentActionType
// 	public    : virtual shared_ptr<ActionParameter>& operator[](unsigned int);
// 	public    : virtual const shared_ptr<ActionParameter>& operator[](unsigned int) const;
// 
// 	// looping over Bodies, and their Actions (with setCurrentActionType)
// 	public    : virtual void gotoFirst();
// 	public    : virtual bool notAtEnd();
// 	public    : virtual void gotoNext();
// 	public    : virtual shared_ptr<ActionParameter> getCurrent(int & id);
// 	
// 
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// /// Serialization										///
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// 	
// 	REGISTER_CLASS_NAME(ActionParameterVectorVector);
// };
// 
// REGISTER_SERIALIZABLE(ActionParameterVectorVector,false);
//
#endif // __ACTION_VEC_VEC_HPP__
