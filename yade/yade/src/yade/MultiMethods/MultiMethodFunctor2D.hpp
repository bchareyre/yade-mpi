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

#ifndef __MULTIMETHODFUNCTOR2D_H__
#define __MULTIMETHODFUNCTOR2D_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "Factorable.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all collision functor.

	Every functions that describe collision between two CollisionGeometrys must derived from MultiMethodFunctor2D.
*/
class MultiMethodFunctor2D : public Factorable
{

	protected : bool reverse;

	public : void setReverse(bool r);
	public : bool isReverse();

	// construction
	public : MultiMethodFunctor2D ();
	public : virtual ~MultiMethodFunctor2D ();

	public : virtual const string getFunctorOrder() const;

// FIXME : it would be better to use typelist here
// 	protected : virtual bool collide(const shared_ptr<CollisionGeometry> , const shared_ptr<CollisionGeometry> , const Se3r& , const Se3r& , shared_ptr<Interaction> ) { throw; };
// 	protected : virtual bool reverseCollide(const shared_ptr<CollisionGeometry> , const shared_ptr<CollisionGeometry> ,  const Se3r& , const Se3r& , shared_ptr<Interaction> ) { throw; };
// 
// 	public    : inline bool operator() (const shared_ptr<CollisionGeometry> cm1, const shared_ptr<CollisionGeometry> cm2, const Se3r& se31, const Se3r& se32, shared_ptr<Interaction> c)
// 	{
// 		if (reverse)
// 			return reverseCollide(cm1,cm2,se31,se32,c);
// 		else
// 			return collide(cm1,cm2,se31,se32,c);
// 	}

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

// don't forget to use this macro so that Dispatcher can static_cast before calling the Functor
// so inside the functor we already have the right type.

#define DEFINE_FUNCTOR_ORDER(class1,class2)							\
	public : virtual bool checkFunctorOrder(const string& suggestedOrder) const	\
	{											\
		return ((string(#class1)+" "+string(#class2))==suggestedOrder);			\
	}											\


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __MULTIMETHODFUNCTOR2D_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
