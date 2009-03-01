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

class InteractionGeometryEngineUnit;
class InteractionPhysicsEngineUnit;
class ConstitutiveLaw;

class Interaction : public Serializable
{
	private	:
		body_id_t id1,id2;	// FIXME  this should be vector<body_id_t> ids;

	public :
		bool isNew;		// FIXME : better to test if InteractionPhysics==0 and remove this flag; we can also remove this flag, if we make another container for PotetntialInteraction with only ids
		bool isReal;		// maybe we can remove this, and check if InteractingGeometry, and InteractionPhysics are empty?
		bool cycle; // phase flag to mark (for example, SpatialQuickSortCollider mark by it the stale interactions) 
		bool isNeighbor;	// Has a meaning only with triangulationCollider atm NOTE : TriangulationCollider needs that

		shared_ptr<InteractionGeometry> interactionGeometry;
		shared_ptr<InteractionPhysics> interactionPhysics;

		Interaction ();
		Interaction(body_id_t newId1,body_id_t newId2);

		body_id_t getId1() {return id1;};
		body_id_t getId2() {return id2;};

		//! swaps order of bodies within the interaction
		void swapOrder();

		//! cache functors that are called for this interaction. Currently used by InteractionDispatchers.
		struct {
			// Whether geometry dispatcher exists at all; this is different from !geom, since that can mean we haven't populated the cache yet.
			// Therefore, geomExists must be initialized to true first (done in Interaction ctor).
			bool geomExists;
			// shared_ptr's are initialized to NULLs automagically
			shared_ptr<InteractionGeometryEngineUnit> geom;
			shared_ptr<InteractionPhysicsEngineUnit> phys;
			shared_ptr<ConstitutiveLaw> constLaw;
		} functorCache;
			

		#if 0
			//! Whether both bodies involved in interaction satisfies given mask; provide rootBody for faster lookup
			bool maskBothOK(int mask, MetaBody* rootBody=NULL){return (mask==0) || (Body::byId(id1,rootBody)->maskOK(mask) && Body::byId(id2,rootBody)->maskOK(mask));}
			//! Whether at least one body in interaction satisfies given mask; provide rootBody for faster lookup
			bool maskAnyOK(int mask, MetaBody* rootBody=NULL){return (mask==0) || Body::byId(id1,rootBody)->maskOK(mask) || Body::byId(id2,rootBody)->maskOK(mask);}
		#endif

	REGISTER_ATTRIBUTES(/*no base*/,
		(id1)
		(id2)
		(isNew)
		(isReal)
		(interactionGeometry)
		(interactionPhysics)
	);
	REGISTER_CLASS_AND_BASE(Interaction,Serializable);
};

REGISTER_SERIALIZABLE(Interaction);
