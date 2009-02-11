// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Interaction.hpp>
#include<yade/core/EngineUnit2D.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>

class ConstitutiveLaw: public EngineUnit2D <
		void, TYPELIST_4(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*, MetaBody*)
	>{
		int forceIdx, torqueIdx;
	public:
		ConstitutiveLaw(){
			// cache force/torque indices for fast access in bodyForce and bodyTorque
			forceIdx=shared_ptr<PhysicalAction>(new Force())->getClassIndex();
			torqueIdx=shared_ptr<PhysicalAction>(new Momentum())->getClassIndex();
		}
	REGISTER_CLASS_AND_BASE(ConstitutiveLaw,EngineUnit2D);
	/*! Convenience functions to get forces/torques quickly.
	 * They are not strictly necessary and for simulation that have nodes with only translational DOFs,
	 * It creates no overhead runtime overhead, since intialization of Forces/Momentums
	 * is done only if the derived ConstitutiveLaw says NEEDS_BEX("Force","Momentum"), for example.
	 */
	Vector3r& bodyForce(const body_id_t id, MetaBody* rb) const { return static_pointer_cast<Force>(rb->physicalActions->find(id,forceIdx))->force; }
	Vector3r& bodyTorque(const body_id_t id, MetaBody* rb) const { return static_pointer_cast<Momentum>(rb->physicalActions->find(id,torqueIdx))->momentum;}
	/*! Convenience function to apply force and torque from one force at contact point. Not sure if this is the right place for it. */
	void applyForceAtContactPoint(const Vector3r& force, const Vector3r& contactPoint, const body_id_t id1, const Vector3r& pos1, const body_id_t id2, const Vector3r& pos2, MetaBody* rb){
		bodyForce(id1,rb)+=force; bodyForce(id2,rb)-=force;
		bodyTorque(id1,rb)+=(contactPoint-pos1).Cross(force);
		bodyTorque(id2,rb)-=(contactPoint-pos2).Cross(force);
	}
};
REGISTER_SERIALIZABLE(ConstitutiveLaw);
	
