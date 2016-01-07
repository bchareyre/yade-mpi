/*************************************************************************
*  Copyright (C) 2013 by Burak ER                                 	 *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include <pkg/common/GLDrawFunctors.hpp>
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>

class Gl1_DeformableElement : public GlShapeFunctor
{	
	public:
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
		YADE_CLASS_BASE_DOC(Gl1_DeformableElement,GlShapeFunctor,"Renders :yref:`Node` object"
		);
	RENDERS(DeformableElement);
};

REGISTER_SERIALIZABLE(Gl1_DeformableElement);


