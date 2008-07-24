/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAWFACET_HPP
#define GLDRAWFACET_HPP

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawFacet : public GLDrawGeometricalModelFunctor
{
	
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);
		
	RENDERS(Facet);
	REGISTER_CLASS_NAME(GLDrawFacet);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);
};

REGISTER_SERIALIZABLE(GLDrawFacet,false);

#endif // GLDRAWFACET_HPP

