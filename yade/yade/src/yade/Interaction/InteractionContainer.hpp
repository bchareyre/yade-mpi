#ifndef __INTERACTIONCONTAINER_HPP__
#define __INTERACTIONCONTAINER_HPP__

#include "Serializable.hpp"

class Interaction;

using namespace boost;

class InteractionContainer : public Serializable
{
	public    : InteractionContainer() { interaction.clear(); };
	public    : virtual ~InteractionContainer() {};

	public    : virtual bool insert(shared_ptr<Interaction>&)						{throw;};
	public    : virtual void clear() 									{throw;};
	public    : virtual bool erase(unsigned int /*id1*/,unsigned int /*id2*/) 				{throw;};
	//public  : virtual bool erase(vector<unsigned int> ids) = 0;

	public    : virtual bool find(unsigned int /*id1*/,unsigned int /*id2*/,shared_ptr<Interaction>&) 	{throw;};
	//public  : virtual bool find(vector<unsigned int> ids,shared_ptr<Interaction>& i) = 0;

	// looping over the data
	public    : virtual void gotoFirst() 									{throw;};
	public    : virtual bool notAtEnd() 									{throw;};
	public    : virtual void gotoNext() 									{throw;};
 	public    : virtual shared_ptr<Interaction> getCurrent() 						{throw;};

//	public    : virtual shared_ptr<Interaction> getFirst() 							{throw;};
// 	public    : virtual bool hasCurrent() 									{throw;};
// 	public    : virtual shared_ptr<Interaction> getNext() 							{throw;};

	// deletes currently pointed element, and goes to the next one.
	public    : virtual void eraseCurrentAndGotoNext() 							{throw;};
	public    : virtual unsigned int size() 								{throw;};


	// serialization of this class...
	REGISTER_CLASS_NAME(InteractionContainer);

	private   : vector<shared_ptr<Interaction> > interaction;
	public    : virtual void registerAttributes();

	protected : virtual void preProcessAttributes(bool deserializing);
	protected : virtual void postProcessAttributes(bool deserializing);
};

REGISTER_SERIALIZABLE(InteractionContainer,false);

#endif // __INTERACTIONCONTAINER_HPP__
