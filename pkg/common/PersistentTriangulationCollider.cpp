/*************************************************************************
*  Copyright (C) 2009 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef YADE_CGAL

#include"PersistentTriangulationCollider.hpp"
#include<yade/core/Body.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/BodyContainer.hpp>
#include<limits>
#include<utility>
#include<vector>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/ElastMat.hpp>


using namespace std;
		
// PersistentTriangulationCollider::PersistentTriangulationCollider() : Collider()
// {
// 	haveDistantTransient=false;
// 	isTriangulated = false;
// 	Tes = new ( TesselationWrapper );
// 
// 	nbObjects=0;
// 	xBounds.clear();
// 	yBounds.clear();
// 	zBounds.clear();
// 	minima.clear();
// 	maxima.clear();
// }


PersistentTriangulationCollider::~PersistentTriangulationCollider()
{
	delete Tes;
}

void PersistentTriangulationCollider::action ()
{
	// compatibility func, can be removed later
	findBoundDispatcherInEnginesIfNoFunctorsAndWarn();
	// update bounds
	boundDispatcher->scene=scene; boundDispatcher->action();

	shared_ptr<BodyContainer> bodies=scene->bodies;
	bool triangulationIteration = false;

//BEGIN VORONOI TESSELATION
	if ( !isTriangulated || scene->iter % 50 == 0 )
	{
//TesselationWrapper Tes;
		Tes->clear();
		FOREACH(const shared_ptr<Body>& b, *scene->bodies){
			if(!b || !b->isDynamic()) continue;
			//means "is it a sphere (not a wall)"
			const Sphere* s = YADE_CAST<Sphere*>(b->shape.get());
			Tes->insert ( b->state->pos[0],b->state->pos[1],b->state->pos[2], s->radius, b->getId() );
		}
		Tes->AddBoundingPlanes();
		isTriangulated = true;
		triangulationIteration = true;
		//}
//  else
//  {
//   //if (scene->iter % 100 == 0) {
//   if (1) {
//   Tes->RemoveBoundingPlanes();
//   BodyContainer::iterator bi    = bodies->begin();
//   BodyContainer::iterator biEnd = bodies->end();
//   for ( ; bi!=biEnd ; ++bi )
//   {
//    if ( ( *bi )->isDynamic() )
//    {//means "is it a sphere (not a wall)"
//     const Sphere* s = YADE_CAST<Sphere*> ( ( *bi )->shape.get() );
//     const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*> ( ( *bi )->physicalParameters.get() );
//     Tes->move ( p->se3.position[0],p->se3.position[1],p->se3.position[2], s->radius, ( *bi )->getId() );
// //    else {
// //    const Box* s = YADE_CAST<Box*>((*bi)->shape.get());
// //    const RigidBodyParameters* p = YADE_CAST<RigidBodyParameters*>((*bi)->physicalParameters.get());
// //    Tes.move(p->se3.position[0],p->se3.position[1],p->se3.position[2], s->radius, (*bi)->getId());
// //    }
//    }
//   }
//   Tes->AddBoundingPlanes();
//   }
	}
//ENDOF VORONOI TESSELATION


	interactions = scene->interactions;

	if ( triangulationIteration )
	{
		std::pair<unsigned int,unsigned int> interaction_pair;
		unsigned int& id1 = interaction_pair.first;
		unsigned int& id2 = interaction_pair.second;

		unsigned int numberOfInteractions = Tes->NumberOfFacets(true);
		for ( unsigned int i=0; i<numberOfInteractions; ++i )
		{
			Tes->nextFacet ( interaction_pair );

			// look if the pair (id1,id2) already exists in the overleppingBB collection
			const shared_ptr<Interaction>& interaction=interactions->find ( Body::id_t ( id1 ),Body::id_t ( id2 ) );
			bool found= ( interaction!=0 );//Bruno's Hack

			// inserts the pair p=(id1,id2) if the two Aabb overlaps and if p does not exists in the overlappingBB
			if ( !found )
			{
				interactions->insert ( Body::id_t ( id1 ),Body::id_t ( id2 ) );
				//cerr << "inserted " << id1 << "-" << id2<<endl;
			}
			// interactions with iterLastSeen<scene->iter will be deleted by InteractionLoop, if the collider was run at that step as well
			interaction->iterLastSeen=scene->iter;
			// removes the pair p=(id1,id2) if the two Aabb do not overlapp any more and if p already exists in the overlappingBB
		}
	}

}

YADE_PLUGIN((PersistentTriangulationCollider));

#endif /* YADE_CGAL */
