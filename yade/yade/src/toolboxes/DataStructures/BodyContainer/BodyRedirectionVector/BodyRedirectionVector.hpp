/*
#ifndef __BODYVECTOR_HPP__
#define __BODYVECTOR_HPP__

#include "Serializable.hpp"

class Interaction;

using namespace boost;

class BodyVector : public BodyContainer
{
	public    : BodyVector() { data.clear(); };
	public    : virtual ~BodyVector() {};

	public    : virtual bool insert(shared_ptr<Interaction>&)						{throw;};
	public    : virtual void clear() 									{throw;};
	public    : virtual bool erase(unsigned int id1,unsigned int id2) 				{throw;};
	//public  : virtual bool erase(vector<unsigned int> ids) = 0;

	public    : virtual bool find(unsigned int id1,unsigned int id2,shared_ptr<Interaction>&) 	{throw;};
	//public  : virtual bool find(vector<unsigned int> ids,shared_ptr<Interaction>& i) = 0;

	// looping over the data
	public    : virtual shared_ptr<Interaction> getFirst() 							{throw;};
	public    : virtual bool hasCurrent() 									{throw;};
	public    : virtual shared_ptr<Interaction> getNext() 							{throw;};
	public    : virtual shared_ptr<Interaction> getCurrent() 						{throw;};
	// deletes currently pointed element, and returns the next one.
	public    : virtual shared_ptr<Interaction> eraseCurrent() 						{throw;};
	public    : virtual unsigned int size() 								{throw;};


	// serialization of this class...
	REGISTER_CLASS_NAME(BodyVector);

	private   : vector<shared_ptr<Interaction> > data;
	public    : virtual void registerAttributes();

	protected : virtual void beforeSerialization();
	protected : virtual void afterSerialization();
	protected : virtual void beforeDeserialization();
	protected : virtual void afterDeserialization();
};

REGISTER_SERIALIZABLE(BodyVector,false);

#endif // __BODYVECTOR_HPP__
*/


#ifndef __BODYVECTOR_HPP__
#define __BODYVECTOR_HPP__

#include "BodyContainer.hpp"
#include <vector>

class Body;

using namespace boost;

class BodyVector : public BodyContainer
{
	private	: std::vector< shared_ptr<Body> > bodies;
	private	: std::vector< long int > indexes;
	private	: std::vector< shared_ptr<Body> >::iterator vii;
	private	: std::vector< shared_ptr<Body> >::iterator temporaryVii;
	private	: std::vector< shared_ptr<Body> >::iterator viiEnd;
	private	: std::list< std::vector< shared_ptr<Body> >::iterator > iteratorList;

	public	: BodyVector();
	public	: virtual ~BodyVector();

	public	: virtual unsigned int insert(shared_ptr<Body>&);
	public	: virtual unsigned int insert(shared_ptr<Body>& , unsigned int);
	public	: virtual void clear();
	public	: virtual bool erase(unsigned int);
	public	: virtual bool find(unsigned int , shared_ptr<Body>&) const;
	public	: virtual shared_ptr<Body>& operator[](unsigned int);
	public	: virtual const shared_ptr<Body>& operator[](unsigned int) const;
	public	: virtual void pushIterator();
	public	: virtual void popIterator();

	public	: virtual void gotoFirst();
	public	: virtual bool notAtEnd();
	public	: virtual void gotoNext();
	public	: virtual shared_ptr<Body> getCurrent();

	public	: virtual unsigned int size();

	// serialization of this class...
	REGISTER_CLASS_NAME(BodyVector);
};

REGISTER_SERIALIZABLE(BodyVector,false);

#endif // __BODYVECTOR_HPP__
