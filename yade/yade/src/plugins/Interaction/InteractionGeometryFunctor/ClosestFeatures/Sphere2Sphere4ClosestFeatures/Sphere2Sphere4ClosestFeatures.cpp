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

#include "Sphere2Sphere4ClosestFeatures.hpp"
#include "InteractionSphere.hpp"
#include "ClosestFeatures.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Sphere4ClosestFeatures::go(		const shared_ptr<InteractionDescription>& cm1,
						const shared_ptr<InteractionDescription>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	shared_ptr<InteractionSphere> s1 = dynamic_pointer_cast<InteractionSphere>(cm1);
	shared_ptr<InteractionSphere> s2 = dynamic_pointer_cast<InteractionSphere>(cm2);
	
	Vector3r v = se31.position-se32.position;
	Real l = v.normalize();
	
	if (l<s1->radius+s2->radius)
	{
		shared_ptr<ClosestFeatures> cf = shared_ptr<ClosestFeatures>(new ClosestFeatures());
		cf->closestsPoints.push_back(std::pair<Vector3r,Vector3r>(se31.position-v*s1->radius,se32.position+v*s2->radius));
		c->interactionGeometry = cf;
		return true;
	}
	else	
		return false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Sphere4ClosestFeatures::goReverse(	const shared_ptr<InteractionDescription>& cm1,
						const shared_ptr<InteractionDescription>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
