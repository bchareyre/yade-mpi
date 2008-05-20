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

#include"InteractionContainerIteratorPointer.hpp"
#include<yade/lib-serialization/Serializable.hpp>
#include<boost/thread/mutex.hpp>
#include<boost/range.hpp>

class Interaction;

using namespace boost;

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

	private :
		vector<shared_ptr<Interaction> > interaction;

	protected :
		virtual void registerAttributes();
		virtual void preProcessAttributes(bool deserializing);
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(InteractionContainer);
	REGISTER_BASE_CLASS_NAME(Serializable);
};

REGISTER_SERIALIZABLE(InteractionContainer,false);

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

