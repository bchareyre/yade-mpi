
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "Serializable.hpp"

class SDECLinkedSpheres : public Serializable
{
	// construction
	public : SDECLinkedSpheres ();
	public : ~SDECLinkedSpheres ();

	public : void processAttributes();
	public : void registerAttributes();

	public : void exec();

	REGISTER_CLASS_NAME(SDECLinkedSpheres);
};

REGISTER_SERIALIZABLE(SDECLinkedSpheres,false);

#endif // __ROTATINGBOX_H__

