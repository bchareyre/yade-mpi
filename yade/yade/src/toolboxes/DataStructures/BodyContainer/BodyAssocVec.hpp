#ifndef __BODYASSOCVEC_HPP__
#define __BODYASSOCVEC_HPP__

#include "BodyContainer.hpp"
#include "AssocVector.hpp"

class Body;

using namespace boost;

class BodyAssocVec : public BodyContainer
{
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> > bodies;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator bii;
	private	: Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator biiEnd;

	public	: BodyAssocVec();
	public	: virtual ~BodyAssocVec();

	public	: virtual unsigned int insert(shared_ptr<Body>&);
	public	: virtual void clear();
	public	: virtual bool erase(unsigned int);
	public	: virtual shared_ptr<Body> find(unsigned int);
	public	: virtual shared_ptr<Body> getFirst();
	public	: virtual bool hasCurrent();
	public	: virtual shared_ptr<Body> getNext();
	public	: virtual shared_ptr<Body> getCurrent();
	public	: virtual unsigned int size();

	// serialization of this class...
	REGISTER_CLASS_NAME(BodyAssocVec);
};

REGISTER_SERIALIZABLE(BodyAssocVec,false);

#endif // __BODYASSOCVEC_HPP__
