
#ifndef __LINKEDSPHERES_H__
#define __LINKEDSPHERES_H__

#include "FileGenerator.hpp"
#include "Vector3.hpp"

class SDECLinkedSpheres : public FileGenerator
{
	// FIXME - if it is Vector3<int> nbSpheres; it crashes:
	//
	//		terminate called after throwing an instance of 'SerializableError'
	//		what():  Cannot determine type with findType()

	private	: Vector3r nbSpheres; // Vector3<int> nbSpheres; FIXME

	private	: Real minRadius;
	private	: Real maxRadius;
	private	: Real disorder;
	private	: Real spacing;
	private	: Real supportSize;
	private	: bool support1;
	private	: bool support2;
	private	: Real kn;
	private	: Real ks;
	// construction
	public : SDECLinkedSpheres ();
	public : ~SDECLinkedSpheres ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(SDECLinkedSpheres);
};

REGISTER_SERIALIZABLE(SDECLinkedSpheres,false);

#endif // __LINKEDSPHERES_H__

