#include "Interaction.hpp"
#include "InteractionGeometry.hpp"

Interaction::Interaction ()
{
}

Interaction::Interaction(unsigned int newId1,unsigned int newId2) : id1(newId1) , id2(newId2)
{
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
