/*
#ifndef __INTERACTIONHASHMAP_HPP__
#define __INTERACTIONHASHMAP_HPP__

#include "InteractionContainer.hpp"
#include "Interaction.hpp"
#include <ext/hash_map>

using namespace std;
using namespace __gnu_cxx;

class InteractionHashMap : public InteractionContainer
{
	private : struct eqPair
		{
			bool operator()(const pair<unsigned int,unsigned int>& p1, const pair<unsigned int,unsigned int>& p2) const
			{
				return (p1.first==p2.first && p1.second==p2.second);
			}
		};

	private : struct hashPair
		{
			unsigned int operator()(const pair<unsigned int,unsigned int>& p) const
			{
				return (p.first);
			}
		};

	private	: typedef hash_map<pair<unsigned int,unsigned int>, shared_ptr<Interaction>, hashPair, eqPair > IHashMap;
	private : IHashMap interactions;
	private : IHashMap::iterator hmii;
	private : IHashMap::iterator hmiiEnd;


	public	: InteractionHashMap();
	public	: virtual ~InteractionHashMap();

	public	: virtual bool insert(shared_ptr<Interaction>& i);
	public	: virtual void clear();
	public	: virtual bool erase(unsigned int id1,unsigned int id2);
	public	: virtual bool find(unsigned int id1,unsigned int id2,shared_ptr<Interaction>& i);

	public	: virtual void gotoFirst();
	public	: virtual bool notAtEnd();
	public	: virtual void gotoNext();
	public	: virtual shared_ptr<Interaction> getCurrent();

//	public	: virtual shared_ptr<Interaction> getFirst();
// 	public	: virtual bool hasCurrent();
// 	public	: virtual shared_ptr<Interaction> getNext();
// 	public	: virtual shared_ptr<Interaction> getCurrent();
	
	public	: virtual shared_ptr<Interaction> eraseCurrent();

	public	: virtual unsigned int size();

	REGISTER_CLASS_NAME(InteractionHashMap);
};

REGISTER_SERIALIZABLE(InteractionHashMap,false);

#endif // __INTERACTIONHASHMAP_HPP__
*/