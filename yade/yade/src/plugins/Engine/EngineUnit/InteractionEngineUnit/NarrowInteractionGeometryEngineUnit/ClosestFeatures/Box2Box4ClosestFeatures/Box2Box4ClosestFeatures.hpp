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

#include <yade-common/InteractionGeometryEngineUnit.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Provide interaction handling between a axis aligned box and a sphere in terms of ClosestFeatures */
class Box2Box4ClosestFeatures : public InteractionGeometryEngineUnit
{
	private : typedef struct BoxBoxInteractionInfo
	{
		bool isNormalPrincipalAxis;
		bool invertNormal;
		Real penetrationDepth;
		int code;
		Vector3r normal;
	} BoxBoxInteractionInfo;

	public : virtual bool go(		const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c);
	public : virtual bool goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c);

	private : bool testSeparatingAxis(Real expr1, Real expr2, Vector3r n,int c,BoxBoxInteractionInfo* bbInfo);

	REGISTER_CLASS_NAME(Box2Box4ClosestFeatures);
	DEFINE_FUNCTOR_ORDER_2D(InteractingBox,InteractingBox);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Box2Box4ClosestFeatures,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __BOX2BOX4CLOSESTFEATURES_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
