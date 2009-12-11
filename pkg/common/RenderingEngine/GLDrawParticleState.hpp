/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawParticleState : public GlStateFunctor
{
	public :
		virtual void go(const shared_ptr<PhysicalParameters>&); // should be: BodyState

	RENDERS(ParticleParameters); // should be: ParticleState
	REGISTER_CLASS_NAME(GLDrawParticleState);
	REGISTER_BASE_CLASS_NAME(GlStateFunctor);
};

REGISTER_SERIALIZABLE(GLDrawParticleState);


