////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "InteractionVecSet.hpp"
#include "InteractionVecSetIterator.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iostream>
// FIXME : this has to be really efficient

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

InteractionVecSet::InteractionVecSet()
{
	currentSize = 0;
	clear();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

InteractionVecSet::~InteractionVecSet()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool InteractionVecSet::insert(shared_ptr<Interaction>& i)
{
	unsigned int id1 = i->getId1();
	unsigned int id2 = i->getId2();

	if (id1>id2)
		swap(id1,id2);

	if ( id1 >=interactions.size())
		interactions.resize(id1+1);

	if (interactions[id1].insert(pair<unsigned int,shared_ptr<Interaction> >(id2,i)).second)
	{
		currentSize++;
		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool InteractionVecSet::insert(unsigned int id1,unsigned int id2)
{
	shared_ptr<Interaction> i(new Interaction(id1,id2) );
	return insert(i);	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void InteractionVecSet::clear()
{
	interactions.clear();
	currentSize=0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

bool InteractionVecSet::erase(unsigned int id1,unsigned int id2)
{
	if (id1>id2)
		swap(id1,id2);

	if ( id1 < interactions.size())
	{
		shared_ptr<Interaction> tmpI;
		if (interactions[id1].erase(pair<unsigned int,shared_ptr<Interaction> >(id2,tmpI)))
		{
			currentSize--;
			return true;
		}
		else
			return false;
	}

	return false;

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

const shared_ptr<Interaction>& InteractionVecSet::find(unsigned int id1,unsigned int id2)
{
	if (id1>id2)
		swap(id1,id2);

	if (static_cast<unsigned int>(id1)<interactions.size())
	{
		set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator sii;
		shared_ptr<Interaction> tmpI;
		sii = interactions[id1].find(pair<unsigned int,shared_ptr<Interaction> >(id2,tmpI));
		if (sii!=interactions[id1].end())
			return (*sii).second;
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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

InteractionContainer::iterator InteractionVecSet::begin()
{
	shared_ptr<InteractionVecSetIterator> it(new InteractionVecSetIterator());
	it->vii    = interactions.begin();
	it->viiEnd = interactions.end();
 
	if (it->vii!=it->viiEnd)
	{
		it->sii    = (*it->vii).begin();
 		it->siiEnd = (*it->vii).end();
	
		while( it->sii == it->siiEnd )
		{
			++it->vii;
			if(it->vii != it->viiEnd)
			{
				it->sii	   = (*it->vii).begin();
				it->siiEnd = (*it->vii).end();
			}
			else
				return InteractionContainer::iterator(it);
		}
	}

	return InteractionContainer::iterator(it);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

InteractionContainer::iterator InteractionVecSet::end()
{

	shared_ptr<InteractionVecSetIterator> it(new InteractionVecSetIterator());

	it->vii		= interactions.end();
	it->viiEnd	= interactions.end();
	
// in fact it is not possible to assign ssi, because it doesn't exist at all. (both begin() and end() do not exist)

//	it->sii		= interactions.begin()->end();
//	it->siiEnd	= interactions.begin()->end();
// trying to access out of memory bounds: end() points behind LAST element. so accessing it causes segfault.
//	it->sii		= (*it->vii).end();
//	it->siiEnd	= (*it->vii).end();

	return InteractionContainer::iterator(it);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

// 
// void InteractionVecSet::eraseCurrentAndGotoNextPotential()
// {
// 	vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator tmpVii = vii;
// 	set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator tmpSii          = sii;
// 	
// 	gotoNextPotential();
// 	
// 	(*tmpVii).erase(tmpSii);
// 	currentSize--;	
// }
// 
// void InteractionVecSet::eraseCurrentAndGotoNext()
// {
// 	vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator tmpVii = vii;
// 	set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator tmpSii          = sii;
// 	
// 	gotoNext();
// 	
// 	(*tmpVii).erase(tmpSii);
// 	currentSize--;	
// 	
// }

unsigned int InteractionVecSet::size()
{
	return currentSize;
}

