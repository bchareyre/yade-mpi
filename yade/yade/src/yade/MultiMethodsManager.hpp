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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MULTIMETHODSMANAGER_H__
#define __MULTIMETHODSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Indexable;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

using namespace boost;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

template<class Functor>
class MultiMethodsManager 
{
	protected : std::vector<std::vector<shared_ptr<Functor> > > callBacks;
	//private   : vector<string> indexedClassName;
	//indexedCollisionGeometry;
	//private   : vector<string> indexedInteractionModel;

	// construction
	public : MultiMethodsManager ();
	public : virtual ~MultiMethodsManager ();

	protected : virtual bool addPair(const string& name1,const string& name2,const string& libName) = 0;
	protected : bool add(shared_ptr<Indexable>& i1,shared_ptr<Indexable>& i2, const string& name1, const string& name2, const string& libName);

	//public : bool go(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Interaction> c);
};   

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "MultiMethodsManager.tpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __MULTIMETHODSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
