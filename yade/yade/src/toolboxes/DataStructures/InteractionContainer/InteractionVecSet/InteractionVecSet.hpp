#ifndef __INTERACTIONVECSET_HPP__
#define __INTERACTIONVECSET_HPP__


#include "InteractionContainer.hpp"
#include "Interaction.hpp"

#include <vector>
#include <set>

using namespace std;
using namespace __gnu_cxx;

class InteractionVecSet : public InteractionContainer
{
	private : struct lessThanPair
		{
			bool operator()(const pair<unsigned int,shared_ptr<Interaction> >& p1, const pair<unsigned int,shared_ptr<Interaction> >& p2) const
			{
				return (p1.first<p2.first);
			}
		};

	private : vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > > interactions;
	private : vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator vii;
	private : vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator viiEnd;
	private : set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator sii;
	private : set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator siiEnd;
	private : unsigned int currentSize;


	public : InteractionVecSet();
	public : virtual ~InteractionVecSet();

	public : virtual bool insert(unsigned int id1,unsigned int id2);
	public : virtual bool insert(shared_ptr<Interaction>& i);
	public : virtual void clear();
	public : virtual bool erase(unsigned int id1,unsigned int id2);
	public : virtual shared_ptr<Interaction> find(unsigned int id1,unsigned int id2);

	public	: virtual void gotoFirst();
	public	: virtual bool notAtEnd();
	public	: virtual void gotoNext();
	public  : virtual void gotoNextPotentialOrReal();
	public	: virtual shared_ptr<Interaction> getCurrent();

	public : virtual void eraseCurrentAndGotoNext();
	
	public : virtual unsigned int size();

	REGISTER_CLASS_NAME(InteractionVecSet);
};

REGISTER_SERIALIZABLE(InteractionVecSet,false);

#endif // __INTERACTIONVECSET_HPP__
	
