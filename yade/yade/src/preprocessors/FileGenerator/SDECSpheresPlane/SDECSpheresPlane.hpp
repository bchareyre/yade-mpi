
#ifndef __ROTATINGBOX_H__
#define __ROTATINGBOX_H__

#include "FileGenerator.hpp"

class SDECSpheresPlane : public FileGenerator
{
	private : int nbSpheres;
	private : Real minRadius;
	private : Real kn;
	private : Real ks;
	private : Vector3r groundSize;
	private : Real maxRadius;
	private : Real dampingForce;
	private : Real dampingMomentum;
	// construction
	public : SDECSpheresPlane ();
	public : ~SDECSpheresPlane ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(SDECSpheresPlane);
};

REGISTER_SERIALIZABLE(SDECSpheresPlane,false);

#endif // __ROTATINGBOX_H__

