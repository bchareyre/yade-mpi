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
}


Interaction::Interaction(unsigned int newId1,unsigned int newId2) : id1(newId1) , id2(newId2)
{	
	isNew = true;
	isReal = false;
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

