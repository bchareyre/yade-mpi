#include "InteractionVecSet.hpp"
#include <iostream>

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

void InteractionVecSet::clear()
{
	interactions.clear();
	currentSize=0;
}

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

bool InteractionVecSet::find(unsigned int id1,unsigned int id2,shared_ptr<Interaction>& i)
{
	if (id1>id2)
		swap(id1,id2);

	if (static_cast<unsigned int>(id1)<interactions.size())
	{
		shared_ptr<Interaction> tmpI;
		sii = interactions[id1].find(pair<unsigned int,shared_ptr<Interaction> >(id2,tmpI));
		if (sii!=interactions[id1].end())
		{
			i = (*sii).second;
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

// FIXME : this has to be really efficient


//shared_ptr<Interaction> InteractionVecSet::getFirst()
void InteractionVecSet::gotoFirst()
{
	vii    = interactions.begin();
	viiEnd = interactions.end();

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

//bool InteractionVecSet::hasCurrent()
bool InteractionVecSet::notAtEnd()
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

//shared_ptr<Interaction> InteractionVecSet::getNext()
void InteractionVecSet::gotoNext()
{
	if ( sii != siiEnd )
		++sii;
	while( sii == siiEnd )
	{
		++vii;
		if(vii != viiEnd)
		{
//			return shared_ptr<Interaction>();
			sii	= (*vii).begin();
			siiEnd	= (*vii).end();
		}
		else
			break;
	}
//	return (*sii).second;
}

//shared_ptr<Interaction> InteractionVecSet::getCurrent()
shared_ptr<Interaction> InteractionVecSet::getCurrent()
{
//	if(hasCurrent())
		return (*sii).second;
//	else
//		return shared_ptr<Interaction>();
}

// FIXME - make sure that everything in this file is fast and has no mistakes!
//shared_ptr<Interaction>  InteractionVecSet::eraseCurrent()

void InteractionVecSet::eraseCurrentAndGotoNext()
{
	//cout << "InteractionVecSet : eraseCurrentAndGotoNext" << endl;
	
	vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator tmpVii = vii;
	set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator tmpSii          = sii;
	
	//int id1 = (*sii).second->getId1();
	//int id2 = (*sii).second->getId2();
	
	gotoNext();
	
	//if (!erase(id1,id2))
	//{
	//	cout << "not erased" << endl;
	//}
	(*tmpVii).erase(tmpSii);
	currentSize--;	


}

unsigned int InteractionVecSet::size()
{
	return currentSize;
}

