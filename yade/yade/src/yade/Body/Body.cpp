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

#include "Body.hpp"
#include "InteractionVecSet.hpp"
#include "InteractionHashMap.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// we must initialize id = 0, otherwise BodyContainer will crash.
// FIXME - where to create interactions? here, or maybe somewhere else - who decides which type to use by default?
Body::Body () : Serializable() , id(0) , interactions(shared_ptr<InteractionContainer>(new InteractionHashMap))
{
}

Body::Body (unsigned int newId) : Serializable() , id(newId) , interactions(shared_ptr<InteractionContainer>(new InteractionHashMap))
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Body::~Body()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Body::postProcessAttributes(bool)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void Body::registerAttributes()
{
	REGISTER_ATTRIBUTE(id);
	REGISTER_ATTRIBUTE(isDynamic); // FIXME : only used for collision : no collision between 2 non dynamic objects
	REGISTER_ATTRIBUTE(velocity);
	REGISTER_ATTRIBUTE(angularVelocity);
	REGISTER_ATTRIBUTE(se3);
	REGISTER_ATTRIBUTE(bv);
	REGISTER_ATTRIBUTE(cm);
	REGISTER_ATTRIBUTE(gm);
	REGISTER_ATTRIBUTE(actors);
// FIXME ? - must be registered or not? how about saving state during the simulation?
	//REGISTER_ATTRIBUTE(interactions);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
