#ifndef __BODYCONTAINER_HPP__
#define __BODYCONTAINER_HPP__

#include "Serializable.hpp"

class Body;

using namespace boost;

class BodyContainer : public Serializable
{
	public    : BodyContainer() { bodyContainer.clear(); };
	public    : virtual ~BodyContainer() {};

	public    : virtual unsigned int insert(shared_ptr<Body>&)		{throw;};
	public    : virtual void clear() 					{throw;};
	public    : virtual bool erase(unsigned int) 				{throw;};
	public    : virtual shared_ptr<Body> find(unsigned int)			{throw;};

	// looping over the data
	public    : virtual shared_ptr<Body> getFirst() 			{throw;};
	public    : virtual bool hasCurrent() 					{throw;};
	public    : virtual shared_ptr<Body> getNext() 				{throw;};
	public    : virtual shared_ptr<Body> getCurrent() 			{throw;};
	public    : virtual unsigned int size() 				{throw;};


	// serialization of this class...
	REGISTER_CLASS_NAME(BodyContainer);

	private   : vector<shared_ptr<Body> > bodyContainer;
	public    : virtual void registerAttributes();

	protected : virtual void beforeSerialization();
	protected : virtual void afterSerialization();
	protected : virtual void beforeDeserialization();
	protected : virtual void afterDeserialization();
};

REGISTER_SERIALIZABLE(BodyContainer,false);

#endif // __BODYCONTAINER_HPP__

