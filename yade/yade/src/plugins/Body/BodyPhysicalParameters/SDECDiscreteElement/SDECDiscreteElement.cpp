#include "SDECDiscreteElement.hpp"

SDECDiscreteElement::SDECDiscreteElement () : RigidBody()
{
	createIndex();
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

