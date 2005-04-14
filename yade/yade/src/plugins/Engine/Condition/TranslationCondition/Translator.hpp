#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include "KinematicMotion.hpp"

#ifdef WIN32
#include <windows.h>
#endif

#include "Vector3.hpp"

class Translator : public KinematicMotion
{

	public : Real velocity;
	public : Vector3r translationAxis;
	
	public : void moveToNextTimeStep(Body * body);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	REGISTER_CLASS_NAME(Translator);
};

REGISTER_SERIALIZABLE(Translator,false);

#endif // __TRANSLATOR_H__
