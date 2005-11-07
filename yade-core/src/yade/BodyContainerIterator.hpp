/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


#ifndef BODYCONTAINERITERATOR_HPP
#define BODYCONTAINERITERATOR_HPP

#include <boost/shared_ptr.hpp>
#include <iostream>

#include "Body.hpp"

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


#endif // BODYCONTAINERITERATOR_HPP


