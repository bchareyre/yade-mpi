/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionContainer.hpp>
#include<yade/core/Interaction.hpp>


#if __GNUC__<4 || (__GNUC__==4 && __GNUC_MINOR__<=2)
	// use this in gcc<=4.2
	// see also the corresponding IHashMap typedef below
	#include<ext/hash_map>
#else
	#include<tr1/unordered_map>
#endif

#include<vector>

using namespace std;
using namespace __gnu_cxx;




struct eqPair
{
	bool operator()(const pair<body_id_t,body_id_t>& p1, const pair<body_id_t,body_id_t>& p2) const
	{
		return (p1.first==p2.first && p1.second==p2.second);
	}
};

struct hashPair
{
	unsigned int operator()(const pair<body_id_t,body_id_t>& p) const
	{
		return ((unsigned int)p.first+(unsigned int)p.second)%182501621;
	}
};

#if __GNUC__<4 || (__GNUC__==4 && __GNUC_MINOR__<=2)
	typedef hash_map<pair<body_id_t,body_id_t>, shared_ptr<Interaction>, hashPair, eqPair > IHashMap;
#else
	typedef tr1::unordered_map<pair<body_id_t,body_id_t>, shared_ptr<Interaction>, hashPair, eqPair > IHashMap;
#endif

class InteractionHashMap : public InteractionContainer
{
	private :
		IHashMap interactions;
		shared_ptr<Interaction> empty;

	public :
		InteractionHashMap();
		virtual ~InteractionHashMap();

		virtual bool insert(body_id_t id1,body_id_t id2);
		virtual bool insert(shared_ptr<Interaction>& i);
		virtual void clear();
		virtual bool erase(body_id_t id1,body_id_t id2);
		virtual const shared_ptr<Interaction>& find(body_id_t id1,body_id_t id2);
	
		virtual InteractionContainer::iterator begin();
	        virtual InteractionContainer::iterator end();
	
		virtual unsigned int size();

	REGISTER_CLASS_NAME(InteractionHashMap);
	REGISTER_BASE_CLASS_NAME(InteractionContainer);
};

class InteractionHashMapIterator : public InteractionContainerIterator 
{
	public :
		IHashMap::iterator hmii;

		InteractionHashMapIterator();
		~InteractionHashMapIterator();

		virtual bool isDifferent(const InteractionContainerIterator& i);
		virtual void affect(const InteractionContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Interaction> getValue();
		virtual shared_ptr<InteractionContainerIterator> createPtr();

};

REGISTER_SERIALIZABLE(InteractionHashMap);

