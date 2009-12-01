/*************************************************************************
*  Copyright (C) 2008 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg-common/InteractionGeometryEngineUnit.hpp>
//#include<yade/pkg-dem/InteractingSphere2InteractingSphere4SpheresContactGeometry.hpp>
#include<yade/pkg-snow/Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry.hpp>

class Ef2_BssSnowGrain_BssSnowGrain_makeIstSnowLayersContact : public InteractionGeometryEngineUnit
{
	public :
		//InteractingSphere2InteractingSphere4SpheresContactGeometry g;
		Ef2_BssSnowGrain_BssSnowGrain_makeSpheresContactGeometry g;

		virtual bool go(const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const shared_ptr<Interaction>& c);
		virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1, const shared_ptr<InteractingGeometry>& cm2, const State& state1, const State& state2, const Vector3r& shift2, const shared_ptr<Interaction>& c);
					
		Ef2_BssSnowGrain_BssSnowGrain_makeIstSnowLayersContact();

		//bool box;
	std::vector<Vector3r> find_boundary(std::vector<Vector3r> points);
	bool is_point_inside_cross_section(Vector3r v,Real point_plane_distance,const std::vector<Vector3r>& cross_section_circumference,Vector3r center,Vector3r N);
		
	REGISTER_CLASS_NAME(Ef2_BssSnowGrain_BssSnowGrain_makeIstSnowLayersContact);
	REGISTER_BASE_CLASS_NAME(InteractionGeometryEngineUnit);

	FUNCTOR2D(BssSnowGrain,BssSnowGrain);
	
	DEFINE_FUNCTOR_ORDER_2D(BssSnowGrain,BssSnowGrain);

	REGISTER_ATTRIBUTES(InteractionGeometryEngineUnit,/* */);
};

REGISTER_SERIALIZABLE(Ef2_BssSnowGrain_BssSnowGrain_makeIstSnowLayersContact);


