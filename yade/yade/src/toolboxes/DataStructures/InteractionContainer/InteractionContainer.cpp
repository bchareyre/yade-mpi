#include "InteractionContainer.hpp"
#include "Interaction.hpp"

void InteractionContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(interactionContainer);
};

void InteractionContainer::beforeSerialization()
{
	interactionContainer.clear();
	shared_ptr<Interaction> tmp;
	for( tmp=this->getFirst() ; this->hasCurrent() ; tmp=this->getNext() )
		interactionContainer.push_back(tmp);
	// now interactionContainer is full of data
};

void InteractionContainer::afterSerialization()
{
	interactionContainer.clear();
	// not anymore.
};

void InteractionContainer::beforeDeserialization()
{
	// make sure that data is ready for deserialization
	interactionContainer.clear();
};

void InteractionContainer::afterDeserialization()
{
	// copy interactionContainer into real container
	this->clear();
	vector<shared_ptr<Interaction> >::iterator it    = interactionContainer.begin();
	vector<shared_ptr<Interaction> >::iterator itEnd = interactionContainer.end();
	for( ; it != itEnd ; ++it)
		this->insert(*it);
	interactionContainer.clear();
};
