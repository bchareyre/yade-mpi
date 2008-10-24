// © 2004 Olivier Galizzi <olivier.galizzi@imag.fr>
#pragma once

#include<yade/pkg-common/NormalShearInteractions.hpp>

class ElasticContactInteraction: public NormalShearInteraction
{
	public :
		// kn,ks,normal inherited from NormalShearInteraction
		Real initialKn			// initial normal elastic constant.
				,initialKs			// initial shear elastic constant.
				,equilibriumDistance		// equilibrium distance
				,initialEquilibriumDistance	// initial equilibrium distance
				,frictionAngle 			// angle of friction, according to Coulumb criterion
				,tangensOfFrictionAngle
				,unMax		// the maximum value of penetration depth of the history of this interaction
				,previousun	// the value of this un at the last time step
				,previousFn;	// the value of the normal force at the last time step
	
		Vector3r	prevNormal;			// unit normal of the contact plane.

		ElasticContactInteraction();
		virtual ~ElasticContactInteraction();
	protected :
		virtual void registerAttributes();
// {
// 			NormalShearInteraction::registerAttributes();
// 			REGISTER_ATTRIBUTE(prevNormal);
// 			REGISTER_ATTRIBUTE(initialKn);
// 			REGISTER_ATTRIBUTE(initialKs);
// 			REGISTER_ATTRIBUTE(tangensOfFrictionAngle);
// 		}
	REGISTER_CLASS_NAME(ElasticContactInteraction);
	REGISTER_BASE_CLASS_NAME(NormalShearInteraction);
	REGISTER_CLASS_INDEX(ElasticContactInteraction,NormalShearInteraction);
};
REGISTER_SERIALIZABLE(ElasticContactInteraction,false);

