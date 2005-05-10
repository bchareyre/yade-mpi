#include "InteractionContainer.hpp"
#include <yade/Interaction.hpp>

void InteractionContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(interaction);
};

void InteractionContainer::preProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		interaction.clear();
	}
	else
	{
		interaction.clear();
		for( this->gotoFirst() ; this->notAtEnd() ; this->gotoNext() )
			interaction.push_back(this->getCurrent());
	}
};

void InteractionContainer::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		this->clear();
		vector<shared_ptr<Interaction> >::iterator it    = interaction.begin();
		vector<shared_ptr<Interaction> >::iterator itEnd = interaction.end();
		for( ; it != itEnd ; ++it)
			this->insert(*it);
		interaction.clear();
	}
	else
	{
		interaction.clear();
	}
};

