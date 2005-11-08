/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#include "Box2PolyhedralSweptSphere.hpp"
#include "PolyhedralSweptSphere.hpp"
#include <yade/yade-package-common/Box.hpp>


void Box2PolyhedralSweptSphere::go(	const shared_ptr<GeometricalModel>& gm,
				shared_ptr<InteractingGeometry>& ig,
				const Se3r& se3,
				const Body*	)
{
	Box* box = static_cast<Box*>(gm.get());
	PolyhedralSweptSphere* pss = static_cast<PolyhedralSweptSphere*>(ig.get());
	
}
	
