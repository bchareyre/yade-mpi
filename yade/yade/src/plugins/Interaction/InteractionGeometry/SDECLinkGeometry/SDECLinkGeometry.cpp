#include "SDECLinkGeometry.hpp"

SDECLinkGeometry::~SDECLinkGeometry()
{
}


void SDECLinkGeometry::postProcessAttributes(bool /*deserializing*/)
{
// 	if(deserializing)
// 	{
// 		shearForce = Vector3r(0,0,0);
// 		thetar = Vector3r(0,0,0);
// 	}
}

void SDECLinkGeometry::registerAttributes()
{
	InteractionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(radius1);
	REGISTER_ATTRIBUTE(radius2);
	
/*	REGISTER_ATTRIBUTE(initialKn);
	REGISTER_ATTRIBUTE(initialKs);
	REGISTER_ATTRIBUTE(initialEquilibriumDistance);
	REGISTER_ATTRIBUTE(knMax);
	REGISTER_ATTRIBUTE(ksMax);
	REGISTER_ATTRIBUTE(heta);*/
}
