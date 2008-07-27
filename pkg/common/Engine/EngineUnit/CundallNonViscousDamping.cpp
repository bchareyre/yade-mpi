// © 2004 Janek Kozicki <cosurgi@berlios.de>

#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include"CundallNonViscousDamping.hpp"

YADE_PLUGIN("CundallNonViscousForceDamping","CundallNonViscousMomentumDamping");

// this is Cundall non-viscous local damping, applied to force (Force)
void CundallNonViscousForceDamping::go(    const shared_ptr<PhysicalAction>& a
						, const shared_ptr<PhysicalParameters>& b
						, const Body* body)
{
	if(body->isClump()) return;
	Force * af = static_cast<Force*>(a.get());
	ParticleParameters * p = static_cast<ParticleParameters*>(b.get());
	
	for (int i=0; i<3; ++i)
	{
		af->force[i] *= 1 - damping*Mathr::Sign(af->force[i]*p->velocity[i]);	
	}
}

// this is Cundall non-viscous local damping, applied to momentum (Momentum)
void CundallNonViscousMomentumDamping::go( 	  const shared_ptr<PhysicalAction>& a
						, const shared_ptr<PhysicalParameters>& b
						, const Body* body)
{
	if(body->isClump()) return;
	Momentum * am = static_cast<Momentum*>(a.get());
	RigidBodyParameters * rb = static_cast<RigidBodyParameters*>(b.get());
	
	Vector3r& m  = am->momentum;
	
	for (int i=0; i<3; ++i){
		m[i] *= 1 - damping*Mathr::Sign(m[i]*rb->angularVelocity[i]);	
	}
}





