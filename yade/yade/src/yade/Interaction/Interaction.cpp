#include "Interaction.hpp"

Interaction::Interaction ()
{
}

Interaction::~Interaction ()
{

}

void Interaction::afterDeserialization()
{

}

void Interaction::registerAttributes()
{
	REGISTER_ATTRIBUTE(interactionGeometry);
}
