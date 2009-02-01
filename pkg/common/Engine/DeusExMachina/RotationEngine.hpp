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
		//! axis of rotation (direction); will be normalized by the engine
		Vector3r rotationAxis;
		bool rotateAroundZero;
		Vector3r zeroPoint;

		void applyCondition(MetaBody * );

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(RotationEngine);
	REGISTER_BASE_CLASS_NAME(DeusExMachina);
};
REGISTER_SERIALIZABLE(RotationEngine);

/* Engine applying both rotation and translation, along the same axis, whence the name SpiralEngine
 */
class SpiralEngine:public DeusExMachina{
	public:
		SpiralEngine():angularVelocity(0.),linearVelocity(0.),axis(Vector3r::UNIT_X),axisPt(0,0,0){}
		Real angularVelocity;
		Real linearVelocity;
		//! axis of translation and rotation (direction); will be normalized by the engine
		Vector3r axis;
		//! a point on the axis, to position it in space properly
		Vector3r axisPt;
	virtual void applyCondition(MetaBody*);
	REGISTER_CLASS_AND_BASE(SpiralEngine,DeusExMachina);
	REGISTER_ATTRIBUTES(DeusExMachina,(angularVelocity)(linearVelocity)(axis)(axisPt));
};
REGISTER_SERIALIZABLE(SpiralEngine);

/*! Engine applying spiral motion, finding current angular velocity by linearly interpolating in
 * times and velocities and translation by using slope parameter.
 *
 * The interpolation assumes the margin value before the first time point and last value
 * after the last time point. If wrap is specified, time will wrap around the last times value to the first one (note that no interpolation
 * between last and first values is done).
 * */
class InterpolatingSpiralEngine: public SpiralEngine{
	public:
		//! list of times at which velocities are given; must be increasing
		vector<Real> times;
		//! list of angular velocities; manadatorily of same length as times
		vector<Real> angularVelocities;
		//! wrap t if t>times_n, i.e. t_wrapped=t-N*(times_n-times_0)
		bool wrap;
		//! axial translation per radian turn (can be negative)
		Real slope;
		//! holder of interpolation state, should not be touched by the user.
		size_t pos;
		InterpolatingSpiralEngine(): wrap(false), slope(0), pos(0){}
		virtual void applyCondition(MetaBody* rb);
	REGISTER_CLASS_AND_BASE(InterpolatingSpiralEngine,SpiralEngine);
	REGISTER_ATTRIBUTES(SpiralEngine,(times)(angularVelocities)(wrap)(slope)(pos));
};
REGISTER_SERIALIZABLE(InterpolatingSpiralEngine);




