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

#ifndef __BOX2BOX4CLOSESTFEATURES_H__
#define __BOX2BOX4CLOSESTFEATURES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "CollisionFunctor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Provide collision handling between a axis aligned box and a sphere in terms of ClosestFeatures */
class Box2Box4ClosestFeatures : public CollisionFunctor
{
	private : typedef struct BoxBoxCollisionInfo
	{
		bool isNormalPrincipalAxis;
		bool invertNormal;
		float penetrationDepth;
		int code;
		Vector3 normal;
	} BoxBoxCollisionInfo;

	// construction
	public : Box2Box4ClosestFeatures ();
	public : virtual ~Box2Box4ClosestFeatures ();

	protected : virtual bool collide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Interaction> c);
	protected : virtual bool reverseCollide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,  const Se3& se31, const Se3& se32, shared_ptr<Interaction> c);

	private : bool testSeparatingAxis(float expr1, float expr2, Vector3 n,int c,BoxBoxCollisionInfo* bbInfo);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_FACTORABLE(Box2Box4ClosestFeatures);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOX2BOX4CLOSESTFEATURES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
