#include "SDECPermanentLink.hpp"

SDECPermanentLink::SDECPermanentLink() : InteractionGeometry()
{
}


SDECPermanentLink::~SDECPermanentLink ()
{


}

void SDECPermanentLink::processAttributes()
{
	shearForce = Vector3(0,0,0);
}

void SDECPermanentLink::registerAttributes()
{
	InteractionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(initialKn);			
	REGISTER_ATTRIBUTE(initialKs);			
	REGISTER_ATTRIBUTE(initialEquilibriumDistance);
	REGISTER_ATTRIBUTE(prevNormal);		
	REGISTER_ATTRIBUTE(knMax);
	REGISTER_ATTRIBUTE(ksMax);
}
