/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"BodyRedirectionVector.hpp"


#include<yade/core/Body.hpp>


BodyRedirectionVectorIterator::BodyRedirectionVectorIterator() : BodyContainerIterator()
{

}


BodyRedirectionVectorIterator::~BodyRedirectionVectorIterator()
{

}


bool BodyRedirectionVectorIterator::isDifferent(const BodyContainerIterator& i)
{
	return (vii != static_cast<const BodyRedirectionVectorIterator&>(i).vii );
}


void BodyRedirectionVectorIterator::increment()
{
	++vii;
}


void BodyRedirectionVectorIterator::affect(const BodyContainerIterator& i)
{
	vii = static_cast<const BodyRedirectionVectorIterator&>(i).vii;
}


shared_ptr<Body> BodyRedirectionVectorIterator::getValue()
{
	return (*vii);
}


shared_ptr<BodyContainerIterator> BodyRedirectionVectorIterator::createPtr()
{
	return shared_ptr<BodyContainerIterator>(new BodyRedirectionVectorIterator());
}

/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/


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
	// This line is important! Otherwise the bodies are renumbered after reloading, 
	// and all subscribedBodies breaks, which is the case if you delete bodies
	// from simulation. Lattice is deleting bodies. /Janek
	unsigned int position = b->getId();

	// WRONG !!
	//unsigned position=0;

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
		// finds the first free key (was: which is bigger than id... why bigger??)
		unsigned int newPosition = 0; // was: newPosition=position;
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
		//cerr << "WARNING: body id=\"" << position << "\" is already used. Using first free id=\"" << newPosition << "\", beware - if you are loading a file, this will break transientInteractions for this body!\n";
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
//	std::vector< shared_ptr<Body> >::iterator tmpVii    = bodies.begin();
//	std::vector< shared_ptr<Body> >::iterator tmpViiEnd = bodies.end();
//	for( ; tmpVii != tmpViiEnd ; ++tmpVii )
//		if(id == (*tmpVii)->getId() )
//		{
//			bodies.erase(tmpVii); // WRONG - bodies do change their indexes!
//			indexes[id] = -1;
//			return true;
//		}
//	return false;

	long int deleted = indexes[id];
	if( deleted != -1 )
	{
		std::vector< shared_ptr<Body> >::iterator tmpVii    = bodies.begin(); 
		tmpVii += deleted;
//		assert( (*tmpVii)->getId() == id );
		bodies.erase(tmpVii);
		indexes[id] = -1;
		
		std::vector<long int>::iterator scan = indexes.begin();
		std::vector<long int>::iterator end  = indexes.end();
		for( ; scan != end ; ++scan )
			if( *scan > deleted )
				--(*scan); 
		return true;
	}
	else
		return false;
}


bool BodyRedirectionVector::exists(unsigned int id) const
{
        return id<indexes.size() && (indexes[id] != -1);
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


BodyContainer::iterator BodyRedirectionVector::begin()
{
	shared_ptr<BodyRedirectionVectorIterator> it(new BodyRedirectionVectorIterator());
	it->vii   = bodies.begin();

	return BodyContainer::iterator(it);
}


BodyContainer::iterator BodyRedirectionVector::end()
{
	shared_ptr<BodyRedirectionVectorIterator> it(new BodyRedirectionVectorIterator());
	it->vii   = bodies.end();

	return BodyContainer::iterator(it);
}



// 
// void BodyRedirectionVector::pushIterator()
// {// FIXME - make sure that this is FIFO (I'm tired now...)
// 	iteratorList.push_front(vii);
// }
// 
// 
// void BodyRedirectionVector::popIterator()
// {
// 	vii = iteratorList.front();
// 	iteratorList.pop_front();
// }
// 

unsigned int BodyRedirectionVector::size()
{
	return bodies.size();
}


// YADE_PLUGIN();
