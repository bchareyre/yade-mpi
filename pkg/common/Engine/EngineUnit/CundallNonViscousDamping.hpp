// © 2004 Janek Kozicki <cosurgi@berlios.de>
#pragma once
#include<yade/pkg-common/PhysicalActionDamperUnit.hpp>

class CundallNonViscousForceDamping : public PhysicalActionDamperUnit{
	public:
		Real damping;
		CundallNonViscousForceDamping(): damping(0){};
		virtual void registerAttributes(){PhysicalActionDamperUnit::registerAttributes();REGISTER_ATTRIBUTE(damping);}
	virtual void go(const shared_ptr<PhysicalParameters>&, const Body*, MetaBody*);
	FUNCTOR1D(ParticleParameters);
	REGISTER_CLASS_AND_BASE(CundallNonViscousForceDamping,PhysicalActionDamperUnit);
};
REGISTER_SERIALIZABLE(CundallNonViscousForceDamping);

class CundallNonViscousMomentumDamping : public PhysicalActionDamperUnit{
	public:
		Real damping;
		CundallNonViscousMomentumDamping(): damping(0){};
		virtual void registerAttributes(){PhysicalActionDamperUnit::registerAttributes();REGISTER_ATTRIBUTE(damping); }
		virtual void go(const shared_ptr<PhysicalParameters>&, const Body*, MetaBody*);
	FUNCTOR1D(RigidBodyParameters);
	REGISTER_CLASS_AND_BASE(CundallNonViscousMomentumDamping,PhysicalActionDamperUnit);
};
REGISTER_SERIALIZABLE(CundallNonViscousMomentumDamping);


