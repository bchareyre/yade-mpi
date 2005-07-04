#include "InteractionVecSet.hpp"
#include <iostream>
// FIXME : this has to be really efficient

InteractionVecSet::InteractionVecSet()
{
	currentSize = 0;
	clear();
}

InteractionVecSet::~InteractionVecSet()
{
}

bool InteractionVecSet::insert(shared_ptr<Interaction>& i)
{
	unsigned int id1 = i->getId1();
	unsigned int id2 = i->getId2();

	if (id1>id2)
		swap(id1,id2);

	if ( id1 >=volatileInteractions.size())
		volatileInteractions.resize(id1+1);

	if (volatileInteractions[id1].insert(pair<unsigned int,shared_ptr<Interaction> >(id2,i)).second)
	{
		currentSize++;
		return true;
	}
	else
		return false;
}

bool InteractionVecSet::insert(unsigned int id1,unsigned int id2)
{
	shared_ptr<Interaction> i(new Interaction(id1,id2) );
	return insert(i);	
};

void InteractionVecSet::clear()
{
	volatileInteractions.clear();
	currentSize=0;
}

bool InteractionVecSet::erase(unsigned int id1,unsigned int id2)
{
	if (id1>id2)
		swap(id1,id2);

	if ( id1 < volatileInteractions.size())
	{
		shared_ptr<Interaction> tmpI;
		if (volatileInteractions[id1].erase(pair<unsigned int,shared_ptr<Interaction> >(id2,tmpI)))
		{
			currentSize--;
			return true;
		}
		else
			return false;
	}

	return false;

}

const shared_ptr<Interaction>& InteractionVecSet::find(unsigned int id1,unsigned int id2)
{
	if (id1>id2)
		swap(id1,id2);

	if (static_cast<unsigned int>(id1)<volatileInteractions.size())
	{
		shared_ptr<Interaction> tmpI;
		sii = volatileInteractions[id1].find(pair<unsigned int,shared_ptr<Interaction> >(id2,tmpI));
		if (sii!=volatileInteractions[id1].end())
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

void InteractionVecSet::gotoFirstPotential()
{
	vii    = volatileInteractions.begin();
	viiEnd = volatileInteractions.end();

	if (vii!=viiEnd)
	{
		sii    = (*vii).begin();
		siiEnd = (*vii).end();
	
		while( sii == siiEnd )
		{
			++vii;
			if(vii != viiEnd)
			{
				sii	= (*vii).begin();
				siiEnd	= (*vii).end();
			}
			else
				return;
		}
	}
}

bool InteractionVecSet::notAtEndPotential()
{
	set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator tmpSii          = sii;
	vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator tmpVii = vii;

	if( vii == viiEnd )
		return false;

	while (tmpSii==siiEnd)
	{
		++tmpVii;
		if (tmpVii==viiEnd)
			return false;
		tmpSii = (*vii).begin();
		siiEnd = (*vii).end();
	}
	return true;
}

void InteractionVecSet::gotoNextPotential()
{
	if ( sii != siiEnd )
		++sii;
	while( sii == siiEnd )
	{
		++vii;
		if(vii != viiEnd)
		{
			sii	= (*vii).begin();
			siiEnd	= (*vii).end();
		}
		else
			break;
	}
}

void InteractionVecSet::gotoFirst()
{
	gotoFirstPotential();
	while (notAtEnd() && !getCurrent()->isReal)
		gotoNextPotential();
}

void InteractionVecSet::gotoNext()
{
	gotoNextPotential();
	while( notAtEnd() && !getCurrent()->isReal)
		gotoNextPotential();
}

bool InteractionVecSet::notAtEnd()
{
	return notAtEndPotential();
}

const shared_ptr<Interaction>& InteractionVecSet::getCurrent()
{
		return (*sii).second;
}

void InteractionVecSet::eraseCurrentAndGotoNextPotential()
{
	vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator tmpVii = vii;
	set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator tmpSii          = sii;
	
	gotoNextPotential();
	
	(*tmpVii).erase(tmpSii);
	currentSize--;	
}

void InteractionVecSet::eraseCurrentAndGotoNext()
{
	vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator tmpVii = vii;
	set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator tmpSii          = sii;
	
	gotoNext();
	
	(*tmpVii).erase(tmpSii);
	currentSize--;	
	
}

unsigned int InteractionVecSet::size()
{
	return currentSize;
}

