/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_SIMPLE_ELASTIC_INTERACTION_HPP
#define GLDRAW_SIMPLE_ELASTIC_INTERACTION_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawNormalInteraction : public GLDrawInteractionPhysicsFunctor
{
	public :
		virtual void go(const shared_ptr<InteractionPhysics>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);

	RENDERS(NormalInteraction);
	REGISTER_CLASS_NAME(GLDrawNormalInteraction);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionPhysicsFunctor);
};

REGISTER_SERIALIZABLE(GLDrawNormalInteraction,false);

#endif

