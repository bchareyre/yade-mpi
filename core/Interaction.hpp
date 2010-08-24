// Copyright (C) 2004 by Olivier Galizzi <olivier.galizzi@imag.fr>
//  Copyright (C) 2004 by Janek Kozicki <cosurgi@berlios.de>
//
#pragma once
#include<yade/lib-serialization/Serializable.hpp>
// keep those two here, template instantiation & boost::python gets broken otherwise, e.g. https://bugs.launchpad.net/bugs/618766
#include<yade/core/InteractionGeometry.hpp> 
#include<yade/core/InteractionPhysics.hpp>
#include<yade/core/Body.hpp>


class InteractionGeometryFunctor;
class InteractionPhysicsFunctor;
class LawFunctor;
class Scene;

class Interaction : public Serializable
{
	private	:
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

		Interaction(Body::id_t newId1,Body::id_t newId2);

		const Body::id_t& getId1() const {return id1;};
		const Body::id_t& getId2() const {return id2;};

		//! swaps order of bodies within the interaction
		void swapOrder();

		bool operator<(const Interaction& other) const { return getId1()<other.getId1() || (getId1()==other.getId1() && getId2()<other.getId2()); }

		//! cache functors that are called for this interaction. Currently used by InteractionDispatchers.
		struct {
			// Whether geometry dispatcher exists at all; this is different from !geom, since that can mean we haven't populated the cache yet.
			// Therefore, geomExists must be initialized to true first (done in Interaction::reset() called from ctor).
			bool geomExists;
			#ifdef YADE_DEVIRT_FUNCTORS
				// is a InteractionGeometryFunctor::StaticFuncPtr, but we would have to #include a file from pkg-common here
				// cast at those few places instead, for now
				void* geomPtr;
			#endif
			// shared_ptr's are initialized to NULLs automagically
			shared_ptr<InteractionGeometryFunctor> geom;
			shared_ptr<InteractionPhysicsFunctor> phys;
			shared_ptr<LawFunctor> constLaw;
		} functorCache;

		//! Reset interaction to the intial state (keep only body ids)
		void reset();
		//! common initialization called from both constructor and reset()
		void init();
			
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Interaction,Serializable,"Interaction between pair of bodies.",
		((Body::id_t,id1,0,Attr::pyReadonly,":yref:`Id<Body::id>` of the first body in this interaction."))
		((Body::id_t,id2,0,Attr::pyReadonly,":yref:`Id<Body::id>` of the first body in this interaction."))
		((long,iterMadeReal,-1,,"Step number at which the interaction was fully (in the sense of interactionGeometry and interactionPhysics) created. (Should be touched only by :yref:`InteractionPhysicsDispatcher` and :yref:`InteractionDispatchers`, therefore they are made friends of Interaction"))
		((shared_ptr<InteractionGeometry>,interactionGeometry,,,"Geometry part of the interaction."))
		((shared_ptr<InteractionPhysics>,interactionPhysics,,,"Physical (material) part of the interaction."))
		((Vector3i,cellDist,Vector3i(0,0,0),,"Distance of bodies in cell size units, if using periodic boundary conditions; id2 is shifted by this number of cells from its :yref:`State::pos` coordinates for this interaction to exist. Assigned by the collider.\n\n.. warning::\n\t(internal)  cellDist must survive Interaction::reset(), it is only initialized in ctor. Interaction that was cancelled by the constitutive law, was reset() and became only potential must have the priod information if the geometric functor again makes it real. Good to know after few days of debugging that :-)")),
		/* ctor */ init(),
		/*py*/
		.def_readwrite("geom",&Interaction::interactionGeometry,"Shorthand for :yref:`Interaction::interactionGeometry`")
		.def_readwrite("phys",&Interaction::interactionPhysics,"Shorthand for :yref:`Interaction::interactionPhysics`")
		.add_property("isReal",&Interaction::isReal,"True if this interaction has both geom and phys; False otherwise.")
	);
};

REGISTER_SERIALIZABLE(Interaction);
