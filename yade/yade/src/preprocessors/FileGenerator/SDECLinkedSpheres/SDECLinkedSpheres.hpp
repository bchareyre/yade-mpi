
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "FileGenerator.hpp"

class SDECLinkedSpheres : public FileGenerator
{
	private	: int nbSpheresX;
	private	: int nbSpheresY;
	private	: int nbSpheresZ;
	private	: float minRadius;
	private	: float maxRadius;
	private	: float disorder;
	private	: float spacing;
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

