
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "Serializable.hpp"

class SDECSpheresPlane : public Serializable
{
	// construction
	public : SDECSpheresPlane ();
	public : ~SDECSpheresPlane ();

	public : void afterDeserialization();
	public : void registerAttributes();

	public : void exec();

	REGISTER_CLASS_NAME(SDECSpheresPlane);
};

REGISTER_SERIALIZABLE(SDECSpheresPlane,false);

#endif // __ROTATINGBOX_H__

