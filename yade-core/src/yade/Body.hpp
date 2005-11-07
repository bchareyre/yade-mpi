/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BODY_HPP
#define BODY_HPP

#include <iostream>

#include "GeometricalModel.hpp"
#include "InteractingGeometry.hpp"
#include "BoundingVolume.hpp"
#include "PhysicalParameters.hpp"
#include "InteractionContainer.hpp"
#include "Interaction.hpp"
#include "PhysicalActionContainer.hpp"

#include <yade/yade-lib-wm3-math/Se3.hpp>
#include <yade/yade-lib-serialization/Serializable.hpp>
#include <yade/yade-lib-multimethods/Indexable.hpp>


/*! \brief Abstract interface for bodies stored in BodyContainer, Body represents the atomic element of simulation.
*/

class Body : public Serializable
{

/// Attributes
	private	: unsigned int id;
	public	: unsigned int getId() const {return id;};

	// group to which body belongs (maybe vector<int> , to allow multiple groups?)

	// we can use them to make group one yellow, and group two red, or to record data from some
	// selected bodies

	// FIXME - but we SHOULDN'T use them in InteractionSolver, because it allows
	//         to have flat simulation. We should make tree simulation and see...
	private : int groupMask;
	public  : int getGroupMask() {return groupMask; };

	// only BodyContainer can set the id of a body
	friend class BodyContainer;

	public : // FIXME - should be private ...
		shared_ptr<PhysicalParameters>	physicalParameters;	/// mass, sitffness
		shared_ptr<GeometricalModel>	geometricalModel;	/// Polyhedron, Box
		shared_ptr<InteractingGeometry> interactionGeometry;	/// sphere hierarchy, InteractingBox
		shared_ptr<BoundingVolume>	boundingVolume;		/// AABB, K-Dop
	
	// FIXME : should be determined automatically or not ?? if the body has a subscription to a
	// kinematic engine then it is not dynamic but maybe a body with no subscription can be not dynamic ??
	/*! isDynamic is true if the state of the body is not modified by a kinematicEngine. It is useful
	for example for collision detection : if two colliding bodies are only kinematic then it is useless to
	modelise their contact */
	public : bool isDynamic;
	
/// Constructor/Destructor

	public : Body ();
	public : Body (unsigned int newId, int newGroup);

/// Serialization
	
	REGISTER_CLASS_NAME(Body);
	REGISTER_BASE_CLASS_NAME(Serializable);

	public : void registerAttributes();

};

REGISTER_SERIALIZABLE(Body,false);

#endif // BODY_HPP

