/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SDECLINKPHYSICS_HPP
#define SDECLINKPHYSICS_HPP

#include <yade/yade-core/InteractionPhysics.hpp>

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
		void registerAttributes();
	REGISTER_CLASS_NAME(SDECLinkPhysics);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);

};

REGISTER_SERIALIZABLE(SDECLinkPhysics,false);

#endif // SDECLINKPHYSICS_HPP

