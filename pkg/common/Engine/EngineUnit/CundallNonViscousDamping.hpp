// © 2004 Janek Kozicki <cosurgi@berlios.de>
#pragma once
#include<yade/pkg-common/PhysicalActionDamperUnit.hpp>

class CundallNonViscousForceDamping : public PhysicalActionDamperUnit{
	public:
		Real damping;
		CundallNonViscousForceDamping(): damping(0){};
		virtual void go(const shared_ptr<PhysicalAction>&, const shared_ptr<PhysicalParameters>& , const Body*);
	protected:
		virtual void registerAttributes(){
			PhysicalActionDamperUnit::registerAttributes();
			REGISTER_ATTRIBUTE(damping);
		}
	NEEDS_BEX("Force");
	FUNCTOR2D(Force,ParticleParameters);
	REGISTER_CLASS_NAME(CundallNonViscousForceDamping);
	REGISTER_BASE_CLASS_NAME(PhysicalActionDamperUnit);
};
REGISTER_SERIALIZABLE(CundallNonViscousForceDamping,false);

class CundallNonViscousMomentumDamping : public PhysicalActionDamperUnit{
	public:
		Real damping;
		CundallNonViscousMomentumDamping(): damping(0){};
		virtual void go(const shared_ptr<PhysicalAction>&, const shared_ptr<PhysicalParameters>&, const Body*);
	protected:
		 virtual void registerAttributes(){
			PhysicalActionDamperUnit::registerAttributes();
			REGISTER_ATTRIBUTE(damping);
		 }
	NEEDS_BEX("Momentum");
	FUNCTOR2D(Momentum,RigidBodyParameters);
	REGISTER_CLASS_NAME(CundallNonViscousMomentumDamping);
	REGISTER_BASE_CLASS_NAME(PhysicalActionDamperUnit);
};
REGISTER_SERIALIZABLE(CundallNonViscousMomentumDamping,false);


