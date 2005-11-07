/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONHASHMAPITERATOR_HPP
#define INTERACTIONHASHMAPITERATOR_HPP

#include <yade/yade-core/InteractionContainerIterator.hpp>
#include <vector>
#include "InteractionHashMap.hpp"

using namespace std;

class InteractionHashMapIterator : public InteractionContainerIterator 
{
	public :
		IHashMap::iterator hmii;

		InteractionHashMapIterator();
		~InteractionHashMapIterator();

		virtual bool isDifferent(const InteractionContainerIterator& i);
		virtual void affect(const InteractionContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Interaction> getValue();
		virtual shared_ptr<InteractionContainerIterator> createPtr();

};

#endif // INTERACTIONHASHMAPITERATOR_HPP

