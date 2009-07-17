// © 2004 Janek Kozicki <cosurgi@berlios.de>
#pragma once
#include<yade/pkg-common/PhysicalActionDamperUnit.hpp>

class CundallNonViscousForceDamping : public PhysicalActionDamperUnit{
	public:
		Real damping;
		CundallNonViscousForceDamping(): damping(0){};
		virtual void go(const shared_ptr<PhysicalParameters>&, const Body*, MetaBody*);
	REGISTER_ATTRIBUTES(PhysicalActionDamperUnit,(damping));
	FUNCTOR1D(ParticleParameters);
	REGISTER_CLASS_AND_BASE(CundallNonViscousForceDamping,PhysicalActionDamperUnit);
};
REGISTER_SERIALIZABLE(CundallNonViscousForceDamping);

class CundallNonViscousMomentumDamping : public PhysicalActionDamperUnit{
	public:
		Real damping;
		CundallNonViscousMomentumDamping(): damping(0){};
		virtual void go(const shared_ptr<PhysicalParameters>&, const Body*, MetaBody*);
	REGISTER_ATTRIBUTES(PhysicalActionDamperUnit,(damping));
	FUNCTOR1D(RigidBodyParameters);
	REGISTER_CLASS_AND_BASE(CundallNonViscousMomentumDamping,PhysicalActionDamperUnit);
};
REGISTER_SERIALIZABLE(CundallNonViscousMomentumDamping);


