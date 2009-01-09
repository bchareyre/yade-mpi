#pragma once

#include<yade/core/GeometricalModel.hpp>
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<vector>
#include<boost/serialization/vector.hpp>
#include<boost/serialization/shared_ptr.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include"BshSnowGrain.hpp"

class BssSnowGrain : public InteractingSphere
{
	public: 
		std::map<int,Real>	depth;
		BshSnowGrain	m_copy;

		BssSnowGrain();
		BssSnowGrain(BshSnowGrain* grain, Real one_voxel_in_meters_is);
	
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(BssSnowGrain);
	REGISTER_BASE_CLASS_NAME(InteractingSphere);
	REGISTER_CLASS_INDEX(BssSnowGrain,InteractingSphere);
};

REGISTER_SERIALIZABLE(BssSnowGrain);




