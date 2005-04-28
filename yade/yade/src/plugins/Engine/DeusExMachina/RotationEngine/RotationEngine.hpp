#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include "DeusExMachina.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include "Vector3.hpp"

class RotationEngine : public DeusExMachina
{
	public : RotationEngine();

	public : Real angularVelocity;
	public : Vector3r rotationAxis;
	public : bool rotateAroundZero;

	public : void applyCondition(Body * body);

	public : void registerAttributes();
	REGISTER_CLASS_NAME(RotationEngine);
};

REGISTER_SERIALIZABLE(RotationEngine,false);

#endif // __OSCILLATOR_H__
