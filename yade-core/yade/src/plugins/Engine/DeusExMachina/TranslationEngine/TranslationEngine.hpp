#ifndef __TRANSLATOR_H__
#define __TRANSLATOR_H__

#include <yade/DeusExMachina.hpp>

#ifdef WIN32
#include <windows.h>
#endif

#include <yade-lib-wm3-math/Vector3.hpp>

class TranslationEngine : public DeusExMachina
{

	public : Real velocity;
	public : Vector3r translationAxis;
	
	public : void applyCondition(Body * body);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	REGISTER_CLASS_NAME(TranslationEngine);
};

REGISTER_SERIALIZABLE(TranslationEngine,false);

#endif // __TRANSLATOR_H__
