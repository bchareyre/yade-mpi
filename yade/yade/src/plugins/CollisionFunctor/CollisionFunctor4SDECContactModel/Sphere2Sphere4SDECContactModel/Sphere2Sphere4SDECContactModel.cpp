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

#include "Sphere2Sphere4SDECContactModel.hpp"
#include "Sphere.hpp"
#include "SDECContactModel.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Sphere4SDECContactModel::go(	const shared_ptr<CollisionGeometry>& cm1,
						const shared_ptr<CollisionGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c)
{
	shared_ptr<Sphere> s1 = dynamic_pointer_cast<Sphere>(cm1);
	shared_ptr<Sphere> s2 = dynamic_pointer_cast<Sphere>(cm2);

	Vector3r normal = se32.translation-se31.translation;
	float penetrationDepth = s1->radius+s2->radius-normal.normalize();

	if (penetrationDepth>0)
	{
		// FIXME : remove those uncommented lines
		shared_ptr<SDECContactModel> scm;
		if (c->interactionGeometry)
			scm = dynamic_pointer_cast<SDECContactModel>(c->interactionGeometry);
		else
			scm = shared_ptr<SDECContactModel>(new SDECContactModel());
			
		scm->contactPoint = se31.translation+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
		scm->normal = normal;
		scm->penetrationDepth = penetrationDepth;
		scm->radius1 = s1->radius;
		scm->radius2 = s2->radius;
				
		if (!c->interactionGeometry)
			c->interactionGeometry = scm;
	
// FIXME : uncommente those lines	
/////////////////////////////////////////////////
// 		shared_ptr<SDECContactModel> scm = shared_ptr<SDECContactModel>(new SDECContactModel());
// 		scm->contactPoint = se31.translation+(s1->radius-0.5*penetrationDepth)*normal;//0.5*(pt1+pt2);
// 		scm->normal = normal;
// 		scm->penetrationDepth = penetrationDepth;
// 		scm->radius1 = s1->radius;
// 		scm->radius2 = s2->radius;
// 		c->interactionGeometry = scm;
/////////////////////////////////////////		
		return true;
	}
	else	
		return false;

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

bool Sphere2Sphere4SDECContactModel::goReverse(	const shared_ptr<CollisionGeometry>& cm1,
						const shared_ptr<CollisionGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						shared_ptr<Interaction>& c)
{
	return go(cm1,cm2,se31,se32,c);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
