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
#include "InteractionDescription.hpp"
#include "BoundingVolume.hpp"
#include "BoundingVolumeFactoryFunctor.hpp"
#include "Body.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class BoundingVolumeFactoryDispatcher : public Actor
{
	protected: DynLibDispatcher
		<	TYPELIST_2( InteractionDescription , BoundingVolume ) ,	// base classess for dispatch
			BoundingVolumeFactoryFunctor,					// class that provides multivirtual call
			void ,							// return type
			TYPELIST_3(
					  const shared_ptr<InteractionDescription>&	// arguments
					, shared_ptr<BoundingVolume>&
					, const Se3r&
				)
		> bvFactoriesManager;

	private : vector<vector<string> > bvFactories;
	public  : void addBVFactories(const string& str1,const string& str2,const string& str3);

	// construction
	public : BoundingVolumeFactoryDispatcher ();
	public : ~BoundingVolumeFactoryDispatcher ();

	public : void registerAttributes();
	public : void postProcessAttributes(bool deserializing);
	public : virtual void action(Body* b);
	public : void updateBoundingVolume(Body* b);
	public : void updateBoundingVolume(shared_ptr<Body> b);
	REGISTER_CLASS_NAME(BoundingVolumeFactoryDispatcher);

	public : bool isActivated();

};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(BoundingVolumeFactoryDispatcher,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOUNDINGVOLUMEUPDATOR_HPP__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
