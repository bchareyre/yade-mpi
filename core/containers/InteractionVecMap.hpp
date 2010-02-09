// 2008 © Sergei Dorofeenko <sega@users.berlios.de>
#pragma once

#include<yade/core/InteractionContainer.hpp>
#include<yade/core/Interaction.hpp>
#include<map>
#include<vector>

using namespace std;

class InteractionVecMapIterator : public InteractionContainerIterator 
{
	public :

		vector<shared_ptr<Interaction>  >::iterator vii;
		InteractionVecMapIterator();
		~InteractionVecMapIterator();

		virtual bool isDifferent(const InteractionContainerIterator& i);
		virtual void affect(const InteractionContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Interaction> getValue();
		virtual shared_ptr<InteractionContainerIterator> createPtr();


};


using namespace __gnu_cxx;

class InteractionVecMap : public InteractionContainer
{
	private :
		vector<shared_ptr<Interaction> > interactions;
		vector<map<body_id_t, unsigned int  > > vecmap;
		unsigned int currentSize;
		shared_ptr<Interaction> empty;

	public :
		InteractionVecMap();
		virtual ~InteractionVecMap();

		virtual bool insert(body_id_t id1,body_id_t id2);
		virtual bool insert(shared_ptr<Interaction>& i);
		virtual void clear();
		virtual bool erase(body_id_t id1,body_id_t id2);
		virtual const shared_ptr<Interaction>& find(body_id_t id1,body_id_t id2);

		virtual InteractionContainer::iterator begin();
     	virtual InteractionContainer::iterator end();
		
		virtual shared_ptr<Interaction>& operator[](unsigned int);
		virtual const shared_ptr<Interaction>& operator[](unsigned int) const;

		virtual unsigned int size();

	REGISTER_ATTRIBUTES(InteractionContainer,);
	REGISTER_CLASS_NAME(InteractionVecMap);
	REGISTER_BASE_CLASS_NAME(InteractionContainer);

};

REGISTER_SERIALIZABLE(InteractionVecMap);

