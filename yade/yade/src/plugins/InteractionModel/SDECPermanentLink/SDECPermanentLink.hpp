#ifndef __SDECCPERMANENTLINK_H__
#define __SDECCPERMANENTLINK_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"

class SDECPermanentLink : public InteractionGeometry
{
	public : float radius1;
	public : float radius2;
	public : Vector3r normal;			// new unit normal of the contact plane.
	//public : float penetrationDepth;
	//public : Vector3r contactPoint;

	public : float kn;				// normal elastic constant.
	public : float ks;				// shear elastic constant.

	public : float knMax;
	public : float ksMax;

	public : float initialKn;			// initial normal elastic constant.
	public : float initialKs;			// initial shear elastic constant.
	public : float equilibriumDistance;		// equilibrium distance
	public : float initialEquilibriumDistance;	// initial equilibrium distance
	public : Vector3r prevNormal;			// unit normal of the contact plane.
	public : Vector3r normalForce;			// normal force applied on a DE
	public : Vector3r shearForce;			// shear force applied on a DE

/////////////////////////////////////////////////////// FIXME : this goes to another dynlib - MDEM
	//public : Vector3r initRotation1;
	//public : Vector3r initRotation2;

	public : Quaternionr prevRotation1;
	//public : Quaternionr currentRotation1;
	public : Quaternionr prevRotation2;
	//public : Quaternionr currentRotation2;
	
	public : Vector3r thetar;
	public : float heta;
	public : float averageRadius;
	public : float kr;
////////////////////////////////////////////////////////

	// construction
	public : SDECPermanentLink ();
	public : ~SDECPermanentLink ();

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(SDECPermanentLink);
};

REGISTER_SERIALIZABLE(SDECPermanentLink,false);

#endif // __SDECCPERMANENTLINK_H__
