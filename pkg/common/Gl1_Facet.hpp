/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/common/GLDrawFunctors.hpp>

class Gl1_Facet : public GlShapeFunctor
{	
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	RENDERS(Facet);
	YADE_CLASS_BASE_DOC_STATICATTRS(Gl1_Facet,GlShapeFunctor,"Renders :yref:`Facet` object",
		((bool,normals,false,,"In wire mode, render normals of facets and edges; facet's :yref:`colors<Shape::color>` are disregarded in that case."))
	);
};

REGISTER_SERIALIZABLE(Gl1_Facet);


