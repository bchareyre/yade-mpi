#include "SDECPermanentLink.hpp"

SDECPermanentLink::SDECPermanentLink() : InteractionGeometry()
{
}


SDECPermanentLink::~SDECPermanentLink ()
{


}

void SDECPermanentLink::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		shearForce = Vector3r(0,0,0);
		thetar = Vector3r(0,0,0);
	}
}

void SDECPermanentLink::registerAttributes()
{
	InteractionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(radius1);
	REGISTER_ATTRIBUTE(radius2);
}
