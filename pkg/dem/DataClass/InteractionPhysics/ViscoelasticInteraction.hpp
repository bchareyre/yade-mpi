/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-dem/ElasticContactInteraction.hpp>

class ViscoelasticInteraction : public ElasticContactInteraction
{
	public :
	    
	    /// Normal viscous 
	    Real cn;
	    /// Shear viscous 
	    Real cs;
	
		ViscoelasticInteraction();
		virtual ~ViscoelasticInteraction();
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(ViscoelasticInteraction);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);

	REGISTER_CLASS_INDEX(ViscoelasticInteraction,InteractionPhysics);

};

REGISTER_SERIALIZABLE(ViscoelasticInteraction);


