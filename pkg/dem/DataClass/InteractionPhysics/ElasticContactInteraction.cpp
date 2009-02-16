#include "ElasticContactInteraction.hpp"

ElasticContactInteraction::ElasticContactInteraction()
{
	createIndex();
	
}

ElasticContactInteraction::~ElasticContactInteraction()
{
}
 
// void ElasticContactInteraction::postProcessAttributes(bool)
// {
// 
// }


void ElasticContactInteraction::registerAttributes()
{
	NormalShearInteraction::registerAttributes();
	REGISTER_ATTRIBUTE(prevNormal);
	//REGISTER_ATTRIBUTE(initialKn);
	//REGISTER_ATTRIBUTE(initialKs);
	REGISTER_ATTRIBUTE(tangensOfFrictionAngle);


//		Real		 kn				// normal elastic constant.
//				,ks				// shear elastic constant.
//				,initialKn			// initial normal elastic constant.
//				,initialKs			// initial shear elastic constant.
//				,equilibriumDistance		// equilibrium distance
//				,initialEquilibriumDistance	// initial equilibrium distance
//				,frictionAngle 			// angle of friction, according to Coulumb criterion
//				,tangensOfFrictionAngle;
//	
//		Vector3r	prevNormal			// unit normal of the contact plane.
//				,normalForce			// normal force applied on a DE
//				,shearForce;			// shear force applied on a DE
}
YADE_PLUGIN();

//YADE_PLUGIN("ElasticContactInteraction");
