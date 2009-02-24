// 2008 © Sergei Dorofeenko <sega@users.berlios.de>

#include "InteractionVecMap.hpp"
#include <iostream>


InteractionVecMapIterator::InteractionVecMapIterator() : InteractionContainerIterator()
{

}


InteractionVecMapIterator::~InteractionVecMapIterator()
{

}

shared_ptr<Interaction>& InteractionVecMap::operator[](unsigned int id)
{
	return interactions[id];
}

const shared_ptr<Interaction>& InteractionVecMap::operator[](unsigned int id) const
{
	return interactions[id];
}

bool InteractionVecMapIterator::isDifferent(const InteractionContainerIterator& i)
{
	const InteractionVecMapIterator& it = static_cast<const InteractionVecMapIterator&>(i);
	return ( vii != it.vii );
}


void InteractionVecMapIterator::increment()
{
	++vii;
}


void InteractionVecMapIterator::affect(const InteractionContainerIterator& i)
{
	const InteractionVecMapIterator& tmpi = static_cast<const InteractionVecMapIterator&>(i);
	vii    = tmpi.vii;
}


shared_ptr<Interaction> InteractionVecMapIterator::getValue()
{
	return *vii;
}


shared_ptr<InteractionContainerIterator> InteractionVecMapIterator::createPtr()
{
	return shared_ptr<InteractionContainerIterator>(new InteractionVecMapIterator());
}


/*********************************************************************/

InteractionVecMap::InteractionVecMap()
{
	currentSize = 0;
	clear();
}


InteractionVecMap::~InteractionVecMap()
{
}


bool InteractionVecMap::insert(shared_ptr<Interaction>& i)
{
	boost::mutex::scoped_lock lock(drawloopmutex);

	body_id_t id1 = i->getId1();
	body_id_t id2 = i->getId2();

	if (id1>id2)
		swap(id1,id2);

	if ( static_cast<unsigned int>(id1) >=vecmap.size())
		vecmap.resize(id1+1);

	if (vecmap[id1].insert(pair<body_id_t,unsigned int >(id2,currentSize)).second)
	{
		if (interactions.size() == currentSize)
			interactions.resize(currentSize+1);

		interactions[currentSize]=i;
		currentSize++;
	
		return true;
	}
	else
		return false;
}


bool InteractionVecMap::insert(body_id_t id1,body_id_t id2)
{
	shared_ptr<Interaction> i(new Interaction(id1,id2) );
	return insert(i);	
}


void InteractionVecMap::clear()
{
	boost::mutex::scoped_lock lock(drawloopmutex);

	vecmap.clear();
	interactions.clear();
	currentSize=0;
}


bool InteractionVecMap::erase(body_id_t id1,body_id_t id2)
{
	boost::mutex::scoped_lock lock(drawloopmutex);

	if (id1>id2)
		swap(id1,id2);

	if ( static_cast<unsigned int>(id1) < vecmap.size())
	{
		map<body_id_t,unsigned int >::iterator mii;
		mii = vecmap[id1].find(id2);
		if ( mii != vecmap[id1].end() )
		{
			unsigned int iid = (*mii).second;
			vecmap[id1].erase(mii);
			currentSize--;
			if (iid<currentSize) {
				interactions[iid]=interactions[currentSize];
				id1 = interactions[iid]->getId1();
				id2 = interactions[iid]->getId2();
				if (id1>id2) swap(id1,id2);
				vecmap[id1][id2]=iid;
			}
			return true;
		}
		else
			return false;
	}

	return false;

}


const shared_ptr<Interaction>& InteractionVecMap::find(body_id_t id1,body_id_t id2)
{
	if (id1>id2)
		swap(id1,id2);

	if (static_cast<unsigned int>(id1)<vecmap.size())
	{
		map<body_id_t,unsigned int >::iterator mii;
		mii = vecmap[id1].find(id2);
		if (mii!=vecmap[id1].end())
			return interactions[(*mii).second];
		else
		{
			empty = shared_ptr<Interaction>();
			return empty;
		}
	}
	else
	{
		empty = shared_ptr<Interaction>();
		return empty;
	}
}


InteractionContainer::iterator InteractionVecMap::begin()
{
	shared_ptr<InteractionVecMapIterator> it(new InteractionVecMapIterator());
	it->vii    = interactions.begin();
	return InteractionContainer::iterator(it);
}


InteractionContainer::iterator InteractionVecMap::end()
{
	shared_ptr<InteractionVecMapIterator> it(new InteractionVecMapIterator());
	it->vii		= interactions.begin()+currentSize;
	return InteractionContainer::iterator(it);
}

unsigned int InteractionVecMap::size()
{
	return currentSize;
}

// YADE_PLUGIN();
