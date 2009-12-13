/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class Gl1_Facet : public GlShapeFunctor
{	
	//! render facet's and edges' normals
	static bool normals;
	public :
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);

	RENDERS(Facet);
	REGISTER_CLASS_NAME(Gl1_Facet);
	REGISTER_BASE_CLASS_NAME(GlShapeFunctor);
	REGISTER_ATTRIBUTES(GlShapeFunctor,(normals));
};

REGISTER_SERIALIZABLE(Gl1_Facet);


