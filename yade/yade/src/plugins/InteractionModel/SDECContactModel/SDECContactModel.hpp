#ifndef __SDECCONTACTMODEL_H__
#define __SDECCONTACTMODEL_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"

class SDECContactModel : public InteractionGeometry
{
	public : float radius1;
	public : float radius2;
	public : Vector3r normal;			// new unit normal of the contact plane.
	public : float penetrationDepth;
	public : Vector3r contactPoint;

	public : float kn;				// normal elastic constant.
	public : float ks;				// shear elastic constant.
	public : float initialKn;			// initial normal elastic constant.
	public : float initialKs;			// initial shear elastic constant.
	public : float equilibriumDistance;		// equilibrium distance
	public : float initialEquilibriumDistance;	// initial equilibrium distance
	public : Vector3r prevNormal;			// unit normal of the contact plane.
	public : Vector3r normalForce;			// normal force applied on a DE
	public : Vector3r shearForce;			// shear force applied on a DE

	// construction
	public : SDECContactModel ();
	public : ~SDECContactModel ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	REGISTER_CLASS_NAME(SDECContactModel);
};

REGISTER_SERIALIZABLE(SDECContactModel,false);

#endif // __SDECCONTACTMODEL_H__
