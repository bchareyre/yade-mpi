#ifndef __MASSPSRINGBODY_H__
#define __MASSPSRINGBODY_H__

#include "ConnexBody.hpp"
#include "Vector3.hpp"
#include "Matrix3.hpp"

// FIXME - class class NodeProperties is duplicated in FEMBody and MassSpringBody

class NodeProperties : public Serializable
{
	public : Real invMass;
	public : Vector3r velocity;

	public : NodeProperties() {};
	public : NodeProperties(Real im) : invMass(im), velocity(Vector3r(0,0,0)) {};
	protected : virtual void postProcessAttributes(bool) {};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(invMass);
		REGISTER_ATTRIBUTE(velocity);
	};
	REGISTER_CLASS_NAME(NodeProperties);
};

REGISTER_SERIALIZABLE(NodeProperties,true);

class MassSpringBody : public ConnexBody
{
	public : Real stiffness;
	public : Real damping;
 	public : vector<NodeProperties> properties;
	public : vector<Real> initialLengths;
	public : vector<pair<int,Vector3r> > externalForces;

	// construction
	public : MassSpringBody ();
	public : ~MassSpringBody ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	REGISTER_CLASS_NAME(MassSpringBody);
};

REGISTER_SERIALIZABLE(MassSpringBody,false);

#endif // __MASSPSRINGBODY_H__
