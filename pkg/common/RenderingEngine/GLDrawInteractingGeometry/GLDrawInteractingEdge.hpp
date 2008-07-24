/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef __GLDRAWINTERACTINGEDGE_HPP__
#define __GLDRAWINTERACTINGEDGE_HPP__

#include<yade/pkg-common/GLDrawFunctors.hpp>

class GLDrawInteractingEdge : public GLDrawInteractingGeometryFunctor
{	
	
	public :
		virtual void go(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&,bool);

	RENDERS(InteractingEdge);
	REGISTER_CLASS_NAME(GLDrawInteractingEdge);
	REGISTER_BASE_CLASS_NAME(GLDrawInteractingGeometryFunctor);
};

REGISTER_SERIALIZABLE(GLDrawInteractingEdge,false);

#endif //  GLDRAWINTERACTINGEDGE_HPP

