/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Interaction.hpp"

#include<yade/core/Scene.hpp>

Interaction::Interaction(Body::id_t newId1,Body::id_t newId2): id1(newId1), id2(newId2), cellDist(Vector3i(0,0,0)){ reset(); }

bool Interaction::isFresh(Scene* rb){ return iterMadeReal==rb->iter;}

void Interaction::init(){
	isNeighbor = true;//NOTE : TriangulationCollider needs that
	iterMadeReal=-1;
	functorCache.geomExists=true;
	//functorCache.geom=shared_ptr<InteractionGeometryFunctor>(); functorCache.phys=shared_ptr<InteractionPhysicsFunctor>(); functorCache.constLaw=shared_ptr<LawFunctor>();
}

void Interaction::reset(){
	interactionGeometry=shared_ptr<InteractionGeometry>();
	interactionPhysics=shared_ptr<InteractionPhysics>();
	init();
}


void Interaction::swapOrder(){
	if(interactionGeometry || interactionPhysics){
		throw std::logic_error("Bodies in interaction cannot be swapped if they have interactionGeometry or interactionPhysics.");
	}
	std::swap(id1,id2);
	cellDist*=-1;
}
