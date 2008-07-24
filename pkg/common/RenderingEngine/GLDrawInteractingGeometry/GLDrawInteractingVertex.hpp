/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef __GLDRAWINTERACTINGVERTEX_HPP__
#define __GLDRAWINTERACTINGVERTEX_HPP__

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawInteractingVertex : public GLDrawInteractingGeometryFunctor
{	
	
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool);

	RENDERS(InteractingVertex);
	REGISTER_CLASS_NAME(GLDrawInteractingVertex);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawInteractingVertex,false);

#endif //  GLDRAWINTERACTINGVERTEX_HPP

