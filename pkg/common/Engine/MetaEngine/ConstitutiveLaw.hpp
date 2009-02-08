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
	/* Convenience functions to get forces/torques quickly.
	 * They are not strictly necessary and for simulation that have nodes with only translational DOFs,
	 * It creates no overhead runtime overhead, since intialization of Forces/Momentums
	 * is done only if the derived ConstitutiveLaw says NEEDS_BEX("Force","Momentum"), for example.
	 */
	Vector3r& bodyForce(body_id_t id, MetaBody* rb){ return static_pointer_cast<Force>(rb->physicalActions->find(id,forceIdx))->force; }
	Vector3r& bodyTorque(body_id_t id, MetaBody* rb){ return static_pointer_cast<Momentum>(rb->physicalActions->find(id,torqueIdx))->momentum;}
};
REGISTER_SERIALIZABLE(ConstitutiveLaw);
	
