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

#ifndef __INTERACTION_H__
#define __INTERACTION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Serializable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "InteractionGeometry.hpp"
#include "InteractionPhysics.hpp"


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Interaction : public Serializable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	// FIXME : better to test if InteractionPhysics==0 and remove this flag
	public	: bool isNew;	
	public  : bool isReal;
	private	: unsigned int id1,id2;


	// FIXME - why public ?!
	public	: shared_ptr<GeometryOfInteraction> interactionGeometry;
	public	: shared_ptr<PhysicsOfInteraction> interactionPhysics;

	// construction
	public	: Interaction ();
	public	: Interaction(unsigned int newId1,unsigned int newId2);
	//public: Interaction(std::vector<unsigned int> ids_) {/*....*/ }; // for multiple interactions....
	public	: ~Interaction ();

	public	: unsigned int getId1() {return id1;}; // everything written inside class declaration is inline, therefore as fast as accessing the variable itself, every compiler guarantees this.
	public	: unsigned int getId2() {return id2;};

	protected : virtual void postProcessAttributes(bool deserializing);
	public	: void registerAttributes();

	REGISTER_CLASS_NAME(Interaction);

};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Interaction,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __INTERACTION_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////






