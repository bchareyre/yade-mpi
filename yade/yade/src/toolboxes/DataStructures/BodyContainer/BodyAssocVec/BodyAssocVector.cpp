#include "BodyAssocVector.hpp"
#include "Body.hpp"

BodyAssocVector::BodyAssocVector()
{
	clear();
}

BodyAssocVector::~BodyAssocVector()
{
}

unsigned int BodyAssocVector::insert(shared_ptr<Body>& b)
{
//	unsigned int max;
//	if( bodies.begin() != bodies.end() )
//		max = (--(bodies.end()))->first + 1;
//	else
//		max = 0;
//	bodies.insert( Loki::AssocVector::value_type( max , b ));
//	bodies[max]=b;
//	BodyContainer::setId(b,max);
//	return max;

	unsigned int position = b->getId();

	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(position);
	if( tmpBii != bodies.end() )
	{
		unsigned int newPosition = position;
		// finds the first free key, which is bigger than id.
		while( bodies.find(newPosition) != bodies.end() )
			++newPosition;
		//cerr << "WARNING: body id=\"" << position << "\" is already used. Using first free id=\"" << newPosition << "\", beware - if you are loading a file, this will break interactions for this body!\n";
		position = newPosition;
	}
	BodyContainer::setId(b,position);
	bodies[position]=b;
	return position;
}

unsigned int BodyAssocVector::insert(shared_ptr<Body>& b, unsigned int newId)
{
	BodyContainer::setId(b,newId);
	return insert(b);
}

void BodyAssocVector::clear()
{
	bodies.clear();
}

bool BodyAssocVector::erase(unsigned int id)
{

// WARNING!!! AssocVector.erase() invalidates all iterators !!!

	bii = bodies.find(id);

	if( bii != bodies.end() )
	{
		bodies.erase(bii);
		return true;
	}
	else
		return false;
}

bool BodyAssocVector::find(unsigned int id , shared_ptr<Body>& b) const
{
	// do not modify the interanl iterator
	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(id);

	if (tmpBii != bodies.end())
	{
		b = (*tmpBii).second;
		return true;
	}
	else
		return false;
}

shared_ptr<Body>& BodyAssocVector::operator[](unsigned int id)
{
	// do not modify bii iterator
	temporaryBii = bodies.find(id);
//	if (bii != bodies.end())
		return (*temporaryBii).second;
//	else
//		return shared_ptr<Body>();
}

const shared_ptr<Body>& BodyAssocVector::operator[](unsigned int id) const
{
	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(id);
// when commented it is faster, but less secure.
//	if (tmpBii != bodies.end())
		return (*tmpBii).second;
//	else
//		return shared_ptr<Body>();
}

void BodyAssocVector::gotoFirst()
{
	bii    = bodies.begin();
	biiEnd = bodies.end();
}

bool BodyAssocVector::notAtEnd()
{
	return ( bii != biiEnd );
}

void BodyAssocVector::gotoNext()
{
	++bii;
}

shared_ptr<Body>& BodyAssocVector::getCurrent()
{
	return (*bii).second;
}

void BodyAssocVector::pushIterator()
{// FIXME - make sure that this is FIFO (I'm tired now...)
	iteratorList.push_front(bii);
}

void BodyAssocVector::popIterator()
{
	bii = iteratorList.front();
	iteratorList.pop_front();
}

unsigned int BodyAssocVector::size()
{
	return bodies.size();
}


