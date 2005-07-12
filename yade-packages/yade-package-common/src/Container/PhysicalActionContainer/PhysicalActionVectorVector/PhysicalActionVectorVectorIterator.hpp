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

#ifndef __PHYSICALACTIONVECTORVECTORITERATOR__
#define __PHYSICALACTIONVECTORVECTORITERATOR__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/PhysicalActionContainerIterator.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

class PhysicalActionVectorVectorIterator : public PhysicalActionContainerIterator
{
	public	  : vector< vector< shared_ptr<PhysicalAction> > >::iterator vvi;
	public	  : vector< vector< shared_ptr<PhysicalAction> > >::iterator vviEnd;
	public	  :         vector< shared_ptr<PhysicalAction> >  ::iterator vi;
	public	  :         vector< shared_ptr<PhysicalAction> >  ::iterator viEnd;
 	public    : vector< bool > * usedIds;

	public : PhysicalActionVectorVectorIterator();
	public : ~PhysicalActionVectorVectorIterator();

	public : virtual bool isDifferent(const PhysicalActionContainerIterator& i);
	public : virtual void affect(const PhysicalActionContainerIterator& i);
	public : virtual void increment();
	public : virtual shared_ptr<PhysicalAction> getValue();
	public : virtual shared_ptr<PhysicalActionContainerIterator> createPtr();
	public : virtual int getCurrentIndex();

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __PHYSICALACTIONVECTORVECTORITERATOR__

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
