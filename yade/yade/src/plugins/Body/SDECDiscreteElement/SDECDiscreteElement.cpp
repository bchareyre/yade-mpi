#include "SDECDiscreteElement.hpp"

SDECDiscreteElement::SDECDiscreteElement () : RigidBody()
{
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
