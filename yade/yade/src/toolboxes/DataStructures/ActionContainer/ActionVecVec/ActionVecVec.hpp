#ifndef __BODYASSOCVEC_HPP__
#define __BODYASSOCVEC_HPP__

#include "BodyContainer.hpp"
#include "AssocVector.hpp"
#include <list>

class Body;

using namespace boost;

class ActionVecVec : public BodyContainer
{
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> > bodies;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator bii;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator temporaryBii;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator biiEnd;
	private	: std::list<Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator> iteratorList;

	public	: ActionVecVec();
	public	: virtual ~ActionVecVec();

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
	REGISTER_CLASS_NAME(ActionVecVec);
};

REGISTER_SERIALIZABLE(ActionVecVec,false);

#endif // __BODYASSOCVEC_HPP__
