/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef GLDRAWGEOMETRICALEDGE_HPP
#define GLDRAWGEOMETRICALEDGE_HPP

#include<yade/pkg-common/GLDrawGeometricalModelFunctor.hpp>

class GLDrawGeometricalEdge : public GLDrawGeometricalModelFunctor
{
	
	public :
		virtual void go(const shared_ptr<GeometricalModel>&, const shared_ptr<PhysicalParameters>&,bool);
		
	RENDERS(GeometricalEdge);
	REGISTER_CLASS_NAME(GLDrawGeometricalEdge);
	REGISTER_BASE_CLASS_NAME(GLDrawGeometricalModelFunctor);
};

REGISTER_SERIALIZABLE(GLDrawGeometricalEdge,false);

#endif // GLDRAWGEOMETRICALEDGE_HPP

