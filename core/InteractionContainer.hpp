/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONCONTAINER_HPP
#define INTERACTIONCONTAINER_HPP

#include<yade/lib-serialization/Serializable.hpp>
#include "InteractionContainerIteratorPointer.hpp"
#include <boost/thread/mutex.hpp>

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

#endif // INTERACTIONCONTAINER_HPP

