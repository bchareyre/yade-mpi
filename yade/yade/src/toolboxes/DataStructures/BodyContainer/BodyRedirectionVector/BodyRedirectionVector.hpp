#ifndef __BODYVECTOR_HPP__
#define __BODYVECTOR_HPP__

#include "BodyContainer.hpp"
#include <vector>

class Body;

using namespace boost;

class BodyRedirectionVector : public BodyContainer
{
	private	: std::vector< shared_ptr<Body> > bodies;
	private	: std::vector< long int > indexes;
	private	: std::vector< shared_ptr<Body> >::iterator vii;
	private	: std::vector< shared_ptr<Body> >::iterator temporaryVii;
	private	: std::vector< shared_ptr<Body> >::iterator viiEnd;
	private	: std::list< std::vector< shared_ptr<Body> >::iterator > iteratorList;

	public	: BodyRedirectionVector();
	public	: virtual ~BodyRedirectionVector();

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
	REGISTER_CLASS_NAME(BodyRedirectionVector);
};

REGISTER_SERIALIZABLE(BodyRedirectionVector,false);

#endif // __BODYVECTOR_HPP__
