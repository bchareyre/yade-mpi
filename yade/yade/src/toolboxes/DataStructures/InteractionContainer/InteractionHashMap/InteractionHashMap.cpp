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

bool InteractionHashMap::find(unsigned int id1,unsigned int id2,shared_ptr<Interaction>& i)
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

void InteractionHashMap::gotoFirst()
{
	hmii    = interactions.begin();
	hmiiEnd = interactions.end();
//	if(hmii == hmiiEnd)
//		return shared_ptr<Interaction>();
//	else
//		return (*hmii).second;
}

bool InteractionHashMap::notAtEnd()
{
	return ( hmii != hmiiEnd );
}

void InteractionHashMap::gotoNext()
{
	++hmii;
//	if(hasCurrent() && ++hmii != hmiiEnd)
//		return (*hmii).second;
//	else
//		return shared_ptr<Interaction>();
}

shared_ptr<Interaction> InteractionHashMap::getCurrent()
{
//	if(hasCurrent())
		return (*hmii).second;
//	else
//		return shared_ptr<Interaction>();
}


// FIXME - is it absolutely correct ??
void InteractionHashMap::eraseCurrentAndGotoNext()
{
	//cout << "InteractionHashMap : eraseCurrentAndGotoNext" << endl;
	if(notAtEnd())
	{
		IHashMap::iterator tmpHmii=hmii;
		++hmii;
		interactions.erase(tmpHmii);
////////		return getCurrent();
	}

//	else
//		return shared_ptr<Interaction>();
}
/*
void InteractionHashMap::eraseCurrent()
{
	if(hasCurrent())
	{
		IHashMap::iterator tmpHmii = hmii;

		++tmpHmii;
		if( tmpHmii != interactions.end() )
		{
			int id1 = tmpHmii->second -> getId1();
			int id2 = tmpHmii->second -> getId2();

			interactions.erase(hmii);
			shared_ptr<Interaction> iiii;
			find(id1,id2,iiii);
		}
		else
		{
			interactions.erase(hmii);
			hmii = interactions.end();
		}
	}
}
*/

unsigned int InteractionHashMap::size()
{
	return interactions.size();
}
