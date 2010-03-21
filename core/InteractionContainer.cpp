// 2008 © Sergei Dorofeenko <sega@users.berlios.de>
// 2009,2010 © Václav Šmilauer <eudoxos@arcig.cz>

#include "InteractionContainer.hpp"

#ifdef YADE_OPENMP
	#include<omp.h>
#endif

#ifdef YADE_BOOST_SERIALIZATION
	BOOST_CLASS_EXPORT(InteractionContainer);
#endif

bool InteractionContainer::insert(const shared_ptr<Interaction>& i)
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
		if (intrs.size() == currentSize)
			intrs.resize(currentSize+1);

		intrs[currentSize]=i;
		currentSize++;
	
		return true;
	}
	else
		return false;
}


bool InteractionContainer::insert(body_id_t id1,body_id_t id2)
{
	shared_ptr<Interaction> i(new Interaction(id1,id2) );
	return insert(i);	
}


void InteractionContainer::clear()
{
	boost::mutex::scoped_lock lock(drawloopmutex);

	vecmap.clear();
	intrs.clear();
	pendingErase.clear();
	currentSize=0;
}


bool InteractionContainer::erase(body_id_t id1,body_id_t id2)
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
				intrs[iid]=intrs[currentSize];
				id1 = intrs[iid]->getId1();
				id2 = intrs[iid]->getId2();
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


const shared_ptr<Interaction>& InteractionContainer::find(body_id_t id1,body_id_t id2)
{
	if (id1>id2)
		swap(id1,id2);

	if (static_cast<unsigned int>(id1)<vecmap.size())
	{
		map<body_id_t,unsigned int >::iterator mii;
		mii = vecmap[id1].find(id2);
		if (mii!=vecmap[id1].end())
			return intrs[(*mii).second];
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

void InteractionContainer::requestErase(body_id_t id1, body_id_t id2, bool force){
	find(id1,id2)->reset(); IdsForce v={id1,id2,force};
	#ifdef YADE_OPENMP
		threadsPendingErase[omp_get_thread_num()].push_back(v);
	#else
		pendingErase.push_back(v);
	#endif
}

void InteractionContainer::clearPendingErase(){
	#ifdef YADE_OPENMP
		FOREACH(list<IdsForce>& pendingErase, threadsPendingErase){
			pendingErase.clear();
		}
	#else
		pendingErase.clear();
	#endif
}

int InteractionContainer::unconditionalErasePending(){
	int ret=0;
	#ifdef YADE_OPENMP
		// shadow this->pendingErase by the local variable, to share code
		FOREACH(list<IdsForce>& pendingErase, threadsPendingErase){
	#endif
			if(!pendingErase.empty()){
				FOREACH(const IdsForce& p, pendingErase){ ret++; erase(p.id1,p.id2); }
				pendingErase.clear();
			}
	#ifdef YADE_OPENMP
		}
	#endif
	return ret;
}

void InteractionContainer::eraseNonReal(){
	typedef pair<int,int> Ids;
	std::list<Ids> ids;
	FOREACH(const shared_ptr<Interaction>& i, *this){
		if(!i->isReal()) ids.push_back(Ids(i->getId1(),i->getId2()));
	}
	FOREACH(const Ids& id, ids){
		this->erase(id.first,id.second);
	}
}

// compare interaction based on their first id
struct compPtrInteraction{
	bool operator() (const shared_ptr<Interaction>& i1, const shared_ptr<Interaction>& i2) const {
		return (*i1)<(*i2);
	}
};

void InteractionContainer::preProcessAttributes(bool deserializing){
	if(deserializing) { interaction.clear(); return; } 
	FOREACH(const shared_ptr<Interaction>& I, *this){
		if(I->interactionGeometry || I->interactionPhysics) interaction.push_back(I);
		// since requestErase'd interactions have no interaction physics/geom, they are not saved
	}
	if(serializeSorted) std::sort(interaction.begin(),interaction.end(),compPtrInteraction());
}

void InteractionContainer::postProcessAttributes(bool deserializing){
	if(!deserializing){interaction.clear(); return;}
	clear();
	FOREACH(const shared_ptr<Interaction>& I, interaction){ insert(I); }
	interaction.clear();
}

