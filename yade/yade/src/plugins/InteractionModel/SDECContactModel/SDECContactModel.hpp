#ifndef __SDECCONTACTMODEL_H__
#define __SDECCONTACTMODEL_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"

class SDECContactModel : public InteractionGeometry
{
	public : float radius1;
	public : float radius2;
	public : Vector3 normal;			// new unit normal of the contact plane.
	public : float penetrationDepth;
	public : Vector3 contactPoint;

	public : float kn;				// normal elastic constant.
	public : float ks;				// shear elastic constant.
	public : float initialKn;			// initial normal elastic constant.
	public : float initialKs;			// initial shear elastic constant.
	public : float equilibriumDistance;		// equilibrium distance
	public : float initialEquilibriumDistance;	// initial equilibrium distance
	public : Vector3 prevNormal;			// unit normal of the contact plane.
	public : Vector3 normalForce;			// normal force applied on a DE
	public : Vector3 shearForce;			// shear force applied on a DE

	// construction
	public : SDECContactModel ();
	public : ~SDECContactModel ();

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(SDECContactModel);
	//REGISTER_CLASS_INDEX(SDECContactModel);
};

REGISTER_SERIALIZABLE(SDECContactModel,false);

#endif // __SDECCONTACTMODEL_H__
