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


/*void SDECDiscreteElement::updateBoundingVolume(Se3& se3)
{
	bv->update(se3);
}

void SDECDiscreteElement::updateCollisionModel(Se3& )
{

}

void SDECDiscreteElement::moveToNextTimeStep()
{	
	RigidBody::moveToNextTimeStep();
}*/
