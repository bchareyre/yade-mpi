/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class Gl1_Box : public GlShapeFunctor{
	public :
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);
	RENDERS(Box);
	YADE_CLASS_BASE_DOC_ATTRS(Gl1_Box,GlShapeFunctor,"Renders :yref:`Box` object",);
};

REGISTER_SERIALIZABLE(Gl1_Box);


