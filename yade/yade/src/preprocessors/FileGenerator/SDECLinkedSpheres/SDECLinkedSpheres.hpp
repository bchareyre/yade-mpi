
#ifndef __LINKEDSPHERES_H__
#define __LINKEDSPHERES_H__

#include "FileGenerator.hpp"
#include "Vector3.hpp"
#include "SimpleBody.hpp"

class SDECLinkedSpheres : public FileGenerator
{
	private	: Vector3r nbSpheres; 

	private	: Real minRadius;
	private	: Real maxRadius;
	private	: Real disorder;
	private	: Real spacing;
	private	: Real supportSize;
	private	: bool support1;
	private	: bool support2;
	private	: Real kn;
	private	: Real ks;
	private : Real dampingForce;
	private : Real dampingMomentum;
	
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
	private : void createSphere(shared_ptr<Body>& body, int i, int j, int k);
	
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

