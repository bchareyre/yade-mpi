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
#include <Wm3Matrix3.h>
#include<yade/lib-base/yadeWm3.hpp>

class RigidBodyParameters : public ParticleParameters
{	
	public :
		// parameters
		Vector3r	 inertia
		// state
				,angularAcceleration
				,angularVelocity;

		/// It is the rotation center of kinematic (non-isDymanic) body. 
		/// It is non-serializable and must be set by kinematic Engine. 
		Vector3r zeroPoint;
	
		RigidBodyParameters ();
		virtual ~RigidBodyParameters ();

/// Serialization										///
	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(RigidBodyParameters);
	REGISTER_BASE_CLASS_NAME(ParticleParameters);
	
/// Indexable											///
	REGISTER_CLASS_INDEX(RigidBodyParameters,ParticleParameters);
};

REGISTER_SERIALIZABLE(RigidBodyParameters);

#endif // RIGIDBODYPARAMETERS_HPP

