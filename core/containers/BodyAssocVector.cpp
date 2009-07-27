/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BodyAssocVector.hpp"

#include<yade/core/Body.hpp>

BodyAssocVectorIterator::BodyAssocVectorIterator() : BodyContainerIterator()
{

}


BodyAssocVectorIterator::~BodyAssocVectorIterator()
{

}


bool BodyAssocVectorIterator::isDifferent(const BodyContainerIterator& i)
{
	return (bii != static_cast<const BodyAssocVectorIterator&>(i).bii );
}


void BodyAssocVectorIterator::increment()
{
	++bii;
}


void BodyAssocVectorIterator::affect(const BodyContainerIterator& i)
{
	bii = static_cast<const BodyAssocVectorIterator&>(i).bii;
}


shared_ptr<Body> BodyAssocVectorIterator::getValue()
{
	return (*bii).second;
}


shared_ptr<BodyContainerIterator> BodyAssocVectorIterator::createPtr()
{
	return shared_ptr<BodyContainerIterator>(new BodyAssocVectorIterator());
}


/************************************************************************/
/************************************************************************/
/************************************************************************/


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

	//unsigned int position = b->getId();
	unsigned int position=0;

	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(position);
	if( tmpBii != bodies.end() )
	{
		// finds the first free key, which is bigger than 0 (was: newPoistion=position, i.e. greater than id... why?)
		unsigned int newPosition = 0;
		while( bodies.find(newPosition) != bodies.end() ) ++newPosition;
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

	Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator bii = bodies.find(id);

	if( bii != bodies.end() )
	{
		bodies.erase(bii);
		return true;
	}
	else
		return false;
}


bool BodyAssocVector::exists(unsigned int id) const
{
	Loki::AssocVector<unsigned int , shared_ptr<Body> >::const_iterator tmpBii;
	tmpBii = bodies.find(id);

	return (tmpBii != bodies.end());
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
	Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator temporaryBii = bodies.find(id);
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


BodyContainer::iterator BodyAssocVector::begin()
{
	shared_ptr<BodyAssocVectorIterator> it(new BodyAssocVectorIterator());
	it->bii   = bodies.begin();

	return BodyContainer::iterator(it);

}


BodyContainer::iterator BodyAssocVector::end()
{
	shared_ptr<BodyAssocVectorIterator> it(new BodyAssocVectorIterator());
	it->bii   = bodies.end();

	return BodyContainer::iterator(it);
}


 
// 
// void BodyAssocVector::pushIterator()
// {// FIXME - make sure that this is FIFO (I'm tired now...)
// 	iteratorList.push_front(bii);
// }
// 
// 
// void BodyAssocVector::popIterator()
// {
// 	bii = iteratorList.front();
// 	iteratorList.pop_front();
// }
// 

unsigned int BodyAssocVector::size() const
{
	return bodies.size();
}


YADE_PLUGIN("BodyAssocVector");