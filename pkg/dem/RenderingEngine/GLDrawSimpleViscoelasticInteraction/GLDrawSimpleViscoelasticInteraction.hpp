/*************************************************************************
*  Copyright (C) 2006 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawSimpleViscoelasticInteraction : public GLDrawInteractionPhysicsFunctor
{
	public :
		virtual void go(const shared_ptr<InteractionPhysics>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);

	RENDERS(SimpleViscoelasticInteraction);
	REGISTER_CLASS_NAME(GLDrawSimpleViscoelasticInteraction);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionPhysicsFunctor);
};

REGISTER_SERIALIZABLE(GLDrawSimpleViscoelasticInteraction);


