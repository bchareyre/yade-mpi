#include "BodyAssocVec.hpp"
#include "Body.hpp"

BodyAssocVec::BodyAssocVec()
{
}

BodyAssocVec::~BodyAssocVec()
{
}

unsigned int BodyAssocVec::insert(shared_ptr<Body>& b)
{
	unsigned int max;
	if( bodies.begin() != bodies.end() )
		max = (--(bodies.end()))->first + 1;
	else
		max = 0;

//	bodies.insert( Loki::AssocVector::value_type( max , b ));
	bodies[max]=b;

//	cout << "inserting body : " << max << endl;

	return max;
}

void BodyAssocVec::clear()
{
	bodies.clear();
}

bool BodyAssocVec::erase(unsigned int id)
{
	bii = bodies.find(id);

	if( bii != bodies.end() )
	{
		bodies.erase(bii);
		return true;
	}
	else
		return false;
}

shared_ptr<Body> BodyAssocVec::find(unsigned int id)
{
	bii = bodies.find(id);
	if (bii != bodies.end())
		return (*bii).second;
	else
		return shared_ptr<Body>();
}

shared_ptr<Body> BodyAssocVec::getFirst()
{
	bii    = bodies.begin();
	biiEnd = bodies.end();
	if(bii == biiEnd)
		return shared_ptr<Body>();
	else
		return (*bii).second;
}

bool BodyAssocVec::hasCurrent()
{
	return ( bii != biiEnd );
}

shared_ptr<Body> BodyAssocVec::getNext()
{
	if(hasCurrent() && ++bii != biiEnd)
		return (*bii).second;
	else
		return shared_ptr<Body>();
}

shared_ptr<Body> BodyAssocVec::getCurrent()
{
	if(hasCurrent())
		return (*bii).second;
	else
		return shared_ptr<Body>();
}

unsigned int BodyAssocVec::size()
{
	return bodies.size();
}


