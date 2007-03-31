/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTIONCONTAINERITERATOR_HPP
#define PHYSICALACTIONCONTAINERITERATOR_HPP

#include <boost/shared_ptr.hpp>
#include <iostream>
#include "PhysicalAction.hpp"

using namespace boost;
using namespace std;

class PhysicalActionContainerIterator 
{
	public :
		int currentIndex;

		PhysicalActionContainerIterator() 		{};
		virtual ~PhysicalActionContainerIterator()	{};
	
		virtual bool isDifferent(const PhysicalActionContainerIterator&)	{ throw;};
		virtual void affect(const PhysicalActionContainerIterator&)		{ throw;};
		virtual void increment()						{ throw;};
		virtual shared_ptr<PhysicalAction> getValue()				{ throw;};
		virtual shared_ptr<PhysicalActionContainerIterator> createPtr()		{ throw;};
		virtual int getCurrentIndex()						{ throw;};
};

#endif // PHYSICALACTIONCONTAINERITERATOR_HPP

