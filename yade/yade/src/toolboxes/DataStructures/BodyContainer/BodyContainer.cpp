#include "BodyContainer.hpp"
#include "Body.hpp"

void BodyContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(bodyContainer);
};

void BodyContainer::beforeSerialization()
{
	bodyContainer.clear();
	shared_ptr<Body> tmp;
	for( tmp=this->getFirst() ; this->hasCurrent() ; tmp=this->getNext() )
		bodyContainer.push_back(tmp);
	// now bodyContainer is full of bodyContainer
};

void BodyContainer::afterSerialization()
{
	bodyContainer.clear();
	// not anymore.
};

void BodyContainer::beforeDeserialization()
{
	// make sure that bodyContainer is ready for deserialization
	bodyContainer.clear();
};

void BodyContainer::afterDeserialization()
{
	// copy bodyContainer into real container
	this->clear();
	vector<shared_ptr<Body> >::iterator it    = bodyContainer.begin();
	vector<shared_ptr<Body> >::iterator itEnd = bodyContainer.end();
	for( ; it != itEnd ; ++it)
		this->insert(*it);
	bodyContainer.clear();
};

