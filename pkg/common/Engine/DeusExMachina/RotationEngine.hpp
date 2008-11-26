// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/DeusExMachina.hpp>
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

/*! Engine applying rotation (by setting angular velocity) to subscribed bodies.
 * If rotateAroundZero is set, then each body is also displaced around zeroPoint.
 */
class RotationEngine : public DeusExMachina
{
	public :
		RotationEngine();

		Real angularVelocity;
		Vector3r rotationAxis;
		bool rotateAroundZero;
		Vector3r zeroPoint;

		void applyCondition(MetaBody * );

	protected :
		void registerAttributes();
		void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(RotationEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(RotationEngine);

/*! Engine applying rotation, finding current angular velocity by interpolating in times and velocities */
class InterpolatingRotationEngine: public RotationEngine{
	public:
		//! list of times at which velocities are given; must be increasing
		vector<Real> times;
		//! list of angular velocities
		vector<Real> velocities;
		//! holder of interpolation state, should not be touched by the user.
		size_t pos;
		InterpolatingRotationEngine(){pos=0;}
		void registerAttributes(){ RotationEngine::registerAttributes(); REGISTER_ATTRIBUTE(times); REGISTER_ATTRIBUTE(velocities); REGISTER_ATTRIBUTE(pos); }
		void applyCondition(MetaBody* rb);
	REGISTER_CLASS_NAME(InterpolatingRotationEngine);
	REGISTER_BASE_CLASS_NAME(RotationEngine);
};
REGISTER_SERIALIZABLE(InterpolatingRotationEngine);


