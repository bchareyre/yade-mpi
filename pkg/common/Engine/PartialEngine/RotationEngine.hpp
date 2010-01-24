// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/PartialEngine.hpp>
#include<Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

/*! Engine applying rotation (by setting angular velocity) to subscribed bodies.
 * If rotateAroundZero is set, then each body is also displaced around zeroPoint.
 */
class RotationEngine : public PartialEngine
{
	public:
		RotationEngine();
		Real angularVelocity;
		Vector3r rotationAxis;
		bool rotateAroundZero;
		Vector3r zeroPoint;
		void applyCondition(Scene * );
	YADE_CLASS_BASE_DOC_ATTRS(RotationEngine,PartialEngine,"Engine applying rotation (by setting angular velocity) to subscribed bodies. If rotateAroundZero is set, then each body is also displaced around zeroPoint.",
		((angularVelocity,"Angular velocity. [rad/s]"))
		((rotationAxis,"Axis of rotation (direction); will be normalized automatically."))
		((rotateAroundZero,"If True, bodies will not rotate around their centroids, but rather around ``zeroPoint``."))
		((zeroPoint,"Point around which bodies will rotate if ``rotateAroundZero`` is True"))
	);
};
REGISTER_SERIALIZABLE(RotationEngine);

/* Engine applying both rotation and translation, along the same axis, whence the name SpiralEngine
 */
class SpiralEngine:public PartialEngine{
	public:
		SpiralEngine():angularVelocity(0.),linearVelocity(0.),axis(Vector3r::UNIT_X),axisPt(0,0,0),angleTurned(0.){}
		Real angularVelocity;
		Real linearVelocity;
		Vector3r axis;
		Vector3r axisPt;
		Real angleTurned;
	virtual void applyCondition(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS(SpiralEngine,PartialEngine,"Engine applying both rotation and translation, along the same axis, whence the name SpiralEngine",
		((angularVelocity,"Angular velocity [rad/s]"))
		((linearVelocity,"Linear velocity [m/s]"))
		((axis,"Axis of translation and rotation; will be normalized by the engine."))
		((axisPt,"A point on the axis, to position it in space properly."))
		((angleTurned,"How much have we turned so far. |yupdate| [rad]"))
	)
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
	//! holder of interpolation state, should not be touched by the user.
	size_t _pos;
	public:
		vector<Real> times;
		vector<Real> angularVelocities;
		bool wrap;
		Real slope;
		InterpolatingSpiralEngine(): _pos(0), wrap(false), slope(0){}
		virtual void applyCondition(Scene* rb);
	YADE_CLASS_BASE_DOC_ATTRS(InterpolatingSpiralEngine,SpiralEngine,"Engine applying spiral motion, finding current angular velocity by linearly interpolating in times and velocities and translation by using slope parameter. \n\n The interpolation assumes the margin value before the first time point and last value after the last time point. If wrap is specified, time will wrap around the last times value to the first one (note that no interpolation between last and first values is done).",
		((times,"List of time points at which velocities are given; must be increasing [s]"))
		((angularVelocities,"List of angular velocities; manadatorily of same length as times. [rad/s]"))
		((wrap,"Wrap t if t>times_n, i.e. t_wrapped=t-N*(times_n-times_0)"))
		((slope,"Axial translation per radian turn (can be negative) [m/rad]"))
	);
};
REGISTER_SERIALIZABLE(InterpolatingSpiralEngine);




