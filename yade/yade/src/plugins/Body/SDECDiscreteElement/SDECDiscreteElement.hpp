#ifndef __SDECDISCRETEELEMENT_H__
#define __SDECDISCRETEELEMENT_H__

#include "RigidBody.hpp"

class SDECDiscreteElement : public RigidBody
{
	public : Real kn;
	public : Real ks;

	// construction
	public : SDECDiscreteElement ();
	public : ~SDECDiscreteElement ();

	//public : void moveToNextTimeStep();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(SDECDiscreteElement);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_INDEX(SDECDiscreteElement);
};

REGISTER_SERIALIZABLE(SDECDiscreteElement,false);

#endif // __SDECDISCRETEELEMENT_H__

