#ifndef __CLOTH_H__
#define __CLOTH_H__

#include "ConnexBody.hpp"
#include "Vector3.hpp"
#include "Matrix3.hpp"

class NodeProperties : public Serializable
{
	public : float invMass;
	public : Vector3 velocity;

	public : NodeProperties() {};
	public : NodeProperties(float im) : invMass(im), velocity(Vector3(0,0,0)) {};
	public : void processAttributes() {};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(invMass);
		REGISTER_ATTRIBUTE(velocity);
	};
	REGISTER_CLASS_NAME(NodeProperties);
};
REGISTER_CLASS(NodeProperties,true);

class Cloth : public ConnexBody
{	
	public : float stiffness;
	public : float damping;
	public : vector<NodeProperties> properties;
	
	// construction
	public : Cloth ();
	public : ~Cloth ();
	
	public : void processAttributes();
	public : void registerAttributes();
	
	public : void updateBoundingVolume(Se3& se3);
	public : void updateCollisionModel(Se3& se3);
	
	public : void moveToNextTimeStep(float dt);

	REGISTER_CLASS_NAME(Cloth);
};

REGISTER_CLASS(Cloth,false);

#endif // __CLOTH_H__
