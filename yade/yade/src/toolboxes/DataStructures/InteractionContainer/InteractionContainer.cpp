#include "InteractionContainer.hpp"
#include "Interaction.hpp"

void InteractionContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(data);
};

void InteractionContainer::beforeSerialization()
{
	data.clear();
	shared_ptr<Interaction> tmp;
	for( tmp=this->getFirst() ; this->hasCurrent() ; tmp=this->getNext() )
		data.push_back(tmp);
	// now data is full of data
};

void InteractionContainer::afterSerialization()
{
	data.clear();
	// not anymore.
};

void InteractionContainer::beforeDeserialization()
{
	// make sure that data is ready for deserialization
	data.clear();
};

void InteractionContainer::afterDeserialization()
{
	// copy data into real container
	this->clear();
	vector<shared_ptr<Interaction> >::iterator it    = data.begin();
	vector<shared_ptr<Interaction> >::iterator itEnd = data.end();
	for( ; it != itEnd ; ++it)
		this->insert(*it);
	data.clear();
};
