/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ELASTIC_CONTACT_PARAMETERS_HPP
#define ELASTIC_CONTACT_PARAMETERS_HPP

#include <yade/yade-core/InteractionPhysics.hpp>

class ElasticContactParameters : public InteractionPhysics
{
	public :

		Real		 kn				// normal elastic constant.
				,ks				// shear elastic constant.
				,initialKn			// initial normal elastic constant.
				,initialKs			// initial shear elastic constant.
				,equilibriumDistance		// equilibrium distance
				,initialEquilibriumDistance	// initial equilibrium distance
				,frictionAngle 			// angle of friction, according to Coulumb criterion
				,tangensOfFrictionAngle;
	
		Vector3r	prevNormal			// unit normal of the contact plane.
				,normalForce			// normal force applied on a DE
				,shearForce;			// shear force applied on a DE
		
		virtual ~ElasticContactParameters();

	REGISTER_CLASS_NAME(ElasticContactParameters);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);

};

REGISTER_SERIALIZABLE(ElasticContactParameters,false);

#endif // ELASTIC_CONTACT_PARAMETERS_HPP

