// Copyright (C) 2004 by Olivier Galizzi <olivier.galizzi@imag.fr>
//  Copyright (C) 2004 by Janek Kozicki <cosurgi@berlios.de>
//
#pragma once
#include<yade/lib-serialization/Serializable.hpp>
#include"InteractionGeometry.hpp"
#include"InteractionPhysics.hpp"



/////////////////////////////////
// FIXME - this is in wrong file!
//#include<boost/strong_typedef.hpp>
//BOOST_STRONG_TYPEDEF(int, body_id_t)
typedef int body_id_t;

class InteractionGeometryFunctor;
class InteractionPhysicsFunctor;
class LawFunctor;
class Scene;

class Interaction : public Serializable
{
	private	:
		body_id_t id1,id2;
		//! Step number at which the interaction was fully created (interactionGeometry and interactionPhysics).
		//! Should be touched only by InteractionPhysicsDispatcher and InteractionDispatchers, making them friends therefore
		long iterMadeReal;
		friend class InteractionPhysicsDispatcher;
		friend class InteractionDispatchers;
	public :
		bool isReal() const {return (bool)interactionGeometry && (bool)interactionPhysics;}
		//! If this interaction was just created in this step (for the constitutive law, to know that it is the first time there)
		bool isFresh(Scene* rb);

		//! At which step this interaction was last detected by the collider. InteractionDispatcher will remove it if InteractionContainer::iterColliderLastRun==currentStep and iterLastSeen<currentStep
		long iterLastSeen;      
		//! NOTE : TriangulationCollider needs this (nothing else)
		bool isNeighbor;

		/*! relative distance between bodies, given in (Scene::cellSize) units
			Position of id1 must be incremented by that distance so that there is spatial interaction 

			NOTE (tricky): cellDist must survive Interaction::reset(), it is only initialized in ctor
			Interaction that was cancelled by the constitutive law, was reset() and became only potential
			must have the priod information if the geometric functor again makes it real. Good to know after
			few days of debugging that :-)
		*/
		Vector3<int> cellDist;

		shared_ptr<InteractionGeometry> interactionGeometry;
		shared_ptr<InteractionPhysics> interactionPhysics;

		Interaction ();
		Interaction(body_id_t newId1,body_id_t newId2);

		const body_id_t& getId1() const {return id1;};
		const body_id_t& getId2() const {return id2;};

		//! swaps order of bodies within the interaction
		void swapOrder();

		bool operator<(const Interaction& other) const { return getId1()<other.getId1() || (getId1()==other.getId1() && getId2()<other.getId2()); }

		//! cache functors that are called for this interaction. Currently used by InteractionDispatchers.
		struct {
			// Whether geometry dispatcher exists at all; this is different from !geom, since that can mean we haven't populated the cache yet.
			// Therefore, geomExists must be initialized to true first (done in Interaction::reset() called from ctor).
			bool geomExists;
			// shared_ptr's are initialized to NULLs automagically
			shared_ptr<InteractionGeometryFunctor> geom;
			shared_ptr<InteractionPhysicsFunctor> phys;
			shared_ptr<LawFunctor> constLaw;
		} functorCache;

		//! Reset interaction to the intial state (keep only body ids)
		void reset();
		//! common initialization called from both constructor and reset()
		void init();
			
	YADE_CLASS_BASE_DOC_ATTRS_PY(Interaction,Serializable,"Interaction between pair of bodies.",
		((id1,"Id of the 1st body in this interaction."))
		((id2,"Id of the 2nd body in this interaction"))
		((iterMadeReal,"Step number at which the interaction was fully (in the sense of interactionGeometry and interactionPhysics) created."))
		((interactionGeometry,"Geometry part of the interaction."))
		((interactionPhysics,"Physical (material) part of the interaction."))
		((cellDist,"Distance of bodies in cell size units, if using periodic boundary conditions; id2 is shifted by this number of cells from its Body::state::pos coordinates for this interaction to exist. Assigned by the collider.")),
		.def_readwrite("geom",&Interaction::interactionGeometry,"Shorthand for interactionGeometry")
		.def_readwrite("phys",&Interaction::interactionPhysics,"Shorthand for interactionPhysics")
		.add_property("isReal",&Interaction::isReal,"True if this interaction has both geom and phys; False otherwise.")
	);
};

REGISTER_SERIALIZABLE(Interaction);
