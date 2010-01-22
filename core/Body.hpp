/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<iostream>
#include"GeometricalModel.hpp"
#include"Shape.hpp"
#include"Bound.hpp"
#ifdef YADE_PHYSPAR
	#include"PhysicalParameters.hpp"
#endif
#include"State.hpp"
#include"Material.hpp"

#include"InteractionContainer.hpp"
#include"Interaction.hpp"

#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>

#define ISDYNAMIC_REDEFINED

class Scene;

/*! \brief Abstract interface for bodies stored in BodyContainer, Body represents the atomic element of simulation.
 */
class Body : public Serializable
{
	public	:
		body_id_t id;
		/*! group to which body belongs (maybe vector<int> , to allow multiple groups?)
		 * we can use them to make group one yellow, and group two red, or to record data
		 * from some selected bodies */
		int groupMask;
	public	:
		/*! \brief Numerical type for ::Body::id.
		 * \bug  Current code mixes signed and unsigned int, this might be a way to enforce consistence. */
		// typedef unsigned int id_t;
		//! Clump of which this body makes part. If it is not part of a clump, set to Body::ID_NONE.
		body_id_t clumpId;
		//! symbolic constant for body that doesn't exist.
		static const body_id_t ID_NONE;
		//! get Body pointer given its id. 
		static const shared_ptr<Body>& byId(body_id_t _id,Scene* rb=NULL);
		static const shared_ptr<Body>& byId(body_id_t _id,shared_ptr<Scene> rb);

		
		//! Whether this Body is a Clump.
		//! @note The following is always true: \code (Body::isClump() XOR Body::isClumpMember() XOR Body::isStandalone()) \endcode
		bool isClump() const {return clumpId!=ID_NONE && id==clumpId;}
		//! Whether this Body is member of a Clump.
		bool isClumpMember() const {return clumpId!=ID_NONE && id!=clumpId;}
		//! Whether this body is standalone (neither Clump, nor member of a Clump)
		bool isStandalone() const {return clumpId==ID_NONE;}
		/*! Hook for clump to update position of members when user-forced reposition and redraw (through GUI) occurs.
		 * This is useful only in cases when engines that do that in every iteration are not active - i.e. when the simulation is paused.
		 * (otherwise, GLViewer would depend on Clump and therefore Clump would have to go to core...) */
		virtual void userForcedDisplacementRedrawHook(){return;}

		body_id_t getId() const {return id;};

		int getGroupMask() {return groupMask; };
		bool maskOk(int mask){return (mask==0 || (groupMask&mask));}

		// only BodyContainer can set the id of a body
		friend class BodyContainer;

		#ifdef YADE_PHYSPAR
			/// here are stored physical things that describe the Body: mass, stiffness
			shared_ptr<PhysicalParameters>	physicalParameters;
		#else
			//! material of the body; might be shared among bodies (via shared_ptr)
			shared_ptr<Material> material;
			//! state of the body
			shared_ptr<State> state;
		#endif
		#ifdef YADE_GEOMETRICALMODEL
			/// the 'perfect' representation of body's geometry: Polyhedron, BoxModel
			shared_ptr<GeometricalModel>	geometricalModel;
		#endif
		/// description of how this body interacts with others, like: SphereHierarchy, Box
		shared_ptr<Shape> shape;
		/// Bound is used for quick detection of potential interactions, that can be: Aabb, K-Dop
		shared_ptr<Bound>	bound;

		/*! isDynamic is true if the state of the body is not modified by a kinematicEngine.
		 * It is useful for example for collision detection : if two colliding bodies are only
		 * kinematic then it is useless to modelise their contact */
		bool isDynamic;
	
		// Constructor/Destructor
		Body ();
		Body (body_id_t newId, int newGroup);

	// Serialization
	//protected:
	//	REGISTER_ATTRIBUTES(Serializable,(id)(groupMask)(isDynamic)(material)(state)(shape)(bound)(clumpId));
	//REGISTER_CLASS_AND_BASE(Body,Serializable);
	YADE_CLASS_BASE_DOC_ATTRS_PY(Body,Serializable,"Basic element of simulation; interacts with other bodies.",
		((id,"[will be overridden]"))((groupMask,"Bitmask for determining interactions"))((isDynamic,"Whether this body will be moved by forces"))((material,"Material instance associated with this body"))((state,"Physical state"))((shape,"Shape of this body"))((bound,"Bound of this body"))((clumpId,"Id of clump this body makes part of; invalid number if not part of clump; see Body().isStandalone, Body().isClump, Body().isClumpMember properties")),
		.def_readwrite("mat",&Body::material,"Shorthand for material")
		.def_readwrite("dynamic",&Body::isDynamic,"Shorthand for isDynamic")
		.def_readonly("id",&Body::id,"Unique id of this body") // should overwrite def_readwrite("id",...) earlier
		.def_readwrite("mask",&Body::groupMask,"Shorthand for groupMask")
		.add_property("isStandalone",&Body::isStandalone,"True if this body is neither clump, nor clump member; false otherwise.")
		.add_property("isClumpMember",&Body::isClumpMember,"True if this body is clump member, false otherwise.")
		.add_property("isClump",&Body::isClump,"True if this body is clump itself, false otherwise");
	);
};
REGISTER_SERIALIZABLE(Body);
