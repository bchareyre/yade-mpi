#ifndef __SDECCPERMANENTLINK_H__
#define __SDECCPERMANENTLINK_H__

#include <vector>

#include "InteractionGeometry.hpp"
#include "Vector3.hpp"
#include "Quaternion.hpp"

class SDECPermanentLink : public InteractionGeometry
{
	public : Real radius1;
	public : Real radius2;
	public : Vector3r normal;			// new unit normal of the contact plane.
	//public : Real penetrationDepth;
	//public : Vector3r contactPoint;

	public : Real kn;				// normal elastic constant.
	public : Real ks;				// shear elastic constant.

	public : Real knMax;
	public : Real ksMax;

	public : Real initialKn;			// initial normal elastic constant.
	public : Real initialKs;			// initial shear elastic constant.
	public : Real equilibriumDistance;		// equilibrium distance
	public : Real initialEquilibriumDistance;	// initial equilibrium distance
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
	public : Real heta;
	public : Real averageRadius;
	public : Real kr;
////////////////////////////////////////////////////////

	// construction
	public : SDECPermanentLink ();
	public : ~SDECPermanentLink ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	REGISTER_CLASS_NAME(SDECPermanentLink);
};

REGISTER_SERIALIZABLE(SDECPermanentLink,false);

#endif // __SDECCPERMANENTLINK_H__
