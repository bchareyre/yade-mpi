#ifndef __SDECCONTACTMODEL_H__
#define __SDECCONTACTMODEL_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"

class SDECContactGeometry : public InteractionGeometry
{
	public : virtual ~SDECContactGeometry();

	public : Real radius1;
	public : Real radius2;
	public : Vector3r normal;			// new unit normal of the contact plane.
	public : Real penetrationDepth;
	public : Vector3r contactPoint;	
	
// 	public : Real kn;				// normal elastic constant.
// 	public : Real ks;				// shear elastic constant.
// 	public : Real initialKn;			// initial normal elastic constant.
// 	public : Real initialKs;			// initial shear elastic constant.
// 	public : Real equilibriumDistance;		// equilibrium distance
// 	public : Real initialEquilibriumDistance;	// initial equilibrium distance
// 	public : Vector3r prevNormal;			// unit normal of the contact plane.
// 	public : Vector3r normalForce;			// normal force applied on a DE
// 	public : Vector3r shearForce;			// shear force applied on a DE

	REGISTER_CLASS_NAME(SDECContactGeometry);
};

REGISTER_SERIALIZABLE(SDECContactGeometry,false);

#endif // __SDECCONTACTMODEL_H__
