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

#ifndef __TERRAIN2SPHERE4CLOSESTFEATURES_H__
#define __TERRAIN2SPHERE4CLOSESTFEATURES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "CollisionFunctor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Provide collision handling between a Terrain and a Sphere in terms of ClosestFeatures */
class Terrain2Sphere4ClosestFeatures : public CollisionFunctor
{
	// construction
	public : Terrain2Sphere4ClosestFeatures ();
	public : virtual ~Terrain2Sphere4ClosestFeatures ();

	protected : virtual bool collide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2, const Se3& se31, const Se3& se32, shared_ptr<Contact> c);
	protected : virtual bool reverseCollide(const shared_ptr<CollisionModel> cm1, const shared_ptr<CollisionModel> cm2,  const Se3& se31, const Se3& se32, shared_ptr<Contact> c);

	// FIXME : put into toolbox and serialization and everything
	private : float sqrDistTriPoint(const Vector3& p, const std::vector<Vector3>& tri, Vector3& pt);

// FIXME : this should be just Factorable, not Serializable !!
	public    : virtual void registerAttributes() {};
	REGISTER_CLASS_NAME(Terrain2Sphere4ClosestFeatures);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_CLASS(Terrain2Sphere4ClosestFeatures,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __TERRAIN2SPHERE4CLOSESTFEATURES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
