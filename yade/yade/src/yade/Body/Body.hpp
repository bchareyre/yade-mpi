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

#ifndef __BODY_HPP__
#define __BODY_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Se3.hpp"
#include "GeometricalModel.hpp"
#include "CollisionGeometry.hpp"
#include "BoundingVolume.hpp"
#include "BodyPhysicalParameters.hpp"
#include "InteractionContainer.hpp"
#include "Interaction.hpp"
#include "Serializable.hpp"
#include "Indexable.hpp"

#include "ActionContainer.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//class Actor;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all physical object.

	All the physical types (SimpleBody, ComplexBody, RigidBody ....) must derived from this class.
*/
class Body : public Serializable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	private	: unsigned int id;
	public	: unsigned int getId() {return id;};
	friend class BodyContainer; // only BodyContainer can set the id of a body

	public : bool containSubBodies;
	
	// int group;
		
	// FIXME : where to put gm,cm and bv : do body need them or only SimpleBody ??
	/*! The geometrical model of this body (polyhedron, box ...) */
	public : shared_ptr<GeometricalModel> gm; // geometricalModel
	/*! The collision model of this body (sphere hierarchy, box ...) */
	public : shared_ptr<CollisionGeometry> cm; // interactionGeometry
	/*! The bounding volume of this body (AABB, K-Dop ...) */
	public : shared_ptr<BoundingVolume> bv; // boundingVolume
	/*! Mechanical parameters of the body (mass, sitffness ...) */
	public : shared_ptr<BodyPhysicalParameters> physicalParameters;


	// FIXME : should be determined automatically or not ?? if the body has a subscription to a kinematic engine then it is not dynamic but maybe a body with no subscription can be not dynamic ??
	/*! isDynamic is true if the state of the body is not modified by a kinematicEngine. It is useful
	for example for collision detection : if two colliding bodies are only kinematic then it is useless to
	modelise their contact */
	public : bool isDynamic;
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : Body ();
	public : Body (unsigned int newId);

	/*! Destructor */
	public : virtual ~Body ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*! If computations on the attributes are needed after serialization. For example you may want to serialize the name of a file that contains geometrical data and postProcessAttributes(bool deserializing) will load the file */

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(Body);
	protected : virtual void postProcessAttributes(bool deserializing);
	/*! Tells the IOManager which attributes should be serialized */
	public : void registerAttributes();

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Body,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODY_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

