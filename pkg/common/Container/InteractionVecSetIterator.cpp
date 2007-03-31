/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "InteractionVecSetIterator.hpp"

InteractionVecSetIterator::InteractionVecSetIterator() : InteractionContainerIterator()
{

}


InteractionVecSetIterator::~InteractionVecSetIterator()
{

}


bool InteractionVecSetIterator::isDifferent(const InteractionContainerIterator& i)
{
	const InteractionVecSetIterator& it = static_cast<const InteractionVecSetIterator&>(i);
	if (it.vii == it.viiEnd) // we are at end of container
		return !(vii==viiEnd);
	else
		return (sii != it.sii );
}


void InteractionVecSetIterator::increment()
{
	if ( sii != siiEnd )
		++sii;
	while( sii == siiEnd )
	{
		++vii;
		if(vii != viiEnd)
		{
			sii	= (*vii).begin();
			siiEnd	= (*vii).end();
		}
		else
			break;
	}
}


void InteractionVecSetIterator::affect(const InteractionContainerIterator& i)
{
	const InteractionVecSetIterator& tmpi = static_cast<const InteractionVecSetIterator&>(i);
	vii    = tmpi.vii;
	viiEnd = tmpi.viiEnd;
	sii    = tmpi.sii;
	siiEnd = tmpi.siiEnd;
}


shared_ptr<Interaction> InteractionVecSetIterator::getValue()
{
	return (*sii).second;
}


shared_ptr<InteractionContainerIterator> InteractionVecSetIterator::createPtr()
{
	return shared_ptr<InteractionContainerIterator>(new InteractionVecSetIterator());
}


