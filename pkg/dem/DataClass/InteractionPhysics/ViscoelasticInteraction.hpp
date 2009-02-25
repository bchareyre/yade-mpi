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

	REGISTER_ATTRIBUTES(ElasticContactInteraction,(cn)(cs));
	REGISTER_CLASS_AND_BASE(ViscoelasticInteraction,ElasticContactInteraction);

};

REGISTER_SERIALIZABLE(ViscoelasticInteraction);


