#include "SDECLinkGeometry.hpp"

SDECLinkGeometry::~SDECLinkGeometry()
{
}

void SDECLinkGeometry::registerAttributes()
{
	InteractionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(radius1);
	REGISTER_ATTRIBUTE(radius2);
}
