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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ACTIONCONTAINER_HPP__
#define __ACTIONCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Action;

class ActionContainer : public Serializable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

// DECLARE YOU ATTRIBUTES HERE

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : ActionContainer();

	/*! Destructor */
	public : virtual ~ActionContainer();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	public    : virtual void clear() 						{throw;};
	// doen't not delete all, just resets data
	public    : virtual void reset() 						{throw;};
	public    : virtual unsigned int size() 					{throw;};
	
	// adds Action acting on one body,
	// it is mathematically added if Action of this polymorphic type already exists,
	// if it doesn't exist, then it is appended to stored list of Actions for that body
	public	  : virtual void add(const shared_ptr<Action>&, unsigned int )		{throw;};
	// adds Action that acts on two bodies.
	// on first body it is substarcted,
	// to second body it is added
	public	  : virtual void add(const shared_ptr<Action>&, unsigned int , unsigned int)	{throw;};
	
	// allows to set current polymorphic Action Type on which other functions will work:
	// function that use this are: eraseAction, operator[], getCurrent()
	public	  : virtual void setCurrentActionType( int /*Action::getClassIndex()*/ ){throw;};
		
	// deletes Action of given polymorphic type from body that has given Id
	public    : virtual bool eraseAction(	  unsigned int /* Body->getId() */
						, int /* Action::getClassIndex() */)	{throw;};
						
	// deletes Action of given polymorphic type from body that has given Id,
	// the polymorphic type is selected by setCurrentActionType()
	// returns true if action existed before deletion
	public    : virtual bool eraseAction(unsigned int) 				{throw;};
	// deletes all Actions in a body of given Id
	public    : virtual void erase(unsigned int) 					{throw;};
	
	// finds and returns action of given polymorphic type, for body of given Id,
	// returns empty shared_ptr if this Action doesn't exist for chosen body
	public    : virtual shared_ptr<Action> find(
					  unsigned int /*Body->getId() */
					, int /*Action::getClassIndex()*/)		const{throw;};
	// same as above, polymorphic Action type is selected with setCurrentActionType
	public    : virtual shared_ptr<Action>& operator[](unsigned int)		{throw;};
	public    : virtual const shared_ptr<Action>& operator[](unsigned int) const	{throw;};

	// looping over Bodies, and their Actions (with setCurrentActionType)
	public    : virtual void gotoFirst() 						{throw;};
	public    : virtual bool notAtEnd() 						{throw;};
	public    : virtual void gotoNext() 						{throw;};

	public    : virtual shared_ptr<Action> getCurrent(int& )			{throw;};
	
	public    : virtual void pushIterator()			 			{throw;};
	public    : virtual void popIterator()			 			{throw;};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(ActionContainer);
	// local storage for uniform serialization of all possible container concrete implementations.
	private   : vector<shared_ptr<Action> > action;
	public    : virtual void registerAttributes();
	//protected : virtual void preProcessAttributes(bool deserializing);
	//protected : virtual void postProcessAttributes(bool deserializing);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(ActionContainer,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __ACTIONCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

