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

void InteractionContainer::requestErase(body_id_t id1, body_id_t id2){
	find(id1,id2)->reset(); bodyIdPair v(id1,id2);
	#ifdef YADE_OPENMP
		threadsPendingErase[omp_get_thread_num()].push_back(v);
	#else
		pendingErase.push_back(v);
	#endif
}

int InteractionContainer::unconditionalErasePending(){
	int ret=0;
	#ifdef YADE_OPENMP
		// shadow this->pendingErase by the local variable, to share code
		FOREACH(list<bodyIdPair>& pendingErase, threadsPendingErase){
	#endif
			if(pendingErase.size()>0){
				FOREACH(const bodyIdPair& p, pendingErase){ ret++; erase(p[0],p[1]); }
				pendingErase.clear();
			}
	#ifdef YADE_OPENMP
		}
	#endif
	return ret;
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
		for( ; i!=iEnd ; ++i )
			interaction.push_back(*i);
		if(serializeSorted) std::sort(interaction.begin(),interaction.end(),compPtrInteraction());
		#ifdef YADE_OPENMP
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
		#ifdef YADE_OPENMP
			// copy global deserialized pairs to be erased to the list of thread #0
			threadsPendingErase[0].insert(threadsPendingErase[0].end(),pendingErase.begin(),pendingErase.end());
		#endif
	}
	else
	{
		interaction.clear();
	}
}



