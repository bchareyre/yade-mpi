#ifndef __INTERACTIONCONTAINER_HPP__
#define __INTERACTIONCONTAINER_HPP__

#include "Serializable.hpp"

class Interaction;

using namespace boost;

class InteractionContainer : public Serializable
{
	public    : InteractionContainer() { data.clear(); };
	public    : virtual ~InteractionContainer() {};

	public    : virtual bool insert(shared_ptr<Interaction>&)						{throw;};
	public    : virtual void clear() 									{throw;};
	public    : virtual bool erase(unsigned int /*id1*/,unsigned int /*id2*/) 				{throw;};
	//public  : virtual bool erase(vector<unsigned int> ids) = 0;

	public    : virtual bool find(unsigned int /*id1*/,unsigned int /*id2*/,shared_ptr<Interaction>&) 	{throw;};
	//public  : virtual bool find(vector<unsigned int> ids,shared_ptr<Interaction>& i) = 0;

	// looping over the data
	public    : virtual shared_ptr<Interaction> getFirst() 							{throw;};
	public    : virtual bool hasCurrent() 									{throw;};
	public    : virtual shared_ptr<Interaction> getNext() 							{throw;};
	public    : virtual shared_ptr<Interaction> getCurent() 						{throw;};
	public    : virtual unsigned int size() 								{throw;};


	// serialization of this class...
	REGISTER_CLASS_NAME(InteractionContainer);

	private   : vector<shared_ptr<Interaction> > data;
	public    : virtual void registerAttributes();

	protected : virtual void beforeSerialization();
	protected : virtual void afterSerialization();
	protected : virtual void beforeDeserialization();
	protected : virtual void afterDeserialization();
};

REGISTER_SERIALIZABLE(InteractionContainer,false);

#endif // __INTERACTIONCONTAINER_HPP__
