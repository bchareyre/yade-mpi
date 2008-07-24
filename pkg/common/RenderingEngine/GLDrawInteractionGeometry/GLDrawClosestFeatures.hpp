/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_CLOSEST_FEATURES_HPP
#define GLDRAW_CLOSEST_FEATURES_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawClosestFeatures : public GLDrawInteractionGeometryFunctor
{
	public :
		virtual void go(const shared_ptr<InteractionGeometry>&,const shared_ptr<Interaction>&,const shared_ptr<Body>&,const shared_ptr<Body>&,bool wireFrame);

	RENDERS(ClosestFeatures);
	REGISTER_CLASS_NAME(GLDrawClosestFeatures);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractionGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawClosestFeatures,false);

#endif

