/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionPhysics.hpp>

class SDECLinkPhysics : public InteractionPhysics
{
	public :	
		Real		 kn				// normal elastic constant.
				,ks				// shear elastic constant.

				,knMax
				,ksMax
				,heta
				,averageRadius
				,kr

				,initialKn			// initial normal elastic constant.
				,initialKs			// initial shear elastic constant.
				,equilibriumDistance		// equilibrium distance
				,initialEquilibriumDistance;	// initial equilibrium distance

		Vector3r	 prevNormal			// unit normal of the contact plane.
				,normalForce			// normal force applied on a DE
				,shearForce			// shear force applied on a DE
				,thetar;

		Quaternionr	 prevRotation1
				,prevRotation2;
		
		
		SDECLinkPhysics();
		virtual ~SDECLinkPhysics();

/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(InteractionPhysics,(initialKn)(initialKs)(initialEquilibriumDistance)(knMax)(ksMax)(heta));
	REGISTER_CLASS_NAME(SDECLinkPhysics);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);

	REGISTER_CLASS_INDEX(SDECLinkPhysics,InteractionPhysics);
};

REGISTER_SERIALIZABLE(SDECLinkPhysics);


