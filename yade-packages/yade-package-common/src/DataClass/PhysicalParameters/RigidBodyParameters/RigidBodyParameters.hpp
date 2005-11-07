/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef RIGIDBODYPARAMETERS_HPP
#define RIGIDBODYPARAMETERS_HPP

#include "ParticleParameters.hpp"
#include <yade/yade-lib-wm3-math/Matrix3.hpp>

class RigidBodyParameters : public ParticleParameters
{	
	public :
		Vector3r	 invInertia
				,inertia
				,angularAcceleration
				,angularVelocity;
	
		RigidBodyParameters ();
		virtual ~RigidBodyParameters ();

/// Serialization										///
	protected :
		virtual void postProcessAttributes(bool deserializing);
		void registerAttributes();
	REGISTER_CLASS_NAME(RigidBodyParameters);
	REGISTER_BASE_CLASS_NAME(ParticleParameters);
	
/// Indexable											///
	REGISTER_CLASS_INDEX(RigidBodyParameters,ParticleParameters);
};

REGISTER_SERIALIZABLE(RigidBodyParameters,false);

#endif // RIGIDBODYPARAMETERS_HPP

