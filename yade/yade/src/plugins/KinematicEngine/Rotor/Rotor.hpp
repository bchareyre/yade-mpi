#ifndef __OSCILLATOR_H__
#define __OSCILLATOR_H__

#include "KinematicEngine.hpp"

#ifdef WIN32
#include <windows.h>
#endif

class Rotor : public KinematicEngine
{
	// construction
	public : Rotor ();
	public : virtual ~Rotor ();

	public : void moveToNextTimeStep(Body * body);

	public : void processAttributes();
	public : void registerAttributes();
	REGISTER_CLASS_NAME(Rotor);
};

REGISTER_SERIALIZABLE(Rotor,false);

#endif // __OSCILLATOR_H__
