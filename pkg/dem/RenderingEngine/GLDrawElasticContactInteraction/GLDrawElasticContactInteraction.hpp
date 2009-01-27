/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawElasticContactInteraction : public GLDrawInteractionPhysicsFunctor
{	
	int boxIndex;
	Real maxLength;
	public :
		GLDrawElasticContactInteraction();
		virtual void go(const shared_ptr<InteractionPhysics>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);
		void drawArrow(const Vector3r from,const Vector3r to,const Vector3r color);
		void drawFlatText(const Vector3r pos,const std::string txt);

	RENDERS(ElasticContactInteraction);
	REGISTER_CLASS_NAME(GLDrawElasticContactInteraction);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionPhysicsFunctor);
};

REGISTER_SERIALIZABLE(GLDrawElasticContactInteraction);


