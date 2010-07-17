// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Interaction.hpp>
#include<yade/core/Functor.hpp>
#include<yade/core/Scene.hpp>

class LawFunctor: public Functor2D <
		void, TYPELIST_3(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*)
	>{
	public:
		virtual ~LawFunctor();
	/*! Convenience functions to get forces/torques quickly. */
	void addForce (const body_id_t id, const Vector3r& f,Scene* rb){rb->forces.addForce (id,f);}
	void addTorque(const body_id_t id, const Vector3r& t,Scene* rb){rb->forces.addTorque(id,t);}
	/*! Convenience function to apply force and torque from one force at contact point. Not sure if this is the right place for it. */
	void applyForceAtContactPoint(const Vector3r& force, const Vector3r& contactPoint, const body_id_t id1, const Vector3r& pos1, const body_id_t id2, const Vector3r& pos2){
		addForce(id1,force,scene);
		addForce(id2,-force,scene);
		addTorque(id1,(contactPoint-pos1).cross(force),scene);
		addTorque(id2,-(contactPoint-pos2).cross(force),scene);
	}
	YADE_CLASS_BASE_DOC(LawFunctor,Functor,"Functor for applying constitutive laws on :yref:`interactions<Interaction>`.");
};
REGISTER_SERIALIZABLE(LawFunctor);
	
