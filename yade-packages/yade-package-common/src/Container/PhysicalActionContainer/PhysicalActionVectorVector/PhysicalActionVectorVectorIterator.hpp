/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTIONVECTORVECTORITERATOR_HPP
#define PHYSICALACTIONVECTORVECTORITERATOR_HPP

#include <yade/yade-core/PhysicalActionContainerIterator.hpp>
#include <vector>

using namespace std;

class PhysicalActionVectorVectorIterator : public PhysicalActionContainerIterator
{
	public :
		vector< vector< shared_ptr<PhysicalAction> > >::iterator vvi;
		vector< vector< shared_ptr<PhysicalAction> > >::iterator vviEnd;
			vector< shared_ptr<PhysicalAction> >  ::iterator vi;
		vector< shared_ptr<PhysicalAction> >  ::iterator viEnd;
		vector< bool > * usedIds;

	public :
		PhysicalActionVectorVectorIterator();
		~PhysicalActionVectorVectorIterator();

		virtual bool isDifferent(const PhysicalActionContainerIterator& i);
		virtual void affect(const PhysicalActionContainerIterator& i);
		virtual void increment();
		virtual shared_ptr<PhysicalAction> getValue();
		virtual shared_ptr<PhysicalActionContainerIterator> createPtr();
		virtual int getCurrentIndex();
};

#endif // __PHYSICALACTIONVECTORVECTORITERATOR__

