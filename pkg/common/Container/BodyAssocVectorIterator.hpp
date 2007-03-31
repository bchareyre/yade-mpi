/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BODYASSOCVECTORITERATOR
#define BODYASSOCVECTORITERATOR

#include<yade/core/BodyContainerIterator.hpp>

#include <vector>

using namespace std;

class BodyAssocVectorIterator : public BodyContainerIterator 
{
	public :
		Loki::AssocVector<unsigned int , shared_ptr<Body> >::iterator bii; // FIXME - private maybe?

		BodyAssocVectorIterator();
		~BodyAssocVectorIterator();
		
		virtual bool isDifferent(const BodyContainerIterator& i);
		virtual void affect(const BodyContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<Body> getValue();
		virtual shared_ptr<BodyContainerIterator> createPtr();

};

#endif // __BODYASSOCVECTORITERATOR__

