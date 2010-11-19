// 2008 © Sergei Dorofeenko <sega@users.berlios.de>
// 2009,2010 © Václav Šmilauer <eudoxos@arcig.cz>

#include "InteractionContainer.hpp"

#ifdef YADE_OPENMP
	#include<omp.h>
#endif

// begin internal functions

bool InteractionContainer::insert(const shared_ptr<Interaction>& i){
	assert(bodies);
	boost::mutex::scoped_lock lock(drawloopmutex);
	Body::id_t id1=i->getId1(), id2=i->getId2();
	if (id1>id2) swap(id1,id2); 
	assert(bodies->size()>id1); assert(bodies->size()>id2); // the bodies must exist already
	const shared_ptr<Body>& b1=(*bodies)[id1]; // body with the smaller id will hold the pointer
	if(!b1->intrs.insert(Body::MapId2IntrT::value_type(id2,i)).second) return false; // already exists
	//assert(linIntrs.size()==currSize);
	linIntrs.resize(++currSize); // currSize updated
	//assert(linIntrs.size()==currSize);
	linIntrs[currSize-1]=i; // assign last element
	i->linIx=currSize-1; // store the index back-reference in the interaction (so that it knows how to erase/move itself)
	return true;
}


void InteractionContainer::clear(){
	assert(bodies);
	boost::mutex::scoped_lock lock(drawloopmutex);
	FOREACH(const shared_ptr<Body>& b, *bodies) b->intrs.clear(); // delete interactions from bodies
	linIntrs.clear(); // clear the linear container
	pendingErase.clear();
	currSize=0;
}


bool InteractionContainer::erase(Body::id_t id1,Body::id_t id2){
	assert(bodies);
	boost::mutex::scoped_lock lock(drawloopmutex);
	if (id1>id2) swap(id1,id2);
	assert(id1<bodies->size() && id2<bodies->size()); // (possibly) existing ids
	const shared_ptr<Body>& b1((*bodies)[id1]); assert(b1); // get the body; check it is not deleted
	Body::MapId2IntrT::iterator I(b1->intrs.find(id2));
	// this used to return false
	if(I==b1->intrs.end()) throw std::logic_error(("InteractionContainer::erase: attempt to delete non-existent interaction ##"+lexical_cast<string>(id1)+"+"+lexical_cast<string>(id2)).c_str());
	// erase from body and then from linIntrs as well
	int linIx=I->second->linIx; 
	b1->intrs.erase(I);
	// iid is not the last element; we have to move last one to its place
	if (linIx<currSize-1) {
		linIntrs[linIx]=linIntrs[currSize-1];
		linIntrs[linIx]->linIx=linIx; // update the back-reference inside the interaction
	}
	//assert(linIntrs.size()==currSize);
	// in either case, last element can be removed now
	linIntrs.resize(--currSize); // currSize updated
	//assert(linIntrs.size()==currSize);
	return true;
}


const shared_ptr<Interaction>& InteractionContainer::find(Body::id_t id1,Body::id_t id2){
	assert(bodies);
	if (id1>id2) swap(id1,id2);
	assert(id1<bodies->size() && id2<bodies->size());
	const shared_ptr<Body>& b1((*bodies)[id1]); assert(b1);
	Body::MapId2IntrT::iterator I(b1->intrs.find(id2));
	if (I!=b1->intrs.end()) return I->second;
	else { empty=shared_ptr<Interaction>(); return empty; }
}

// end internal functions

// the rest uses internal functions to access data structures, and does not have to be modified if they change

bool InteractionContainer::insert(Body::id_t id1,Body::id_t id2)
{
	shared_ptr<Interaction> i(new Interaction(id1,id2) );
	return insert(i);	
}


void InteractionContainer::requestErase(Body::id_t id1, Body::id_t id2, bool force){
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

void InteractionContainer::preSave(InteractionContainer&){
	FOREACH(const shared_ptr<Interaction>& I, *this){
		if(I->geom || I->phys) interaction.push_back(I);
		// since requestErase'd interactions have no interaction physics/geom, they are not saved
	}
	if(serializeSorted) std::sort(interaction.begin(),interaction.end(),compPtrInteraction());
}
void InteractionContainer::postSave(InteractionContainer&){ interaction.clear(); }


void InteractionContainer::preLoad(InteractionContainer&){ interaction.clear(); }

void InteractionContainer::postLoad__calledFromScene(const shared_ptr<BodyContainer>& bb){
	bodies=&bb->body; // update the internal pointer
	clear();
	FOREACH(const shared_ptr<Interaction>& I, interaction){ insert(I); }
	interaction.clear();
}

