/***************************************************************************
 *   Copyright (C) 2005 by Olivier Galizzi   *
 *   olivier.galizzi@imag.fr   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SPHERE_HPP__
#define __SPHERE_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-wm3-math/Math.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-wm3-math/Quaternion.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct Sphere
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __SPHERE_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
