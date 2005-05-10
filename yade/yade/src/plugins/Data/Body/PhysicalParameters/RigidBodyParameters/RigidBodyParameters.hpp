#ifndef __RIGIDBODY_H__
#define __RIGIDBODY_H__

#include "ParticleParameters.hpp"
#include <yade-lib-wm3-math/Matrix3.hpp>

class RigidBodyParameters : public ParticleParameters
{	
	public : Vector3r invInertia;
	public : Vector3r inertia;
	public : Vector3r angularAcceleration;
	public : Vector3r angularVelocity;
	
	public : RigidBodyParameters ();
	public : virtual ~RigidBodyParameters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(RigidBodyParameters);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_INDEX(RigidBodyParameters,ParticleParameters);
	
};

REGISTER_SERIALIZABLE(RigidBodyParameters,false);

#endif // __RIGIDBODY_H__

