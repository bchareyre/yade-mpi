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

#ifndef __Sphere2Mesh2D4ClosestFeatures__
#define __Sphere2Mesh2D4ClosestFeatures__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "InteractionGeometryFunctor.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Provide interaction handling between a axis aligned box and a sphere in terms of ClosestFeatures */
class Sphere2Mesh2D4ClosestFeatures : public InteractionGeometryFunctor
{
	public : virtual bool go(		const shared_ptr<InteractionDescription>& cm1,
						const shared_ptr<InteractionDescription>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c);
	public : virtual bool goReverse(	const shared_ptr<InteractionDescription>& cm1,
						const shared_ptr<InteractionDescription>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c);
	
	REGISTER_CLASS_NAME(Sphere2Mesh2D4ClosestFeatures);
	DEFINE_FUNCTOR_ORDER_2D(Sphere,Mesh2D);

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Sphere2Mesh2D4ClosestFeatures,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __Sphere2Mesh2D4ClosestFeatures__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
