
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "FileGenerator.hpp"

class SDECSpheresPlane : public FileGenerator
{
	// construction
	public : SDECSpheresPlane ();
	public : ~SDECSpheresPlane ();

	public : void afterDeserialization();
	public : void registerAttributes();

	public : void generate();

	REGISTER_CLASS_NAME(SDECSpheresPlane);
};

REGISTER_SERIALIZABLE(SDECSpheresPlane,false);

#endif // __ROTATINGBOX_H__

