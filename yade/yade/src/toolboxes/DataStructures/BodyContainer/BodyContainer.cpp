#include "BodyContainer.hpp"
#include "Body.hpp"

void BodyContainer::registerAttributes()
{
	REGISTER_ATTRIBUTE(bodies);
};

void BodyContainer::beforeSerialization()
{
	bodies.clear();
	shared_ptr<Body> tmp;
	for( tmp=this->getFirst() ; this->hasCurrent() ; tmp=this->getNext() )
		bodies.push_back(tmp);
	// now bodies is full of bodies
};

void BodyContainer::afterSerialization()
{
	bodies.clear();
	// not anymore.
};

void BodyContainer::beforeDeserialization()
{
	// make sure that bodies is ready for deserialization
	bodies.clear();
};

void BodyContainer::afterDeserialization()
{
	// copy bodies into real container
	this->clear();
	vector<shared_ptr<Body> >::iterator it    = bodies.begin();
	vector<shared_ptr<Body> >::iterator itEnd = bodies.end();
	for( ; it != itEnd ; ++it)
		this->insert(*it);
	bodies.clear();
};

