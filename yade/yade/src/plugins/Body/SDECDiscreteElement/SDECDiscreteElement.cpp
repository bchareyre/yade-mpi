#include "SDECDiscreteElement.hpp"

SDECDiscreteElement::SDECDiscreteElement () : RigidBody()
{
}


SDECDiscreteElement::~SDECDiscreteElement()
{

}

void SDECDiscreteElement::processAttributes()
{
	RigidBody::processAttributes();
}

void SDECDiscreteElement::registerAttributes()
{
	RigidBody::registerAttributes();
	REGISTER_ATTRIBUTE(kn);
	REGISTER_ATTRIBUTE(ks);
}


/*void SDECDiscreteElement::updateBoundingVolume(Se3r& se3)
{
	bv->update(se3);
}

void SDECDiscreteElement::updateCollisionGeometry(Se3r& )
{

}

void SDECDiscreteElement::moveToNextTimeStep()
{	
	RigidBody::moveToNextTimeStep();
}*/
