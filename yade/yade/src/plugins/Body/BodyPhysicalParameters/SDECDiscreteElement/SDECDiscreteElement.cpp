#include "SDECDiscreteElement.hpp"

SDECDiscreteElement::SDECDiscreteElement () : RigidBody()
{
	createIndex(); // FIXME - verify that it works, when RigidBody has also called createIndex()
}


SDECDiscreteElement::~SDECDiscreteElement()
{

}

void SDECDiscreteElement::postProcessAttributes(bool deserializing)
{
	RigidBody::postProcessAttributes(deserializing);
}

void SDECDiscreteElement::registerAttributes()
{
	RigidBody::registerAttributes();
	REGISTER_ATTRIBUTE(kn);
	REGISTER_ATTRIBUTE(ks);
}


/*
void SDECDiscreteElement::moveToNextTimeStep()
{	
	RigidBody::moveToNextTimeStep();
}*/
