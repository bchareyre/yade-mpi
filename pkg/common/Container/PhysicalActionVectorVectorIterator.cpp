/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalActionVectorVectorIterator.hpp"


PhysicalActionVectorVectorIterator::PhysicalActionVectorVectorIterator() : PhysicalActionContainerIterator()
{
	currentIndex = -1;
}


PhysicalActionVectorVectorIterator::~PhysicalActionVectorVectorIterator()
{

}


bool PhysicalActionVectorVectorIterator::isDifferent(const PhysicalActionContainerIterator& i)
{
	const PhysicalActionVectorVectorIterator& it = static_cast<const PhysicalActionVectorVectorIterator&>(i);
	if (it.vvi==it.vviEnd)
		return !(vvi==vviEnd);
	else
		return (vi != it.vi );
}


void PhysicalActionVectorVectorIterator::increment()
{
	++vi;
	if(vi == viEnd)
	{
		++vvi;
		++currentIndex;
		while(!(*usedIds)[currentIndex] && vvi != vviEnd)
		{
			++currentIndex;
			++vvi;
		}
		if(vvi != vviEnd)
		{
			vi	= (*vvi).begin();
			viEnd	= (*vvi).end();
		}
	}
}


void PhysicalActionVectorVectorIterator::affect(const PhysicalActionContainerIterator& i)
{
	const PhysicalActionVectorVectorIterator& tmpi = static_cast<const PhysicalActionVectorVectorIterator&>(i);
	vi     = tmpi.vi;
	viEnd  = tmpi.viEnd;
	vvi    = tmpi.vvi;
	vviEnd = tmpi.vviEnd;
	currentIndex = tmpi.currentIndex;
	usedIds	= tmpi.usedIds;
}


shared_ptr<PhysicalAction> PhysicalActionVectorVectorIterator::getValue()
{	
	return *vi;
}


int PhysicalActionVectorVectorIterator::getCurrentIndex()
{
	return currentIndex;
}


shared_ptr<PhysicalActionContainerIterator> PhysicalActionVectorVectorIterator::createPtr()
{
	return shared_ptr<PhysicalActionContainerIterator>(new PhysicalActionVectorVectorIterator());
}
