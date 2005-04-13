
#ifndef __HANGINGCLOTH_H__
#define __HANGINGCLOTH_H__

#include "FileGenerator.hpp"


class HangingCloth : public FileGenerator
{
	private : int width;
	private : int height;
	private : Real springStiffness ;
	private : Real springDamping ;	
	private : Real particleDamping ;
	private : Real clothMass;
	private : int cellSize;
	private : shared_ptr<Interaction>  spring;
	private : bool fixPoint1;
	private : bool fixPoint2;
	private : bool fixPoint3;
	private : bool fixPoint4;
	private : bool ground;
	private : Real dampingForce;
	private : Real sphereYoungModulus,spherePoissonRatio,sphereFrictionDeg;
// spheres
	private : Vector3r nbSpheres,gravity;
	private : Real density;
	private : Real minRadius;
	private : Real maxRadius;
	private : Real disorder;
	private : Real spacing;
	private : Real dampingMomentum;
	private : bool linkSpheres;

	// construction
	public : HangingCloth ();
	public : ~HangingCloth ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
	private : shared_ptr<Interaction>& createSpring(const shared_ptr<ComplexBody>& rootBody,int i,int j);
	private : void createBox(shared_ptr<Body>& body, Vector3r position, Vector3r extents);
	private : void createSphere(shared_ptr<Body>& body, int i, int j, int k);
	
	public : string generate();

	REGISTER_CLASS_NAME(HangingCloth);
};

REGISTER_SERIALIZABLE(HangingCloth,false);

#endif // __HANGINGCLOTH_H__

