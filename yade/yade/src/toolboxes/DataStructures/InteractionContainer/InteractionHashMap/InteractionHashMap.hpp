#ifndef __InteractionHashMap_HPP__
#define __InteractionHashMap_HPP__

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
				//return (p.first);
				//return (p.first+p.second);
				return (p.first+p.second)%182501621;
			}
		};

	private : typedef hash_map<pair<unsigned int,unsigned int>, shared_ptr<Interaction>, hashPair, eqPair > IHashMap;
	private : IHashMap interactions;
	private : IHashMap::iterator hmii;
	private : IHashMap::iterator hmiiEnd;
	private : shared_ptr<Interaction> empty;


	public : InteractionHashMap();
	public : virtual ~InteractionHashMap();

	public : virtual bool insert(unsigned int id1,unsigned int id2);
	public : virtual bool insert(shared_ptr<Interaction>& i);
	public : virtual void clear();
	public : virtual bool erase(unsigned int id1,unsigned int id2);
	public : virtual const shared_ptr<Interaction>& find(unsigned int id1,unsigned int id2);

	public	: virtual void gotoFirst();
	public	: virtual bool notAtEnd();
	public	: virtual void gotoNext();
	public	: virtual void gotoFirstPotential();
	public	: virtual bool notAtEndPotential();
	public	: virtual void gotoNextPotential();
	
	public	: virtual const shared_ptr<Interaction>& getCurrent();

	public : virtual void eraseCurrentAndGotoNext();
	public  : virtual void eraseCurrentAndGotoNextPotential();

	public : virtual unsigned int size();

	REGISTER_CLASS_NAME(InteractionHashMap);
};

REGISTER_SERIALIZABLE(InteractionHashMap,false);

#endif // __InteractionHashMap_HPP__
