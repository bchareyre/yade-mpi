/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CONTACT_HPP
#define CONTACT_HPP

#include<Wm3Math.h>
#include<Wm3Vector3.h>
#include<Wm3Quaternion.h>
#include<yade/lib-base/yadeWm3.hpp>

#include<vector>
#include<set>

using namespace std;

struct Contact
{
	///Interaction
	int		 id;
	bool		 isReal
			,isNew;	
	
	///SpheresContactGeometry
	Real		 radius1
			,radius2
			,penetrationDepth;

	Vector3r	 normal				// new unit normal of the contact plane.
			,contactPoint;	

	///ElasticContactInteraction
	Real		 kn				// normal elastic constant.
			,ks				// shear elastic constant.
			,initialKn			// initial normal elastic constant.
			,initialKs			// initial shear elastic constant.
			,equilibriumDistance		// equilibrium distance
			,initialEquilibriumDistance	// initial equilibrium distance
			,frictionAngle			// angle of friction, according to Coulumb criterion
			,tangensOfFrictionAngle;

	Vector3r	 prevNormal			// unit normal of the contact plane.
			,normalForce			// normal force applied on a DE
			,shearForce;			// shear force applied on a DE

};

struct lessThanContact
{
	bool operator()(const Contact& c1, const Contact& c2)
	{
		return (c1.id<c2.id);
	};
};

typedef set<Contact,lessThanContact> ContactSet;
typedef vector<ContactSet> ContactVecSet;

#endif // __CONTACT_HPP__

