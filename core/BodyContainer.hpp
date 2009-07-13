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

#include<iostream>
#include<boost/range.hpp>
#include<yade/lib-serialization/Serializable.hpp>

class Body;

using namespace boost;
using namespace std;

class BodyContainerIterator 
{
	public :
		BodyContainerIterator() 		{};
		virtual ~BodyContainerIterator()	{};
		
		virtual bool isDifferent(const BodyContainerIterator&)	{ throw;};
		virtual void affect(const BodyContainerIterator&)	{ throw;};
		virtual void increment()				{ throw;};
		virtual shared_ptr<Body> getValue()			{ throw;};
		virtual shared_ptr<BodyContainerIterator> createPtr()	{ throw;};
};

class BodyContainerIteratorPointer
{
	private :
		shared_ptr<BodyContainerIterator> ptr;
		void allocate(const BodyContainerIteratorPointer& bi)
		{
			if (ptr==0) ptr = bi.get()->createPtr();
		}


	public :
		BodyContainerIterator&			getRef()	{ return *ptr; };
		BodyContainerIterator&			getRef() const	{ return *ptr; };
		shared_ptr<BodyContainerIterator>	get()		{ return  ptr; };
		shared_ptr<BodyContainerIterator>	get() const	{ return  ptr; };

		BodyContainerIteratorPointer(const shared_ptr<BodyContainerIterator>& i)	{ ptr = i; };

		BodyContainerIteratorPointer()		{ ptr = shared_ptr<BodyContainerIterator>(); };

		BodyContainerIteratorPointer(const BodyContainerIteratorPointer& bi) 
		{
			allocate(bi);
			ptr->affect(bi.getRef());
		};

		bool operator!=(const BodyContainerIteratorPointer& bi)	{ return ptr->isDifferent(bi.getRef());	};
		bool operator==(const BodyContainerIteratorPointer& bi)	{ return !ptr->isDifferent(bi.getRef());	};
		shared_ptr<Body>			operator*()	{ return ptr->getValue(); };	
		BodyContainerIteratorPointer&		operator++()	{ ptr->increment(); return *this; };
		BodyContainerIteratorPointer&		operator++(int); // disabled 
		BodyContainerIteratorPointer& operator=(const BodyContainerIteratorPointer& bi)
		{
			allocate(bi);
			ptr->affect(bi.getRef());
			return *this;
		};

};



class BodyContainer : public Serializable
{
	public :
		BodyContainer();
		virtual ~BodyContainer();

	
		virtual unsigned int insert(shared_ptr<Body>&)			{throw;}; //FIXME : use const shared_ptr<Body>&
		virtual unsigned int insert(shared_ptr<Body>& , unsigned int)	{throw;};
		virtual void clear() 						{throw;};
		virtual bool erase(unsigned int) 				{throw;};
		virtual bool find(unsigned int , shared_ptr<Body>&) const	{throw;};
		virtual bool exists(unsigned int id) const 			{throw;};
		virtual shared_ptr<Body>& operator[](unsigned int) {throw;};
		virtual const shared_ptr<Body>& operator[](unsigned int) const {throw;};
		
		typedef BodyContainerIteratorPointer iterator;
		virtual BodyContainer::iterator begin()				{throw;};
		virtual BodyContainer::iterator end()				{throw;};
		
		virtual unsigned int size() const {throw;};

	protected :
		void setId(shared_ptr<Body>& , unsigned int);

	REGISTER_CLASS_AND_BASE(BodyContainer,Serializable);

	// local storage for uniform serialization of all possible container concrete implementations.
	private :
		vector<shared_ptr<Body> > body; 
	public :
		REGISTER_ATTRIBUTES(/*no base*/,(body));
	protected :
		virtual void preProcessAttributes(bool deserializing);
		virtual void postProcessAttributes(bool deserializing);
};

REGISTER_SERIALIZABLE(BodyContainer);

/* for BOOST_FOREACH compatibility */
#ifndef FOREACH
#	define FOREACH BOOST_FOREACH
#endif

namespace boost{
	template<> struct range_iterator<BodyContainer>{ typedef BodyContainer::iterator type; };  
	template<> struct range_const_iterator<BodyContainer>{ typedef BodyContainer::iterator type; }; 
}

inline BodyContainer::iterator boost_range_begin(BodyContainer& bc){ return bc.begin(); }
inline BodyContainer::iterator boost_range_end(BodyContainer& bc){ return bc.end(); }
/* //not needed (and const iterator is not supported by BodyContainer either)
inline BodyContainer::iterator boost_range_begin(BodyContainer const & bc){ return bc.begin(); }
inline BodyContainer::iterator boost_range_end(BodyContainer const & bc){ return bc.end(); }
*/
namespace std{
	template<> struct iterator_traits<BodyContainer::iterator>{
		typedef forward_iterator_tag iterator_category;
		typedef shared_ptr<Body> reference;
	};
}

