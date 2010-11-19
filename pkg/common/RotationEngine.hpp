// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
// © 2008 Václav Šmilauer <eudoxos@arcig.cz>
#pragma once

#include<yade/core/PartialEngine.hpp>
#include<yade/lib/base/Math.hpp>
#ifdef YADE_OPENMP
	#include<omp.h>
#endif

/*! Engine applying rotation (by setting angular velocity) to subscribed bodies.
 * If rotateAroundZero is set, then each body is also displaced around zeroPoint.
 */
class RotationEngine: public PartialEngine{
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(RotationEngine,PartialEngine,"Engine applying rotation (by setting angular velocity) to subscribed bodies. If rotateAroundZero is set, then each body is also displaced around zeroPoint.",
		((Real,angularVelocity,0,,"Angular velocity. [rad/s]"))
		((Vector3r,rotationAxis,Vector3r::UnitX(),,"Axis of rotation (direction); will be normalized automatically."))
		((bool,rotateAroundZero,false,,"If True, bodies will not rotate around their centroids, but rather around ``zeroPoint``."))
		((Vector3r,zeroPoint,Vector3r::Zero(),,"Point around which bodies will rotate if ``rotateAroundZero`` is True"))
	);
};
REGISTER_SERIALIZABLE(RotationEngine);

/* Engine applying both rotation and translation, along the same axis, whence the name HelixEngine
 */
class HelixEngine:public PartialEngine{
	public:
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(HelixEngine,PartialEngine,"Engine applying both rotation and translation, along the same axis, whence the name HelixEngine",
		((Real,angularVelocity,0,,"Angular velocity [rad/s]"))
		((Real,linearVelocity,0,,"Linear velocity [m/s]"))
		((Vector3r,axis,Vector3r::UnitX(),,"Axis of translation and rotation; will be normalized by the engine."))
		((Vector3r,axisPt,Vector3r::Zero(),,"A point on the axis, to position it in space properly."))
		((Real,angleTurned,0,,"How much have we turned so far. |yupdate| [rad]"))
	);
};
REGISTER_SERIALIZABLE(HelixEngine);

/*! Engine applying spiral motion, finding current angular velocity by linearly interpolating in
 * times and velocities and translation by using slope parameter.
 *
 * The interpolation assumes the margin value before the first time point and last value
 * after the last time point. If wrap is specified, time will wrap around the last times value to the first one (note that no interpolation
 * between last and first values is done).
 * */
class InterpolatingHelixEngine: public HelixEngine{
	//! holder of interpolation state, should not be touched by the user.
	size_t _pos;
	public:
		virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(InterpolatingHelixEngine,HelixEngine,"Engine applying spiral motion, finding current angular velocity by linearly interpolating in times and velocities and translation by using slope parameter. \n\n The interpolation assumes the margin value before the first time point and last value after the last time point. If wrap is specified, time will wrap around the last times value to the first one (note that no interpolation between last and first values is done).",
		((vector<Real>,times,,,"List of time points at which velocities are given; must be increasing [s]"))
		((vector<Real>,angularVelocities,,,"List of angular velocities; manadatorily of same length as times. [rad/s]"))
		((bool,wrap,false,,"Wrap t if t>times_n, i.e. t_wrapped=t-N*(times_n-times_0)"))
		((Real,slope,0,,"Axial translation per radian turn (can be negative) [m/rad]")),
		/*ctor*/ _pos=0;
	);
};
REGISTER_SERIALIZABLE(InterpolatingHelixEngine);

class HarmonicRotationEngine:public RotationEngine{
	public:
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS(HarmonicRotationEngine,RotationEngine,"This engine implements the harmonic-rotation oscillation of bodies. http://en.wikipedia.org/wiki/Simple_harmonic_motion#Dynamics_of_simple_harmonic_motion ; please, set dynamic=False for bodies, droven by this engine, otherwise amplitude will be 2x more, than awaited.",
		((Real,A,0,,"Amplitude [rad]"))
		((Real,f,0,,"Frequency [hertz]"))
		((Real,fi,Mathr::PI/2.0,,"Initial phase [radians]. By default, the body oscillates around initial position."))
	);
};
REGISTER_SERIALIZABLE(HarmonicRotationEngine);
