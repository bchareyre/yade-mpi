#ifndef __InteractionHashMap2_HPP__
#define __InteractionHashMap2_HPP__

#include "InteractionContainer.hpp"
#include "Interaction.hpp"
#include <ext/hash_map>

using namespace std;
using namespace __gnu_cxx;

class InteractionHashMap2 : public InteractionContainer
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
				//return (p.first+p.second);
				return (p.first+p.second)%182501621;
			}
		};

	private : typedef hash_map<pair<unsigned int,unsigned int>, shared_ptr<Interaction>, hashPair, eqPair > IHashMap;
	private : IHashMap interactions;
	private : IHashMap::iterator hmii;
	private : IHashMap::iterator hmiiEnd;


	public : InteractionHashMap2();
	public : ~InteractionHashMap2();

	public : bool insert(shared_ptr<Interaction>& i);
	public : void clear();
	public : bool erase(unsigned int id1,unsigned int id2);
	public : bool find(unsigned int id1,unsigned int id2,shared_ptr<Interaction>& i);
	public : shared_ptr<Interaction> getFirst();
	public : bool hasCurrent();
	public : shared_ptr<Interaction> getNext();
	public : shared_ptr<Interaction> getCurrent();
	public : unsigned int size();

	REGISTER_CLASS_NAME(InteractionHashMap2);
};

REGISTER_SERIALIZABLE(InteractionHashMap2,false);

#endif // __InteractionHashMap2_HPP__
