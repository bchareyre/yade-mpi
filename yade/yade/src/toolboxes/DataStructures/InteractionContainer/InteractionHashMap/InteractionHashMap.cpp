#include "InteractionHashMap.hpp"

InteractionHashMap::InteractionHashMap()
{
	clear();
}

InteractionHashMap::~InteractionHashMap()
{
}

bool InteractionHashMap::insert(shared_ptr<Interaction>& i)
{
	unsigned int id1 = i->getId1();
	unsigned int id2 = i->getId2();
	if (id1>id2)
		swap(id1,id2);

	return interactions.insert( IHashMap::value_type( pair<unsigned int,unsigned int>(id1,id2) , i )).second;
}

bool InteractionHashMap::insert(unsigned int id1,unsigned int id2)
{
	shared_ptr<Interaction> i(new Interaction(id1,id2) );
	return insert(i);	
};

void InteractionHashMap::clear()
{
	interactions.clear();
}

bool InteractionHashMap::erase(unsigned int id1,unsigned int id2)
{
	if (id1>id2)
		swap(id1,id2);

	unsigned int oldSize = interactions.size();
	pair<unsigned int,unsigned int> p(id1,id2);
	unsigned int size = interactions.erase(p);

	return size!=oldSize;

}

shared_ptr<Interaction> InteractionHashMap::find(unsigned int id1,unsigned int id2)
{
	if (id1>id2)
		swap(id1,id2);

	hmii = interactions.find(pair<unsigned int,unsigned int>(id1,id2));
	if (hmii!=interactions.end())
		return (*hmii).second;
	else
		return shared_ptr<Interaction>();
}

void InteractionHashMap::gotoFirstPotential()
{
	hmii    = interactions.begin();
	hmiiEnd = interactions.end();
}

bool InteractionHashMap::notAtEndPotential()
{
	return ( hmii != hmiiEnd );
}

void InteractionHashMap::gotoNextPotential()
{
	++hmii;
}

void InteractionHashMap::gotoFirst()
{
	gotoFirstPotential();
	if (notAtEnd() && !getCurrent()->isReal)
		gotoNext();
}

void InteractionHashMap::gotoNext()
{
	gotoNextPotential();
	while( notAtEnd() && !getCurrent()->isReal)
		gotoNextPotential();
}

bool InteractionHashMap::notAtEnd()
{
	return notAtEndPotential();
}

shared_ptr<Interaction> InteractionHashMap::getCurrent()
{
	return (*hmii).second;
}


void InteractionHashMap::eraseCurrentAndGotoNextPotential()
{
	if (notAtEnd())
	{
		IHashMap::iterator tmpHmii=hmii;
		++hmii;
		interactions.erase(tmpHmii);
	}
}

void InteractionHashMap::eraseCurrentAndGotoNext()
{
	IHashMap::iterator tmpHmii=hmii;	
	while (notAtEnd() && !((*hmii).second->isReal))
		++hmii;	
	interactions.erase(tmpHmii);
}

unsigned int InteractionHashMap::size()
{
	return interactions.size();
}
