#ifndef __SDECCPERMANENTLINK_H__
#define __SDECCPERMANENTLINK_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"

class SDECPermanentLink : public InteractionGeometry
{
	public : float radius1;
	public : float radius2;
	public : Vector3 normal;			// new unit normal of the contact plane.
	//public : float penetrationDepth;
	//public : Vector3 contactPoint;

	public : float kn;				// normal elastic constant.
	public : float ks;				// shear elastic constant.

	public : float knMax;
	public : float ksMax;

	public : float initialKn;			// initial normal elastic constant.
	public : float initialKs;			// initial shear elastic constant.
	public : float equilibriumDistance;		// equilibrium distance
	public : float initialEquilibriumDistance;	// initial equilibrium distance
	public : Vector3 prevNormal;			// unit normal of the contact plane.
	public : Vector3 normalForce;			// normal force applied on a DE
	public : Vector3 shearForce;			// shear force applied on a DE

/////////////////////////////////////////////////////// FIXME : this goes to another dynlib - MDEM
	public : Vector3 initRotation1;
	public : Vector3 prevRotation1;
	public : Vector3 currentRotation1;
	public : Vector3 initRotation2;
	public : Vector3 prevRotation2;
	public : Vector3 currentRotation2;
	public : Vector3 thetar;
	public : float heta;
	public : float averageRadius;
	// kr ???????????
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
