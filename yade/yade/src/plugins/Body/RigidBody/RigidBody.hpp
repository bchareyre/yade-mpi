#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "ConnexBody.hpp"
#include "Vector3.hpp"
#include "Matrix3.hpp"

class RigidBody : public ConnexBody
{
	public : Vector3 invInertia;
	public : Vector3 inertia;
	public : Vector3 acceleration;
	public : Vector3 prevAcceleration;
	public : Vector3 angularAcceleration;
	public : Vector3 prevAngularAcceleration;

	// construction
	public : RigidBody ();
	public : ~RigidBody ();

	public : void processAttributes();
	public : void registerAttributes();

	public : void updateBoundingVolume(Se3& se3);
	public : void updateCollisionModel(Se3& se3);

	public : void moveToNextTimeStep();

	REGISTER_CLASS_NAME(RigidBody);
	//REGISTER_CLASS_INDEX(RigidBody);
};

REGISTER_SERIALIZABLE(RigidBody,false);

#endif // __RIGIDBODY_H__

