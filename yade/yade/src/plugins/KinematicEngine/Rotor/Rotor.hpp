#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include "KinematicEngine.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include "Vector3.hpp"

class Rotor : public KinematicEngine
{

	public : float angularVelocity;
	public : Vector3r rotationAxis;
	// construction
	public : Rotor ();
	public : virtual ~Rotor ();

	public : void moveToNextTimeStep(Body * body);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	REGISTER_CLASS_NAME(Rotor);
};

REGISTER_SERIALIZABLE(Rotor,false);

#endif // __OSCILLATOR_H__
