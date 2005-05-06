#include "BodyRedirectionVector.hpp"
#include "Body.hpp"


BodyRedirectionVector::BodyRedirectionVector()
{
	clear();
}

BodyRedirectionVector::~BodyRedirectionVector()
{
}

// FIXME - make sure that this is correct
unsigned int BodyRedirectionVector::insert(shared_ptr<Body>& b)
{
	unsigned int position = b->getId();

//	std::vector< shared_ptr<Body> >::iterator tmpVii    = bodies.begin();
//	std::vector< shared_ptr<Body> >::iterator tmpViiEnd = bodies.end();
//	tmpVii = bodies.find(position);

	bool used = false;

//	for( ; tmpVii != tmpViiEnd ; ++tmpVii )
//		if(position == (*tmpVii)->getId() )

	if( (indexes.size() > position)  && (indexes[position] != -1) )
			used = true;

	if( used )
	{
		unsigned int newPosition = position;
		// finds the first free key, which is bigger than id.
		bool newUsed = true;
		while( newUsed )
		{
			newUsed = false;
			++newPosition;

//			tmpVii    = bodies.begin();
//			tmpViiEnd = bodies.end();
//			for( ; tmpVii != tmpViiEnd ; ++tmpVii )
//				if(newPosition == (*tmpVii)->getId() )
			if( (indexes.size() > newPosition)  && (indexes[newPosition] != -1) )
					newUsed = true;
		}
		//cerr << "WARNING: body id=\"" << position << "\" is already used. Using first free id=\"" << newPosition << "\", beware - if you are loading a file, this will break volatileInteractions for this body!\n";
		position = newPosition;
	}
	BodyContainer::setId(b,position);
	bodies.push_back(b);
	
	if(indexes.size() < position+1)
		indexes.resize(position+1, -1 );

	indexes[position] = bodies.size()-1;
	
	return position;
}

unsigned int BodyRedirectionVector::insert(shared_ptr<Body>& b, unsigned int newId)
{
	BodyContainer::setId(b,newId);
	return insert(b);
}

void BodyRedirectionVector::clear()
{
	bodies.clear();
	indexes.clear();
}

bool BodyRedirectionVector::erase(unsigned int id)
{
	std::vector< shared_ptr<Body> >::iterator tmpVii    = bodies.begin();
	std::vector< shared_ptr<Body> >::iterator tmpViiEnd = bodies.end();
	
	for( ; tmpVii != tmpViiEnd ; ++tmpVii )
		if(id == (*tmpVii)->getId() )
		{
			bodies.erase(tmpVii);
			indexes[id] = -1;
			return true;
		}

	return false;
}

bool BodyRedirectionVector::find(unsigned int id , shared_ptr<Body>& b) const
{
//	std::vector< shared_ptr<Body> >::iterator tmpVii    = bodies.begin();
//	std::vector< shared_ptr<Body> >::iterator tmpViiEnd = bodies.end();
//	for( ; tmpVii != tmpViiEnd ; ++tmpVii )
//		if(id == tmpVii->getId())
//		{
//			b = *tmpBii;
//			return true;
//		}

	if(indexes[id] == -1)
		return false;
	else
	{
		b = bodies[ indexes[id] ];
		return true;
	}
}

shared_ptr<Body>& BodyRedirectionVector::operator[](unsigned int id)
{
	// do not modify bii iterator
//	temporaryBii = bodies.find(id);
//	if (bii != bodies.end())
//		return (*temporaryBii).second;
//	else
//		return shared_ptr<Body>();

	return bodies[ indexes[id] ];
}

const shared_ptr<Body>& BodyRedirectionVector::operator[](unsigned int id) const
{
//	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
//	tmpBii = bodies.find(id);
//	return (*tmpBii).second;
	
	return bodies[ indexes[id] ];
}

void BodyRedirectionVector::gotoFirst()
{
	vii    = bodies.begin();
	viiEnd = bodies.end();
}

bool BodyRedirectionVector::notAtEnd()
{
	return ( vii != viiEnd );
}

void BodyRedirectionVector::gotoNext()
{
	++vii;
}

shared_ptr<Body>& BodyRedirectionVector::getCurrent()
{
	return (*vii);
}

void BodyRedirectionVector::pushIterator()
{// FIXME - make sure that this is FIFO (I'm tired now...)
	iteratorList.push_front(vii);
}

void BodyRedirectionVector::popIterator()
{
	vii = iteratorList.front();
	iteratorList.pop_front();
}

unsigned int BodyRedirectionVector::size()
{
	return bodies.size();
}


