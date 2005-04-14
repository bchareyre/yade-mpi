#include "BodyMacroParameters.hpp"

BodyMacroParameters::BodyMacroParameters () : RigidBodyParameters()
{
	createIndex();
}

BodyMacroParameters::~BodyMacroParameters()
{
}

void BodyMacroParameters::registerAttributes()
{
	RigidBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(young);
	REGISTER_ATTRIBUTE(poisson);
	REGISTER_ATTRIBUTE(frictionAngle);
}

