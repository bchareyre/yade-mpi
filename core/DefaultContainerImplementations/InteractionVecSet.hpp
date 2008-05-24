/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once

#include<yade/core/InteractionContainer.hpp>
#include<yade/core/Interaction.hpp>
#include<set>
#include<vector>

using namespace std;

struct lessThanPair
{
	bool operator()(const pair<body_id_t,shared_ptr<Interaction> >& p1, const pair<body_id_t,shared_ptr<Interaction> >& p2) const
	{
		return (p1.first<p2.first);
	}
};

class InteractionVecSetIterator : public InteractionContainerIterator 
{
	public :
		vector<set<pair<body_id_t,shared_ptr<Interaction> >,lessThanPair > >::iterator vii;
		vector<set<pair<body_id_t,shared_ptr<Interaction> >,lessThanPair > >::iterator viiEnd;
		set<pair<body_id_t,shared_ptr<Interaction> >,lessThanPair >::iterator sii;
		set<pair<body_id_t,shared_ptr<Interaction> >,lessThanPair >::iterator siiEnd;

		InteractionVecSetIterator();
		~InteractionVecSetIterator();

		virtual bool isDifferent(const InteractionContainerIterator& i);
		virtual void affect(const InteractionContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Interaction> getValue();
		virtual shared_ptr<InteractionContainerIterator> createPtr();

};


using namespace __gnu_cxx;

class InteractionVecSet : public InteractionContainer
{
	private :
		vector<set<pair<body_id_t,shared_ptr<Interaction> >,lessThanPair > > interactions;
		unsigned int currentSize;
		shared_ptr<Interaction> empty;


	public :
		InteractionVecSet();
		virtual ~InteractionVecSet();

		virtual bool insert(body_id_t id1,body_id_t id2);
		virtual bool insert(shared_ptr<Interaction>& i);
		virtual void clear();
		virtual bool erase(body_id_t id1,body_id_t id2);
		virtual const shared_ptr<Interaction>& find(body_id_t id1,body_id_t id2);

		virtual InteractionContainer::iterator begin();
     	virtual InteractionContainer::iterator end();

		virtual unsigned int size();

	REGISTER_CLASS_NAME(InteractionVecSet);
	REGISTER_BASE_CLASS_NAME(InteractionContainer);

};

REGISTER_SERIALIZABLE(InteractionVecSet,false);

