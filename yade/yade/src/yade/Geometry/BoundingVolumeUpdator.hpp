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
 
#ifndef __BOUNDINGVOLUMEUPDATOR_HPP__
#define __BOUNDINGVOLUMEUPDATOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Actor.hpp"
#include "DynLibDispatcher.hpp"
#include "CollisionGeometry.hpp"
#include "BoundingVolume.hpp"
#include "Body.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BoundingVolumeUpdator : public Actor
{
	protected: DynLibDispatcher
		<	TYPELIST_2( CollisionGeometry , BoundingVolume ) ,	// base classess for dispatch
			BoundingVolumeFactory,					// class that provides multivirtual call
			void ,							// return type
			TYPELIST_3(
					const shared_ptr<CollisionGeometry>& ,	// arguments
					shared_ptr<BoundingVolume>& ,
					const Se3r&
				)
		> bvFactoriesManager;

	private : vector<vector<string> > bvFactories;
	public  : void addBVFactories(const string& str1,const string& str2,const string& str3);

	// construction
	public : BoundingVolumeUpdator ();
	public : ~BoundingVolumeUpdator ();

	public : void registerAttributes();
	public : void postProcessAttributes(bool deserializing);
	public : virtual void action(Body* b);
	public : void updateBoundingVolume(Body* b);
	public : void updateBoundingVolume(shared_ptr<Body> b);
	REGISTER_CLASS_NAME(BoundingVolumeUpdator);

};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BoundingVolumeUpdator,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOUNDINGVOLUMEUPDATOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
