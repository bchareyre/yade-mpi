/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __InteractionHashMap_HPP__
#define __InteractionHashMap_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/InteractionContainer.hpp>
#include <yade/yade-core/Interaction.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <ext/hash_map>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;
using namespace __gnu_cxx;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

struct eqPair
{
	bool operator()(const pair<unsigned int,unsigned int>& p1, const pair<unsigned int,unsigned int>& p2) const
	{
		return (p1.first==p2.first && p1.second==p2.second);
	}
};

struct hashPair
{
	unsigned int operator()(const pair<unsigned int,unsigned int>& p) const
	{
		//return (p.first);
		//return (p.first+p.second);
		return (p.first+p.second)%182501621;
	}
};

typedef hash_map<pair<unsigned int,unsigned int>, shared_ptr<Interaction>, hashPair, eqPair > IHashMap;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class InteractionHashMap : public InteractionContainer
{
	private : IHashMap interactions;
	private : shared_ptr<Interaction> empty;


	public : InteractionHashMap();
	public : virtual ~InteractionHashMap();

	public : virtual bool insert(unsigned int id1,unsigned int id2);
	public : virtual bool insert(shared_ptr<Interaction>& i);
	public : virtual void clear();
	public : virtual bool erase(unsigned int id1,unsigned int id2);
	public : virtual const shared_ptr<Interaction>& find(unsigned int id1,unsigned int id2);

	public    : virtual InteractionContainer::iterator begin();
        public    : virtual InteractionContainer::iterator end();

// 	public	: virtual void gotoFirst();
// 	public	: virtual bool notAtEnd();
// 	public	: virtual void gotoNext();
// 	public	: virtual void gotoFirstPotential();
// 	public	: virtual bool notAtEndPotential();
// 	public	: virtual void gotoNextPotential();
	
	//public	: virtual const shared_ptr<Interaction>& getCurrent();

	//public : virtual void eraseCurrentAndGotoNext();
	//public  : virtual void eraseCurrentAndGotoNextPotential();

	public : virtual unsigned int size();

	REGISTER_CLASS_NAME(InteractionHashMap);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(InteractionHashMap,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __InteractionHashMap_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
