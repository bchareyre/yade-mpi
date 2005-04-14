#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include "Condition.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include "Vector3.hpp"

class RotationCondition : public Condition
{
	public : RotationCondition();

	public : Real angularVelocity;
	public : Vector3r rotationAxis;
	public : bool rotateAroundZero;

	public : void applyCondition(Body * body);

	public : void registerAttributes();
	REGISTER_CLASS_NAME(RotationCondition);
};

REGISTER_SERIALIZABLE(RotationCondition,false);

#endif // __OSCILLATOR_H__
