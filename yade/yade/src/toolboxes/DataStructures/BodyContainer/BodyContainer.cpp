#include "BodyContainer.hpp"
#include "Body.hpp"

void BodyContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(bodies2);
};

void BodyContainer::beforeSerialization()
{
	bodies2.clear();
	shared_ptr<Body> tmp;
	for( tmp=this->getFirst() ; this->hasCurrent() ; tmp=this->getNext() )
		bodies2.push_back(tmp);
	// now bodies2 is full of bodies2
};

void BodyContainer::afterSerialization()
{
	bodies2.clear();
	// not anymore.
};

void BodyContainer::beforeDeserialization()
{
	// make sure that bodies2 is ready for deserialization
	bodies2.clear();
};

void BodyContainer::afterDeserialization()
{
	// copy bodies2 into real container
	this->clear();
	vector<shared_ptr<Body> >::iterator it    = bodies2.begin();
	vector<shared_ptr<Body> >::iterator itEnd = bodies2.end();
	for( ; it != itEnd ; ++it)
		this->insert(*it);
	bodies2.clear();
};

