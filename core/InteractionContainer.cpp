/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionContainer.hpp"
#include<omp.h>

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


struct compPtrInteraction{
	bool operator() (const shared_ptr<Interaction>& i1, const shared_ptr<Interaction>& i2) const {
		return (*i1)<(*i2);
	}
};

void InteractionContainer::preProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		interaction.clear();
	}
	else
	{
		interaction.clear(); interaction.reserve(this->size());
		InteractionContainer::iterator i    = this->begin();
		InteractionContainer::iterator iEnd = this->end();
		for( ; i!=iEnd ; ++i ){
			if((*i)->interactionGeometry || (*i)->interactionPhysics) interaction.push_back(*i);
		}
		if(serializeSorted) std::sort(interaction.begin(),interaction.end(),compPtrInteraction());
		// since requestErase'd interactions have no interaction physics/geom, they were not saved anyway
		#if 0 // ifdef YADE_OPENMP
			// copy per-thread erasable pairs to the global attribute that will be serialized
			FOREACH(const list<bodyIdPair>& threadPendingErase, threadsPendingErase){ pendingErase.insert(pendingErase.end(),threadPendingErase.begin(),threadPendingErase.end()); }
		#endif
	}
}


void InteractionContainer::postProcessAttributes(bool deserializing)
{
	if(deserializing)
	{
		this->clear();
		vector<shared_ptr<Interaction> >::iterator it    = interaction.begin();
		vector<shared_ptr<Interaction> >::iterator itEnd = interaction.end();
		for( ; it != itEnd ; ++it)
			this->insert(*it);
		interaction.clear();
		#if 0 // ifdef YADE_OPENMP
			// copy global deserialized pairs to be erased to the list of thread #0
			threadsPendingErase[0].insert(threadsPendingErase[0].end(),pendingErase.begin(),pendingErase.end());
		#endif
		// forc compatibility, if the list was saved
		pendingErase.clear();
	}
	else
	{
		interaction.clear();
	}
}



