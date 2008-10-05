/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Interaction.hpp"

Interaction::Interaction ()
{
	// FIXME : -1
	id1 = 0; 
	id2 = 0;
	isNew = true;
	isReal = false; // maybe we can remove this, and check if InteractingGeometry, and InteractionPhysics are empty?
	
	isNeighbor = true;//NOTE : TriangulationCollider needs that

}


Interaction::Interaction(body_id_t newId1,body_id_t newId2) : id1(newId1) , id2(newId2)
{	
	isNew = true;
	isReal = false;
	isNeighbor = true;
	isNeighbor = true;//NOTE : TriangulationCollider needs that

}


void Interaction::registerAttributes()
{
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
	REGISTER_ATTRIBUTE(isNew);
	REGISTER_ATTRIBUTE(isReal);
	REGISTER_ATTRIBUTE(interactionGeometry);
	REGISTER_ATTRIBUTE(interactionPhysics);
}

void Interaction::swapOrder(){
	if(interactionGeometry || interactionPhysics || !isNew){
		throw std::logic_error("Bodies in interaction cannot be swapped if !isNew, have interactionGeometry or have interactionPhysics.");
	}
	std::swap(id1,id2);
}
