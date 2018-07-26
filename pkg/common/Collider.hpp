/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<core/Bound.hpp>
#include<core/Interaction.hpp>
#include<core/GlobalEngine.hpp>

#include<pkg/common/Dispatching.hpp>

#define YADE_MPI

class Collider: public GlobalEngine {
	public:
		static int avoidSelfInteractionMask;
		/*! Probe the Bound on a bodies presense. Returns list of body ids with which there is potential overlap. */
		virtual  vector<Body::id_t> probeBoundingVolume(const Bound&){throw;}
		/*! Tell whether given bodies may interact, for other than spatial reasons.
		 *
		 * Concrete collider implementations should call this function if
		 * the bodies are in potential interaction geometrically. */
		static bool mayCollide(const Body*, const Body*
		#ifdef YADE_MPI
		,Body::id_t subdomain
		#endif 
		);
		/*! Invalidate all persistent data (if the collider has any), forcing reinitialization at next run.
		The default implementation does nothing, colliders should override it if it is applicable.

		Currently used from Shop::flipCell, which changes cell information for bodies.
		*/
		virtual void invalidatePersistentData(){}

		// ctor with functors for the integrated BoundDispatcher
		virtual void pyHandleCustomCtorArgs(boost::python::tuple& t, boost::python::dict& d);
		
		int get_avoidSelfInteractionMask(){return avoidSelfInteractionMask;}
		void set_avoidSelfInteractionMask(const int &v){avoidSelfInteractionMask = v;}
		
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Collider,GlobalEngine,"Abstract class for finding spatial collisions between bodies. \n\n.. admonition:: Special constructor\n\n\tDerived colliders (unless they override ``pyHandleCustomCtorArgs``) can be given list of :yref:`BoundFunctors <BoundFunctor>` which is used to initialize the internal :yref:`boundDispatcher <Collider.boundDispatcher>` instance.",
		((shared_ptr<BoundDispatcher>,boundDispatcher,new BoundDispatcher,Attr::readonly,":yref:`BoundDispatcher` object that is used for creating :yref:`bounds <Body.bound>` on collider's request as necessary.")),
		/*ctor*/,
		.add_property("avoidSelfInteractionMask",&Collider::get_avoidSelfInteractionMask,&Collider::set_avoidSelfInteractionMask,"This mask is used to avoid the interactions inside a group of particles. To do so, the particles must have the exact same mask and that mask should have one bit in common with this :yref:`avoidSelfInteractionMask<Collider.avoidSelfInteractionMask>` as for their binary representations.")
	);
};
REGISTER_SERIALIZABLE(Collider);
