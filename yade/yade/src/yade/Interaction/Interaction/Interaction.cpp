#include "Interaction.hpp"

Interaction::Interaction ()
{
	// FIXME : -1
	id1 = 0;
	id2 = 0;
	isNew = true;
	isReal = false; // maybe we can remove this, and check if InteractionDescription, and InteractionPhysics are empty?
	isNonPermanent = true;
}

Interaction::Interaction(unsigned int newId1,unsigned int newId2) : id1(newId1) , id2(newId2)
{	
	isNew = true;
	isReal = false;
	isNonPermanent = true;
}

void Interaction::registerAttributes()
{
	REGISTER_ATTRIBUTE(id1);
	REGISTER_ATTRIBUTE(id2);
	REGISTER_ATTRIBUTE(isReal);
	REGISTER_ATTRIBUTE(interactionGeometry);
	REGISTER_ATTRIBUTE(interactionPhysics);
}
