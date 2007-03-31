/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONVECSETITERATOR_HPP
#define INTERACTIONVECSETITERATOR_HPP


#include<yade/core/InteractionContainerIterator.hpp>
#include <vector>

using namespace std;

struct lessThanPair
{
	bool operator()(const pair<unsigned int,shared_ptr<Interaction> >& p1, const pair<unsigned int,shared_ptr<Interaction> >& p2) const
	{
		return (p1.first<p2.first);
	}
};

class InteractionVecSetIterator : public InteractionContainerIterator 
{
	public :
		vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator vii;
		vector<set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair > >::iterator viiEnd;
		set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator sii;
		set<pair<unsigned int,shared_ptr<Interaction> >,lessThanPair >::iterator siiEnd;

		InteractionVecSetIterator();
		~InteractionVecSetIterator();

		virtual bool isDifferent(const InteractionContainerIterator& i);
		virtual void affect(const InteractionContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Interaction> getValue();
		virtual shared_ptr<InteractionContainerIterator> createPtr();

};

#endif // INTERACTIONVECSETITERATOR_HPP

