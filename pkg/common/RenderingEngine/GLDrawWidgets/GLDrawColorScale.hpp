/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawColorScale : public GLDrawGeometricalModelFunctor
{
	
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);
		
	RENDERS(ColorScale);
	REGISTER_CLASS_NAME(GLDrawColorScale);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);
};

REGISTER_SERIALIZABLE(GLDrawColorScale);


