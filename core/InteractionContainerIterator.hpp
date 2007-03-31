/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTIONCONTAINERITERATOR_HPP
#define INTERACTIONCONTAINERITERATOR_HPP


#include <boost/shared_ptr.hpp>
#include <iostream>
#include "Interaction.hpp"

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

#endif // INTERACTIONCONTAINERITERATOR_HPP

