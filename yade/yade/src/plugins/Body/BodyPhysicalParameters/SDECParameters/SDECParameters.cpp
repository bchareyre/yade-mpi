#include "SDECParameters.hpp"

SDECParameters::SDECParameters () : RigidBodyParameters()
{
	createIndex();
}

SDECParameters::~SDECParameters()
{
}

void SDECParameters::registerAttributes()
{
	RigidBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(young);
	REGISTER_ATTRIBUTE(poisson);
	REGISTER_ATTRIBUTE(frictionAngle);
}

