/***************************************************************************
 *   Copyright (C) 2005 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "PhysicalActionVectorVectorIterator.hpp"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionVectorVectorIterator::PhysicalActionVectorVectorIterator() : PhysicalActionContainerIterator()
{
	currentIndex = -1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

PhysicalActionVectorVectorIterator::~PhysicalActionVectorVectorIterator()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool PhysicalActionVectorVectorIterator::isDifferent(const PhysicalActionContainerIterator& i)
{
	const PhysicalActionVectorVectorIterator& it = static_cast<const PhysicalActionVectorVectorIterator&>(i);
	if (it.vvi==it.vviEnd)
		return !(vvi==vviEnd);
	else
		return (vi != it.vi );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<PhysicalAction> PhysicalActionVectorVectorIterator::getValue()
{	
	return *vi;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int PhysicalActionVectorVectorIterator::getCurrentIndex()
{
	return currentIndex;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

shared_ptr<PhysicalActionContainerIterator> PhysicalActionVectorVectorIterator::createPtr()
{
	return shared_ptr<PhysicalActionContainerIterator>(new PhysicalActionVectorVectorIterator());
}
