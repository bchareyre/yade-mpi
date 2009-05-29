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

#include<yade/core/MetaBody.hpp>

Interaction::Interaction(): id1(0), id2(0){ reset(); }
Interaction::Interaction(body_id_t newId1,body_id_t newId2): id1(newId1), id2(newId2){ reset(); }

bool Interaction::isFresh(MetaBody* rb){ return iterMadeReal==rb->currentIteration;}


void Interaction::reset(){
	isNeighbor = true;//NOTE : TriangulationCollider needs that
	iterMadeReal=-1;
	functorCache.geomExists=true;
	//functorCache.geom=shared_ptr<InteractionGeometryEngineUnit>(); functorCache.phys=shared_ptr<InteractionPhysicsEngineUnit>(); functorCache.constLaw=shared_ptr<ConstitutiveLaw>();
}


void Interaction::swapOrder(){
	if(interactionGeometry || interactionPhysics){
		throw std::logic_error("Bodies in interaction cannot be swapped if they have interactionGeometry or interactionPhysics.");
	}
	std::swap(id1,id2);
}
