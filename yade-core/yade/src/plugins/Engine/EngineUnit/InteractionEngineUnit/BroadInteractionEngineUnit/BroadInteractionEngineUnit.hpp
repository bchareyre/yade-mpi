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

#ifndef __SIMPLEBROADCOLLIDER_H__
#define __SIMPLEBROADCOLLIDER_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/BroadInteractor.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class BroadInteractionEngineUnit : public BroadInteractor
{
	// number of potential interacions = number of interacting AABB
	protected : int nbPotentialInteractions;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// construction
	public : BroadInteractionEngineUnit ();
	public : ~BroadInteractionEngineUnit ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	// return a list "volatileInteractions" of pairs of Body which Bounding volume are in potential interaction
	public : void broadInteractionTest(Body* body);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_NAME(BroadInteractionEngineUnit);

};

REGISTER_SERIALIZABLE(BroadInteractionEngineUnit,false);

#endif // __SIMPLEBROADCOLLIDER_H__


/*
	protected : static bool overlappSphereSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
	protected : static inline bool overlappSphereAABB(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2)
	{
		return overlappAABBSphere(bv2,bv1);
	}
	protected : static bool overlappAABBSphere(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
	protected : static bool overlappAABBAABB(const shared_ptr<BoundingVolume> bv1, const shared_ptr<BoundingVolume> bv2);
*/
