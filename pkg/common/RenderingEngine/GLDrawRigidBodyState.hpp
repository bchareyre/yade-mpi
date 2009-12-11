/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawRigidBodyState : public GlStateFunctor
{
	public :
		virtual void go(const shared_ptr<PhysicalParameters>&/*, draw parameters: color, scale, given from OpenGLRenderer, or sth.... */); // should be: BodyState

	RENDERS(RigidBodyParameters); // should be: RigidBodyState
	REGISTER_CLASS_NAME(GLDrawRigidBodyState);
	REGISTER_BASE_CLASS_NAME(GlStateFunctor);
};

REGISTER_SERIALIZABLE(GLDrawRigidBodyState);


