#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "ConnexBody.hpp"
#include "Vector3.hpp"
#include "Matrix3.hpp"

class RigidBody : public ConnexBody
{	
	public : Vector3 invInertia;
	public : Vector3 inertia;

	// construction
	public : RigidBody ();
	public : ~RigidBody ();
	
	public : void processAttributes();
	public : void registerAttributes();
	
	public : void updateBoundingVolume(Se3& se3);
	public : void updateCollisionModel(Se3& se3);
	
	public : void moveToNextTimeStep(float dt);
	REGISTER_CLASS_NAME(RigidBody);
};

REGISTER_CLASS(RigidBody,false);

#endif // __RIGIDBODY_H__
