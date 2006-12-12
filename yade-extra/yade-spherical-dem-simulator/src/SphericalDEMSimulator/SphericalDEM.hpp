/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SPHERICALDEM_HPP
#define SPHERICALDEM_HPP

#include <Wm3Math.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <Wm3Quaternion.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

struct SphericalDEM
{
	bool isDynamic;

	/// Sphere
	Real radius;

	/// ParticleParameter-RigidBodyParameter-BodyMacroParameter
	Vector3r position;
	Quaternionr orientation;

	Vector3r velocity;
	Vector3r angularVelocity;

	Vector3r acceleration;
	Vector3r angularAcceleration;

	Real mass;
	Real invMass;
	Vector3r inertia;
	Vector3r invInertia;

	Real young;
	Real poisson;
	Real frictionAngle;
	
	/// LeapFrogPositionIntegrator
	Vector3r prevVelocity;
	
	/// LeapFrogOrientationIntegrator
	Vector3r prevAngularVelocity;

	Vector3r force;
	Vector3r momentum;
};

#endif // SPHERICALDEM_HPP

