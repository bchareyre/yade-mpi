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
#include <set>
#include <boost/shared_ptr.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "ClassFactory.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class Se3;
class Interaction;
class CollisionModel;
class CollisionFunctor;

using namespace boost;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class MultiMethodsManager
{
	protected : std::vector<std::vector<shared_ptr<CollisionFunctor> > > callBacks;
	private   : vector<string> indexedClassName;//indexedCollisionGeometry;
//	private   : vector<string> indexedInteractionModel;

	// construction
	public : MultiMethodsManager ();

	public : virtual ~MultiMethodsManager ();

	public : bool addCollisionGeometry(const string& name);
//	public : bool addInteractionModel(const string& name);
//	public : bool extendCallBacksTable();

	public : bool go(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Interaction> c);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//#define REGISTER_COLLISION_GEOMETRY(SomeClass)


#define REGISTER_CLASS_TO_MULTI_METHODS_MANAGER(SomeClass)	\
	const bool indexed##SomeClass = Omega::instance().narrowCollider.addCollisionGeometry(#SomeClass);\

//#define REGISTER_INTERACTION_MODEL(SomeClass)
//	const bool indexed##SomeClass = Omega::instance().narrowCollider.addInteractionModel(#SomeClass);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////


#endif // __MULTIMETHODSMANAGER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
