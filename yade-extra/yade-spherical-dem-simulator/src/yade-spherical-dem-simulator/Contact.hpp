/***************************************************************************
 *   Copyright (C) 2005 by Olivier Galizzi   *
 *   olivier.galizzi@imag.fr   *
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

#ifndef __CONTACT_HPP__
#define __CONTACT_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-wm3-math/Math.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-wm3-math/Quaternion.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <set>

using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct Contact
{
	///Interaction
	int id;
	bool isReal;
	bool isNew;	
	
	///SpheresContactGeometry
	Real radius1;
	Real radius2;
	Vector3r normal;			// new unit normal of the contact plane.
	Real penetrationDepth;
	Vector3r contactPoint;	

	///ElasticContactParameters
	Real kn;				// normal elastic constant.
	Real ks;				// shear elastic constant.
	Real initialKn;				// initial normal elastic constant.
	Real initialKs;				// initial shear elastic constant.
	Real equilibriumDistance;		// equilibrium distance
	Real initialEquilibriumDistance;	// initial equilibrium distance
	Vector3r prevNormal;			// unit normal of the contact plane.
	Vector3r normalForce;			// normal force applied on a DE
	Vector3r shearForce;			// shear force applied on a DE
	Real frictionAngle; 			// angle of friction, according to Coulumb criterion
	Real tangensOfFrictionAngle;

};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

struct lessThanContact
{
	bool operator()(const Contact& c1, const Contact& c2)
	{
		return (c1.id<c2.id);
	};
};

typedef set<Contact,lessThanContact> ContactSet;
typedef vector<ContactSet> ContactVecSet;

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __CONTACT_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
