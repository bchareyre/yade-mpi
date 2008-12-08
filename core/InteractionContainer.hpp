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

#include<yade/lib-serialization/Serializable.hpp>
#include<boost/thread/mutex.hpp>
#include<iostream>
#include<boost/range.hpp>



#include<yade/core/Interaction.hpp>

using namespace boost;
using namespace std;

class InteractionContainerIterator 
{
	public :
		InteractionContainerIterator() 		{};
		virtual ~InteractionContainerIterator()	{};
		
		virtual bool isDifferent(const InteractionContainerIterator&)	{ throw;};
		virtual void affect(const InteractionContainerIterator&)	{ throw;};
		virtual void increment()					{ throw;};
		virtual shared_ptr<Interaction> getValue()			{ throw;};
		virtual shared_ptr<InteractionContainerIterator> createPtr()	{ throw;};
};


class InteractionContainerIteratorPointer
{
	private :
		shared_ptr<InteractionContainerIterator> ptr;
		void allocate(const InteractionContainerIteratorPointer& bi)
		{
			if (ptr==0)
				ptr = bi.get()->createPtr();
		}


	public  :
		InteractionContainerIterator&			getRef()	{ return *ptr; };
		InteractionContainerIterator&			getRef() const	{ return *ptr; };
		shared_ptr<InteractionContainerIterator>	get()		{ return  ptr; };
		shared_ptr<InteractionContainerIterator>	get() const	{ return  ptr; };

		InteractionContainerIteratorPointer(const InteractionContainerIteratorPointer& bi) 
		{
			allocate(bi);
			ptr->affect(bi.getRef());
		};

		InteractionContainerIteratorPointer(const shared_ptr<InteractionContainerIterator>& i) { ptr = i; };
		InteractionContainerIteratorPointer()  { ptr = shared_ptr<InteractionContainerIterator>(); };

		bool operator!=(const InteractionContainerIteratorPointer& bi) { return ptr->isDifferent(bi.getRef()); };
		bool operator==(const InteractionContainerIteratorPointer& bi) { return !ptr->isDifferent(bi.getRef()); };
		shared_ptr<Interaction>			operator*() { return ptr->getValue(); };	
		InteractionContainerIteratorPointer&	operator++() { ptr->increment(); return *this; };
		InteractionContainerIteratorPointer&	operator++(int); // disabled
		InteractionContainerIteratorPointer&	operator=(const InteractionContainerIteratorPointer& bi)
		{
			allocate(bi);
			ptr->affect(bi.getRef());
			return *this;
		};

};



class InteractionContainer : public Serializable
{
	public :
		boost::mutex	drawloopmutex; // FIXME a hack, containers have to be rewritten lock-free.

		InteractionContainer() { interaction.clear(); };
		virtual ~InteractionContainer() {};

		virtual bool insert(body_id_t /*id1*/,body_id_t /*id2*/)				{throw;};
		virtual bool insert(shared_ptr<Interaction>&)						{throw;};
		virtual void clear() 									{throw;};
		virtual bool erase(body_id_t /*id1*/,body_id_t /*id2*/) 				{throw;};

		virtual const shared_ptr<Interaction>& find(body_id_t /*id1*/,body_id_t /*id2*/) 	{throw;};

		typedef InteractionContainerIteratorPointer iterator;
      virtual InteractionContainer::iterator begin()						{throw;};
      virtual InteractionContainer::iterator end()						{throw;};
		virtual unsigned int size() 								{throw;};

		virtual shared_ptr<Interaction>& operator[] (unsigned int) {throw;};
		virtual const shared_ptr<Interaction>& operator[] (unsigned int) const {throw;};

	private :
		vector<shared_ptr<Interaction> > interaction;

	protected :
		virtual void preProcessAttributes(bool deserializing);
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(/*no base*/,(interaction));
	REGISTER_CLASS_AND_BASE(InteractionContainer,Serializable);
};

REGISTER_SERIALIZABLE(InteractionContainer);

// BOOST_FOREACH compatibility
#ifndef FOREACH
#  define FOREACH BOOST_FOREACH
#endif

namespace boost{
   template<> struct range_iterator<InteractionContainer>{ typedef InteractionContainer::iterator type; };
   template<> struct range_const_iterator<InteractionContainer>{ typedef InteractionContainer::iterator type; };
}
inline InteractionContainer::iterator boost_range_begin(InteractionContainer& ic){ return ic.begin(); }
inline InteractionContainer::iterator boost_range_end(InteractionContainer& ic){ return ic.end(); }
namespace std{
   template<> struct iterator_traits<InteractionContainer::iterator>{
      typedef forward_iterator_tag iterator_category;
      typedef shared_ptr<Interaction> reference;
   };
}

