#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "Particle.hpp"
#include "Matrix3.hpp"

class RigidBody : public Particle
{	
	public : Vector3r invInertia;
	public : Vector3r inertia;
	public : Vector3r angularAcceleration;
	public : Vector3r prevAngularAcceleration;

	// construction
	public : RigidBody ();
	public : ~RigidBody ();


	public : void moveToNextTimeStep();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(RigidBody);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_INDEX(RigidBody,SimpleBody);
	
};

REGISTER_SERIALIZABLE(RigidBody,false);

#endif // __RIGIDBODY_H__

