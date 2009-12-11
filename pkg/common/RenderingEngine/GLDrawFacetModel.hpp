/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawFacetModel : public GLDrawGeometricalModelFunctor
{
	
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);
		
	RENDERS(FacetModel);
	REGISTER_CLASS_NAME(GLDrawFacetModel);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);
};

REGISTER_SERIALIZABLE(GLDrawFacetModel);


