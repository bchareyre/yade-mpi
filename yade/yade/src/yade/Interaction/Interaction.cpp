#include "Interaction.hpp"
#include "InteractionGeometry.hpp"

Interaction::Interaction ()
{
	isPotential = false;  // should be true 
	isNew = true; // maybe we can remove this, and check if InteractionGeometry, and InteractionPhysics are empty?
}

Interaction::Interaction(unsigned int newId1,unsigned int newId2) : id1(newId1) , id2(newId2)
{
	// FIXME - this should be initialized to true, then modified by BroadCollider.
	isPotential = false;  // should be true 
	isNew = true; // maybe we can remove this, and check if InteractionGeometry, and InteractionPhysics are empty?
};

Interaction::~Interaction ()
{

}

void Interaction::postProcessAttributes(bool)
{

}

void Interaction::registerAttributes()
{
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
	REGISTER_ATTRIBUTE(interactionGeometry);
}
