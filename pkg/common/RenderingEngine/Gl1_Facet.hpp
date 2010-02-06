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
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	RENDERS(Facet);
	YADE_CLASS_BASE_DOC_ATTRS(Gl1_Facet,GlShapeFunctor,"Renders :yref:`Facet` object",
		((normals,"|ystatic| Also render facets' and edges' normals. :ydefault:`false`"))
	);
};

REGISTER_SERIALIZABLE(Gl1_Facet);


