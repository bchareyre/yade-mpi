/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawInteractingSphere : public GLDrawInteractingGeometryFunctor
{	
	private :
		static bool wire, glutNormalize;
		static int glutSlices, glutStacks;
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	RENDERS(InteractingSphere);
	REGISTER_ATTRIBUTES(Serializable,(wire)(glutNormalize)(glutSlices)(glutStacks));
	REGISTER_CLASS_AND_BASE(GLDrawInteractingSphere,GLDrawInteractingGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawInteractingSphere);


