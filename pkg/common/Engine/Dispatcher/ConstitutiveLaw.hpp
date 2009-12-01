// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Interaction.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/MetaBody.hpp>

class ConstitutiveLaw: public Functor2D <
		void, TYPELIST_4(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*, MetaBody*)
	>{
	public:
		ConstitutiveLaw(){}
		virtual ~ConstitutiveLaw();
	REGISTER_CLASS_AND_BASE(ConstitutiveLaw,Functor2D);
	/*! Convenience functions to get forces/torques quickly. */
	void addForce (const body_id_t id, const Vector3r& f,MetaBody* rb){rb->bex.addForce (id,f);}
	void addTorque(const body_id_t id, const Vector3r& t,MetaBody* rb){rb->bex.addTorque(id,t);}
	/*! Convenience function to apply force and torque from one force at contact point. Not sure if this is the right place for it. */
	void applyForceAtContactPoint(const Vector3r& force, const Vector3r& contactPoint, const body_id_t id1, const Vector3r& pos1, const body_id_t id2, const Vector3r& pos2, MetaBody* rb){
		addForce(id1,force,rb);
		addForce(id2,-force,rb);
		addTorque(id1,(contactPoint-pos1).Cross(force),rb);
		addTorque(id2,-(contactPoint-pos2).Cross(force),rb);
	}
	REGISTER_ATTRIBUTES(Functor,/* no attributes here */);
};
REGISTER_SERIALIZABLE(ConstitutiveLaw);
	
