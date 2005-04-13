
#ifndef __BOXSTACK_H__
#define __BOXSTACK_H__

#include "FileGenerator.hpp"

class BoxStack : public FileGenerator
{
	private : Vector3r nbBoxes;
	private : Vector3r boxSize,bulletPosition;
	private : Real boxDensity;
	private : Real bulletSize;
	private : Real bulletDensity;
	private : Vector3r bulletVelocity,gravity;
	private : bool kinematicBullet;
	private : Real dampingForce;
	private : Real dampingMomentum;
	
	// construction
	public : BoxStack ();
	public : ~BoxStack ();

	public : virtual void registerAttributes();

	private : void createKinematicBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents,bool);
	private : void createSphere(shared_ptr<Body>& body);
	private : void createBox(shared_ptr<Body>& body, int i, int j, int k);
	private : void createActors(shared_ptr<ComplexBody>& rootBody);
	private : void positionRootBody(shared_ptr<ComplexBody>& rootBody);
	
	public : virtual string generate();

	REGISTER_CLASS_NAME(BoxStack);
};

REGISTER_SERIALIZABLE(BoxStack,false);

#endif // __BOXSTACK_H__

