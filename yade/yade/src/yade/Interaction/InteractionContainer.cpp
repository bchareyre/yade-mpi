#include "InteractionContainer.hpp"
#include "Interaction.hpp"

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

/*
void InteractionContainer::beforeSerialization()
{
	interaction.clear();
	for( this->gotoFirst() ; this->notAtEnd() ; this->gotoNext() )
		interaction.push_back(this->getCurrent());
	// now interaction is full of data
};

void InteractionContainer::afterSerialization()
{
	interaction.clear();
	// not anymore.
};

void InteractionContainer::beforeDeserialization()
{
	// make sure that data is ready for deserialization
	interaction.clear();
};

void InteractionContainer::afterDeserialization()
{
	// copy interaction into real container
	this->clear();
	vector<shared_ptr<Interaction> >::iterator it    = interaction.begin();
	vector<shared_ptr<Interaction> >::iterator itEnd = interaction.end();
	for( ; it != itEnd ; ++it)
		this->insert(*it);
	interaction.clear();
};
*/
