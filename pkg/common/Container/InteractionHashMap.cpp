/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionHashMap.hpp"

InteractionHashMapIterator::InteractionHashMapIterator() : InteractionContainerIterator()
{

}


InteractionHashMapIterator::~InteractionHashMapIterator()
{

}


bool InteractionHashMapIterator::isDifferent(const InteractionContainerIterator& i)
{
	return (hmii != static_cast<const InteractionHashMapIterator&>(i).hmii );
}


void InteractionHashMapIterator::increment()
{
	++hmii;
}


void InteractionHashMapIterator::affect(const InteractionContainerIterator& i)
{
	hmii = static_cast<const InteractionHashMapIterator&>(i).hmii;
}


shared_ptr<Interaction> InteractionHashMapIterator::getValue()
{
	return (*hmii).second;
}


shared_ptr<InteractionContainerIterator> InteractionHashMapIterator::createPtr()
{
	return shared_ptr<InteractionContainerIterator>(new InteractionHashMapIterator());
}


/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

InteractionHashMap::InteractionHashMap()
{
	clear();
}


InteractionHashMap::~InteractionHashMap()
{
}


bool InteractionHashMap::insert(shared_ptr<Interaction>& i)
{
	boost::mutex::scoped_lock lock(drawloopmutex);

	body_id_t id1 = i->getId1();
	body_id_t id2 = i->getId2();
	if (id1>id2)
		swap(id1,id2);

	return interactions.insert( IHashMap::value_type( pair<body_id_t,body_id_t>(id1,id2) , i )).second;
}


bool InteractionHashMap::insert(body_id_t id1,body_id_t id2)
{
	shared_ptr<Interaction> i(new Interaction(id1,id2) );
	return insert(i);	
}


void InteractionHashMap::clear()
{
	boost::mutex::scoped_lock lock(drawloopmutex);

	interactions.clear();
}


bool InteractionHashMap::erase(body_id_t id1,body_id_t id2)
{
	boost::mutex::scoped_lock lock(drawloopmutex);

	if (id1>id2)
		swap(id1,id2);

	unsigned int oldSize = interactions.size();
	pair<body_id_t,body_id_t> p(id1,id2);
	unsigned int size = interactions.erase(p);

	return size!=oldSize;

}


const shared_ptr<Interaction>& InteractionHashMap::find(body_id_t id1,body_id_t id2)
{
	if (id1>id2)
		swap(id1,id2);

	IHashMap::iterator hmii = interactions.find(pair<body_id_t,body_id_t>(id1,id2));
	if (hmii!=interactions.end())
		return (*hmii).second;
	else
	{
		empty = shared_ptr<Interaction>(); 
		return empty;
	}
}


InteractionContainer::iterator InteractionHashMap::begin()
{
	shared_ptr<InteractionHashMapIterator> it(new InteractionHashMapIterator());
	it->hmii   = interactions.begin();

	return InteractionContainer::iterator(it);
}


InteractionContainer::iterator InteractionHashMap::end()
{
	shared_ptr<InteractionHashMapIterator> it(new InteractionHashMapIterator());
	it->hmii   = interactions.end();

	return InteractionContainer::iterator(it);
}


// void InteractionHashMap::eraseCurrentAndGotoNextPotential()
// {
// 	if (notAtEnd())
// 	{
// 		IHashMap::iterator tmpHmii=hmii;
// 		++hmii;
// 		interactions.erase(tmpHmii);
// 	}
// }
// 
// void InteractionHashMap::eraseCurrentAndGotoNext()
// {
// 	IHashMap::iterator tmpHmii=hmii;	
// 	while (notAtEnd() && !((*hmii).second->isReal))
// 		++hmii;	
// 	interactions.erase(tmpHmii);
// }

unsigned int InteractionHashMap::size()
{
	return interactions.size();
}
YADE_PLUGIN();
