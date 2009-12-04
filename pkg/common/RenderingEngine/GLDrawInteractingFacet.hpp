/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawInteractingFacet : public GLDrawInteractingGeometryFunctor
{	
	//! render facet's and edges' normals
	static bool normals;
	public :
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);

	RENDERS(InteractingFacet);
	REGISTER_CLASS_NAME(GLDrawInteractingFacet);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
	REGISTER_ATTRIBUTES(GLDrawInteractingGeometryFunctor,(normals));
};

REGISTER_SERIALIZABLE(GLDrawInteractingFacet);


