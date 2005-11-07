/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BODYREDIRECTIONVECTORITERATOR_HPP
#define BODYREDIRECTIONVECTORITERATOR_HPP

#include <yade/yade-core/BodyContainerIterator.hpp>

#include <vector>

using namespace std;

class BodyRedirectionVectorIterator : public BodyContainerIterator 
{
	public :
		vector< shared_ptr<Body> >::iterator vii;

		BodyRedirectionVectorIterator();
		~BodyRedirectionVectorIterator();

		virtual bool isDifferent(const BodyContainerIterator& i);
		virtual void affect(const BodyContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Body> getValue();
		virtual shared_ptr<BodyContainerIterator> createPtr();

};

#endif // __BODYREDIRECTIONVECTORITERATOR__

