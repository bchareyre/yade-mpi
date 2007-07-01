#pragma once

#include<vector>
#include<map>
#include<boost/shared_ptr.hpp>

#include<yade/core/Body.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/FileGenerator.hpp>
#include<yade/core/DeusExMachina.hpp>
#include<yade/lib-factory/Factorable.hpp>
#include<yade/pkg-common/PhysicalParametersEngineUnit.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/AABB.hpp>
#include<yade/lib-base/Logging.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>


class ScrewGen: public FileGenerator {
	DECLARE_LOGGER;
	private:
		Real x_tip, x_tip_shaft, x_shaft_end, r_shaft, shaft_density;
		unsigned n_tip;
		//Real thread_circum_per_rad;
		Real thread_x_per_rad;
		Real thread_sphere_r,thread_sphere_density;
	public:
		ScrewGen();
		~ScrewGen(){};
		bool generate();
	protected:
		virtual void postProcessAttributes(bool deserializing){};
		void registerAttributes(){
			FileGenerator::registerAttributes();
			REGISTER_ATTRIBUTE(x_tip);
			REGISTER_ATTRIBUTE(x_tip_shaft);
			REGISTER_ATTRIBUTE(x_shaft_end);
			REGISTER_ATTRIBUTE(r_shaft);
			REGISTER_ATTRIBUTE(n_tip);
			REGISTER_ATTRIBUTE(shaft_density);
			REGISTER_ATTRIBUTE(thread_x_per_rad);
			REGISTER_ATTRIBUTE(thread_sphere_r);
			REGISTER_ATTRIBUTE(thread_sphere_density);
		};
	REGISTER_CLASS_NAME(ScrewGen);
	REGISTER_BASE_CLASS_NAME(FileGenerator);
};

REGISTER_SERIALIZABLE(ScrewGen,false);




