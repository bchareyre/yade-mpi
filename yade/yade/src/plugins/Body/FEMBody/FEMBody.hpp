#ifndef __CLOTH_H__
#define __CLOTH_H__

#include "ConnexBody.hpp"
#include "Vector3.hpp"
#include "Matrix3.hpp"

// FIXME - class class NodeProperties is duplicated in FEMBody and MassSpringBody

class NodeProperties : public Serializable
{
	public : float invMass;
	public : Vector3r velocity;

	public : NodeProperties() {};
	public : NodeProperties(float im) : invMass(im), velocity(Vector3r(0,0,0)) {};
	protected : virtual void postProcessAttributes(bool) {};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(invMass);
		REGISTER_ATTRIBUTE(velocity);
	};
	REGISTER_CLASS_NAME(NodeProperties);
};

REGISTER_SERIALIZABLE(NodeProperties,false);

class FEMBody : public ConnexBody
{
	public : float stiffness;
	public : float damping;
 	public : vector<NodeProperties> properties;
	public : vector<float> initialLengths;
	public : vector<pair<int,Vector3r> > externalForces;

	// construction
	public : FEMBody ();
	public : ~FEMBody ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : void updateBoundingVolume(Se3r& se3);
	public : void updateCollisionGeometry(Se3r& se3);

	REGISTER_CLASS_NAME(FEMBody);
};

REGISTER_SERIALIZABLE(FEMBody,false);

#endif // __CLOTH_H__
