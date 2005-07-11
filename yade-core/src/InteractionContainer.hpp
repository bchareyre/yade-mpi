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

#ifndef __INTERACTIONCONTAINER_HPP__
#define __INTERACTIONCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-serialization/Serializable.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "InteractionContainerIteratorPointer.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Interaction;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class InteractionContainer : public Serializable
{
	public    : InteractionContainer() { interaction.clear(); };
	public    : virtual ~InteractionContainer() {};

	public    : virtual bool insert(unsigned int /*id1*/,unsigned int /*id2*/)				{throw;};
	public    : virtual bool insert(shared_ptr<Interaction>&)						{throw;};
	public    : virtual void clear() 									{throw;};
	public    : virtual bool erase(unsigned int /*id1*/,unsigned int /*id2*/) 				{throw;};

	public    : virtual const shared_ptr<Interaction>& find(unsigned int /*id1*/,unsigned int /*id2*/) 		{throw;};

	// looping over the data
//	public    : virtual void gotoFirst() 									{throw;};
//	public    : virtual bool notAtEnd() 									{throw;};
//	public    : virtual void gotoNext() 									{throw;};

// BEGIN - delete that! we shouldn't check isReal flag inside contaier, because we will make iterators
//	public    : virtual void gotoFirstPotential() 								{throw;};
//	public    : virtual void gotoNextPotential() 								{throw;};
//	public    : virtual bool notAtEndPotential() 								{throw;};
// END



	public    : typedef InteractionContainerIteratorPointer iterator;
        public    : virtual InteractionContainer::iterator begin()			{throw;};
        public    : virtual InteractionContainer::iterator end()			{throw;};

	
 	//public    : virtual const shared_ptr<Interaction>& getCurrent() 						{throw;};

	// deletes currently pointed element, and goes to the next one.
	//public    : virtual void eraseCurrentAndGotoNext() 							{throw;};
	//public    : virtual void eraseCurrentAndGotoNextPotential()						{throw;};
	public    : virtual unsigned int size() 								{throw;};


	// serialization of this class...
	REGISTER_CLASS_NAME(InteractionContainer);

	private   : vector<shared_ptr<Interaction> > interaction;
	public    : virtual void registerAttributes();

	protected : virtual void preProcessAttributes(bool deserializing);
	protected : virtual void postProcessAttributes(bool deserializing);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(InteractionContainer,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __INTERACTIONCONTAINER_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
