/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionHashMapIterator.hpp"


InteractionHashMapIterator::InteractionHashMapIterator() : InteractionContainerIterator()
{

}


InteractionHashMapIterator::~InteractionHashMapIterator()
{

}


bool InteractionHashMapIterator::isDifferent(const InteractionContainerIterator& i)
{
	return (hmii != static_cast<const InteractionHashMapIterator&>(i).hmii );
}


void InteractionHashMapIterator::increment()
{
	++hmii;
}


void InteractionHashMapIterator::affect(const InteractionContainerIterator& i)
{
	hmii = static_cast<const InteractionHashMapIterator&>(i).hmii;
}


shared_ptr<Interaction> InteractionHashMapIterator::getValue()
{
	return (*hmii).second;
}


shared_ptr<InteractionContainerIterator> InteractionHashMapIterator::createPtr()
{
	return shared_ptr<InteractionContainerIterator>(new InteractionHashMapIterator());
}


