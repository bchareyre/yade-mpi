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

// BOOST_FOREACH compatibility
#ifndef FOREACH
#  define FOREACH BOOST_FOREACH
#endif

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
		boost::mutex	drawloopmutex;

		InteractionContainer() { };
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

		// std::pair is not handle by yade::serialization, use vector<body_id_t> instead
		typedef vector<body_id_t> bodyIdPair;
		//! Ask for erasing the interaction given (from the constitutive law); this resets the interaction (to the initial=potential state)
		//! and collider should traverse pendingErase to decide whether to delete the interaction completely or keep it potential
		void requestErase(body_id_t id1, body_id_t id2);
		/*! List of pairs of interactions that will be (maybe) erased by the collider;
			
			If accessed from within a parallel section, pendingEraseMutex must be locked (this is done inside requestErase for you)
			If there is, at one point, a multi-threaded collider, pendingEraseMutex should be moved to the public part and used from there as well.
		*/
		list<bodyIdPair> pendingErase;
		/*! Erase all pending interactions unconditionally.

			This should be called only in rare cases that collider is not used but still interactions should be erased.
			Otherwise collider should decide on a case-by-case basis, which interaction to erase for good and which to keep in the potential state
			(without interactionGeometry and interactionPhysics).

			This function doesn't lock pendingEraseMutex, as it is (supposedly) called from no-parallel sections only once per iteration
		*/
		void unconditionalErasePending();
		/*! Traverse all pending interactions and erase them if the (T*)->shouldBeErased(id1,id2) return true
			and keep it if it return false; finally, pendingErase will be clear()'ed.

			Class using this interface (which is presumably a collider) must define the 
					
				bool shouldBeErased(body_id_t, body_id_t) const

			method which will be called for every interaction.
		*/
		template<class T> void erasePending(const T& t){
			FOREACH(const vector<body_id_t>& p, pendingErase){ if(t.shouldBeErased(p[0],p[1])) erase(p[0],p[1]); }
			pendingErase.clear();
		}
	private :
		#ifdef YADE_OPENMP
			// This is used only from within requestErase() for now, therefore it can be private
			boost::mutex pendingEraseMutex;
		#endif
		// used only during serialization/deserialization
		vector<shared_ptr<Interaction> > interaction;
	protected :
		virtual void preProcessAttributes(bool deserializing);
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(/*no base*/,(interaction)(pendingErase));
	REGISTER_CLASS_AND_BASE(InteractionContainer,Serializable);
};

REGISTER_SERIALIZABLE(InteractionContainer);


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

