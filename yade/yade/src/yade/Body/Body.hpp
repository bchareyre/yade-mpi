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
#include "Interaction.hpp"
#include "list"
#include "Serializable.hpp"
#include "Indexable.hpp"
#include "Actor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//class Actor;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all physical object.

	All the physical types (ConnexBody, NonConnexBody, RigidBody ....) must derived from this class.
*/
class Body : public Serializable
{

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Attributes											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	
	public : vector<shared_ptr<Actor> > actors;

	// FIXME : to put into nonconnex body ??? but maybe useful for autocollision in connexbody
	public : list<shared_ptr<Interaction> > interactions;

	// FIXME : where to put gm,cm and bv : do body need them or only ConnexBody ??
	/*! The geometrical model of this body (polyhedron, box ...) */
	public : shared_ptr<GeometricalModel> gm;
	/*! The collision model of this body (sphere hierarchy, box ...) */
	public : shared_ptr<CollisionGeometry> cm;
	/*! The bounding volume of this body (AABB, K-Dop ...) */
	public : shared_ptr<BoundingVolume> bv;


	//public : shared_ptr<DynamicEngine> dynamic;

	// FIXME : should be determined automatically or not ?? if the body has a subscription to a kinematic engine then it is not dynamic but maybe a body with no subscription can be not dynamic ??
	/*! isDynamic is true if the state of the body is not modified by a kinematicEngine. It is useful
	for example for collision detection : if two colliding bodies are only kinematic then it is useless to
	modelise their contact */
	public : bool isDynamic;

	/*! The velocity of the object */
	public : Vector3r velocity;

	/*! The angular velocity of the object */
	public : Vector3r angularVelocity;

	/*! The position and orientation of the object */
	public : Se3r se3;

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Constructor/Destructor									///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Constructor */
	public : Body ();

	/*! Destructor */
	public : virtual ~Body ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Methods											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	/*! Abstract function overloaded in Connex and NonConnex body. It is called to draw the object in an opengl context */
	public : virtual void glDraw() {/*throw CallVirtualUndifinedMethod()*/};

	/*! Abstract function overloaded in Connex and NonConnex body. It is called to update the bounding volume of the object in generel at the end of each time step */
	public : virtual void updateBoundingVolume(Se3r& ) {};

	/*! Abstract function overloaded in Connex and NonConnex body. It is called to update the collision model of the object. It is useful if the object is deformable
	and needs to recompute its collision model */
	public : virtual void updateCollisionGeometry(Se3r& ) {};

	/*! Abstract function overloaded in Connex and NonConnex body. Move the body to the next time step using its own internal dynamic engine */
	public : virtual void moveToNextTimeStep() {};

	/*! If computations on the attributes are needed after serialization. For example you may want to serialize the name of a file that contains geometrical data and processAttributes will load the file */

	public : void processAttributes();
	/*! Tells the IOManager which attributes should be serialized */
	public : void registerAttributes();

	REGISTER_CLASS_NAME(Body);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Body,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BODY_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

