// © 2004 Janek Kozicki <cosurgi@berlios.de>

#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
#include"CundallNonViscousDamping.hpp"

YADE_PLUGIN("CundallNonViscousForceDamping","CundallNonViscousMomentumDamping");

//! damping of force, for bodies that have only ParticleParameters
void CundallNonViscousForceDamping::go(const shared_ptr<PhysicalParameters>& pp, const Body* body, MetaBody* rb){
	if(body->isClump()) return;
	Vector3r f=getForceUnsynced(body->getId(),rb);
	ParticleParameters *p=static_cast<ParticleParameters*>(pp.get());
	Vector3r df=Vector3r::ZERO;
	for(int i=0; i<3; i++){df[i]=-f[i]*damping*Mathr::Sign(f[i]*p->velocity[i]);}
	rb->bex.addForce(body->getId(),df);
}
//! damping of both force and torque, for bodies that have RigidBodyParameters
void CundallNonViscousMomentumDamping::go(const shared_ptr<PhysicalParameters>& pp, const Body* body, MetaBody* rb){
	if(body->isClump()) return;
	body_id_t id=body->getId();
	Vector3r f=getForceUnsynced(id,rb),t=getTorqueUnsynced(id,rb);
	RigidBodyParameters *p=static_cast<RigidBodyParameters*>(pp.get());
	Vector3r df=Vector3r::ZERO, dt=Vector3r::ZERO;
	for(int i=0; i<3; i++){
		df[i]=-f[i]*damping*Mathr::Sign(f[i]*p->velocity[i]);
		dt[i]=-t[i]*damping*Mathr::Sign(t[i]*p->angularVelocity[i]);
	}
	rb->bex.addForce(id,df); rb->bex.addTorque(id,dt);
}
