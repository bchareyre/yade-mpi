#include "InteractionHashMap2.hpp"

InteractionHashMap2::InteractionHashMap2()
{
}

InteractionHashMap2::~InteractionHashMap2()
{
}

bool InteractionHashMap2::insert(shared_ptr<Interaction>& i)
{
	unsigned int id1 = i->getId1();
	unsigned int id2 = i->getId2();
	if (id1>id2)
		swap(id1,id2);

	return interactions.insert( IHashMap::value_type( pair<unsigned int,unsigned int>(id1,id2) , i )).second;
}

void InteractionHashMap2::clear()
{
	interactions.clear();
}

bool InteractionHashMap2::erase(unsigned int id1,unsigned int id2)
{
	if (id1>id2)
		swap(id1,id2);

	unsigned int oldSize = interactions.size();
	pair<unsigned int,unsigned int> p(id1,id2);
	unsigned int size = interactions.erase(p);

	return size!=oldSize;

}

bool InteractionHashMap2::find(unsigned int id1,unsigned int id2,shared_ptr<Interaction>& i)
{
	if (id1>id2)
		swap(id1,id2);

	hmii = interactions.find(pair<unsigned int,unsigned int>(id1,id2));
	if (hmii!=interactions.end())
	{
		i = (*hmii).second;
		return true;
	}
	else
		return false;
}

 shared_ptr<Interaction>InteractionHashMap2::getFirst()
{
	hmii    = interactions.begin();
	hmiiEnd = interactions.end();
	if(hmii == hmiiEnd)
		return shared_ptr<Interaction>();
	else
		return (*hmii).second;
}

bool InteractionHashMap2::hasCurrent()
{
	return ( hmii != hmiiEnd );
}

shared_ptr<Interaction> InteractionHashMap2::getNext()
{
	if(hasCurrent() && ++hmii != hmiiEnd)
		return (*hmii).second;
	else
		return shared_ptr<Interaction>();
}

shared_ptr<Interaction> InteractionHashMap2::getCurrent()
{
	if(hasCurrent())
		return (*hmii).second;
	else
		return shared_ptr<Interaction>();
}

unsigned int InteractionHashMap2::size()
{
	return interactions.size();
}
