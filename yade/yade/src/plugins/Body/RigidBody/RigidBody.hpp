#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "ConnexBody.hpp"
#include "Vector3.hpp"
#include "Matrix3.hpp"

class RigidBody : public ConnexBody
{
	public : Vector3r invInertia;
	public : Vector3r inertia;
	public : Vector3r acceleration;
	public : Vector3r prevAcceleration;
	public : Vector3r angularAcceleration;
	public : Vector3r prevAngularAcceleration;

	// construction
	public : RigidBody ();
	public : ~RigidBody ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : void updateBoundingVolume(Se3r& se3);
	public : void updateCollisionGeometry(Se3r& se3);

	public : void moveToNextTimeStep();

	REGISTER_CLASS_NAME(RigidBody);
	//REGISTER_CLASS_INDEX(RigidBody);
};

REGISTER_SERIALIZABLE(RigidBody,false);

#endif // __RIGIDBODY_H__

