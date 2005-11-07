/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BOX2POLYHEDRALSWEPTSPHERE_HPP
#define BOX2POLYHEDRALSWEPTSPHERE_HPP

#include <yade/yade-package-common/InteractingGeometryEngineUnit.hpp>

class Box2PolyhedralSweptSphere : public InteractingGeometryEngineUnit
{
	public :
		void go(	  const shared_ptr<GeometricalModel>& gm
				, shared_ptr<InteractingGeometry>& ig
				, const Se3r& se3
				, const Body*	);

	REGISTER_CLASS_NAME(Box2PolyhedralSweptSphere);
	REGISTER_BASE_CLASS_NAME(InteractingGeometryEngineUnit);

};

REGISTER_SERIALIZABLE(Box2PolyhedralSweptSphere,false);

#endif // BOX2POLYHEDRALSWEPTSPHERE_HPP

