#ifndef __BODYCONTAINER_HPP__
#define __BODYCONTAINER_HPP__

#include <yade-lib-serialization/Serializable.hpp>

class Body;

using namespace boost;

class BodyContainer : public Serializable
{
	public    : BodyContainer() { body.clear(); };
	public    : virtual ~BodyContainer() {};

	//FIXME : use const shared_ptr<Body>&
	public    : virtual unsigned int insert(shared_ptr<Body>&)			{throw;};
	public    : virtual unsigned int insert(shared_ptr<Body>& , unsigned int)	{throw;};
	public    : virtual void clear() 						{throw;};
	public    : virtual bool erase(unsigned int) 					{throw;};
	public    : virtual bool find(unsigned int , shared_ptr<Body>&) const		{throw;};
	public    : virtual shared_ptr<Body>& operator[](unsigned int)			{throw;};
	public    : virtual const shared_ptr<Body>& operator[](unsigned int) const	{throw;};

	// looping over the data
	public    : virtual void gotoFirst() 						{throw;};
	public    : virtual bool notAtEnd() 						{throw;};
	public    : virtual void gotoNext() 						{throw;};
	public    : virtual shared_ptr<Body>& getCurrent()	 			{throw;};
	public    : virtual void pushIterator()			 			{throw;};
	public    : virtual void popIterator()			 			{throw;};

	public    : virtual unsigned int size() 					{throw;};

	protected : void setId(shared_ptr<Body>& , unsigned int);

	// serialization of this class...
	REGISTER_CLASS_NAME(BodyContainer);
	// local storage for uniform serialization of all possible container concrete implementations.
	private   : vector<shared_ptr<Body> > body; 
	
	public    : virtual void registerAttributes();

	protected : virtual void preProcessAttributes(bool deserializing);
	protected : virtual void postProcessAttributes(bool deserializing);
};

REGISTER_SERIALIZABLE(BodyContainer,false);

#endif // __BODYCONTAINER_HPP__

