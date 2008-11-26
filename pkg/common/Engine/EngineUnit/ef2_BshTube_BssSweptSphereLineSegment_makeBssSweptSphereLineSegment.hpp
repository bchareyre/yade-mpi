/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ef2_BSH_TUBE__BSS_SSLS__MAKE_BSS_SSLS_HPP
#define ef2_BSH_TUBE__BSS_SSLS__MAKE_BSS_SSLS_HPP

#include<yade/pkg-common/InteractingGeometryEngineUnit.hpp>

class ef2_BshTube_BssSweptSphereLineSegment_makeBssSweptSphereLineSegment : public InteractingGeometryEngineUnit
{
	public :
		void go(	  const shared_ptr<GeometricalModel>& gm
				, shared_ptr<InteractingGeometry>& ig
				, const Se3r& se3
				, const Body*	);

        FUNCTOR2D(BshTube,BssSweptSphereLineSegment);
        REGISTER_CLASS_NAME(ef2_BshTube_BssSweptSphereLineSegment_makeBssSweptSphereLineSegment);
	REGISTER_BASE_CLASS_NAME(InteractingGeometryEngineUnit);
	
        DEFINE_FUNCTOR_ORDER_2D(BshTube,BssSweptSphereLineSegment);
};

REGISTER_SERIALIZABLE(ef2_BshTube_BssSweptSphereLineSegment_makeBssSweptSphereLineSegment);

#endif // ef2_BSH_TUBE__BSS_SSLS__MAKE_BSS_SSLS_HPP


