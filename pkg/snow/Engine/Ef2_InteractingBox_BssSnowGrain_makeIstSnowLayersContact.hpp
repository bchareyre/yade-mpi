/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>
//#include<yade/pkg-dem/InteractingBox2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-snow/Ef2_BssSnowGrain_BssSnowGrain_makeIstSnowLayersContact.hpp>
#include<yade/pkg-snow/Ef2_InteractingBox_BssSnowGrain_makeSpheresContactGeometry.hpp>

class Ef2_InteractingBox_BssSnowGrain_makeIstSnowLayersContact : public InteractionGeometryEngineUnit
{
	Ef2_BssSnowGrain_BssSnowGrain_makeIstSnowLayersContact g;
	Ef2_InteractingBox_BssSnowGrain_makeSpheresContactGeometry ggg;
	public :
		virtual bool go(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const State& state1, const State state2, const Vector3r& shift2,
					const shared_ptr<Interaction>& c);

		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1,
					const shared_ptr<InteractingGeometry>& cm2,
					const State& state1, const State state2, const Vector3r& shift2,
					const shared_ptr<Interaction>& c);

	DECLARE_LOGGER;

	REGISTER_CLASS_NAME(Ef2_InteractingBox_BssSnowGrain_makeIstSnowLayersContact);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	FUNCTOR2D(InteractingBox,BssSnowGrain);

	DEFINE_FUNCTOR_ORDER_2D(InteractingBox,BssSnowGrain);
};

REGISTER_SERIALIZABLE(Ef2_InteractingBox_BssSnowGrain_makeIstSnowLayersContact);


