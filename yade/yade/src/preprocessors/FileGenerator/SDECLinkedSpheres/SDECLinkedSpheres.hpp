
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "FileGenerator.hpp"

class SDECLinkedSpheres : public FileGenerator
{
	// construction
	public : SDECLinkedSpheres ();
	public : ~SDECLinkedSpheres ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : void generate();

	REGISTER_CLASS_NAME(SDECLinkedSpheres);
};

REGISTER_SERIALIZABLE(SDECLinkedSpheres,false);

#endif // __ROTATINGBOX_H__

