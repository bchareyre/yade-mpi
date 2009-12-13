/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class Gl1_Box : public GlShapeFunctor
{
	public :
		virtual void go(const shared_ptr<Shape>&, const shared_ptr<State>&,bool,const GLViewInfo&);

	RENDERS(Box);
	REGISTER_CLASS_NAME(Gl1_Box);
	REGISTER_BASE_CLASS_NAME(GlShapeFunctor);
};

REGISTER_SERIALIZABLE(Gl1_Box);


