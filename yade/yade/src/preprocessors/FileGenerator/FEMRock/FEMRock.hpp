
#ifndef __FEMROCK_H__
#define __FEMROCK_H__

#include "FileGenerator.hpp"
#include "Vector3.hpp"
/*class NodeProperties : public Serializable
{
	public : float invMass;
	public : Vector3r velocity;

	public : NodeProperties() {};
	public : NodeProperties(float im) : invMass(im), velocity(Vector3r(0,0,0)) {};
	protected : virtual void postProcessAttributes(bool deserializing) {};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(invMass);
		REGISTER_ATTRIBUTE(velocity);
	};
	REGISTER_CLASS_NAME(NodeProperties);
};
REGISTER_CLASS(NodeProperties,false);*/

class FEMRock : public FileGenerator
{
	public : vector<Vector3r> nodes;
	public : vector<vector<int> > tetrahedrons;

	// construction
	public : FEMRock ();
	public : ~FEMRock ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	public : string generate();

	REGISTER_CLASS_NAME(FEMRock);
};

REGISTER_SERIALIZABLE(FEMRock,false);

#endif // __FEMROCK_H__

