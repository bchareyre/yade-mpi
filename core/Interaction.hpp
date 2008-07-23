// Copyright (C) 2004 by Olivier Galizzi <olivier.galizzi@imag.fr>
//  Copyright (C) 2004 by Janek Kozicki <cosurgi@berlios.de>
//
#pragma once
#include<boost/shared_ptr.hpp>
#include<yade/lib-serialization/Serializable.hpp>
#include"InteractionGeometry.hpp"
#include"InteractionPhysics.hpp"

/////////////////////////////////
// FIXME - this is in wrong file!
//#include<boost/strong_typedef.hpp>
//BOOST_STRONG_TYPEDEF(int, body_id_t)
typedef int body_id_t;

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

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(Interaction);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(Interaction,false);
