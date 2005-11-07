/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONHASHMAP_HPP
#define INTERACTIONHASHMAP_HPP

#include <yade/yade-core/InteractionContainer.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <ext/hash_map>

using namespace std;
using namespace __gnu_cxx;

struct eqPair
{
	bool operator()(const pair<unsigned int,unsigned int>& p1, const pair<unsigned int,unsigned int>& p2) const
	{
		return (p1.first==p2.first && p1.second==p2.second);
	}
};

struct hashPair
{
	unsigned int operator()(const pair<unsigned int,unsigned int>& p) const
	{
		return (p.first+p.second)%182501621;
	}
};

typedef hash_map<pair<unsigned int,unsigned int>, shared_ptr<Interaction>, hashPair, eqPair > IHashMap;

class InteractionHashMap : public InteractionContainer
{
	private :
		IHashMap interactions;
		shared_ptr<Interaction> empty;

	public :
		InteractionHashMap();
		virtual ~InteractionHashMap();

		virtual bool insert(unsigned int id1,unsigned int id2);
		virtual bool insert(shared_ptr<Interaction>& i);
		virtual void clear();
		virtual bool erase(unsigned int id1,unsigned int id2);
		virtual const shared_ptr<Interaction>& find(unsigned int id1,unsigned int id2);
	
		virtual InteractionContainer::iterator begin();
	        virtual InteractionContainer::iterator end();
	
		virtual unsigned int size();

	REGISTER_CLASS_NAME(InteractionHashMap);
	REGISTER_BASE_CLASS_NAME(InteractionContainer);
};

REGISTER_SERIALIZABLE(InteractionHashMap,false);

#endif //  INTERACTIONHASHMAP_HPP

