#ifndef __SDECDISCRETEELEMENT_H__
#define __SDECDISCRETEELEMENT_H__

#include "RigidBody.hpp"

class SDECDiscreteElement : public RigidBody
{
	public : Real kn;
	public : Real ks;

	public : SDECDiscreteElement ();
	public : virtual ~SDECDiscreteElement ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(SDECDiscreteElement);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_INDEX(SDECDiscreteElement,RigidBody);
};

REGISTER_SERIALIZABLE(SDECDiscreteElement,false);

#endif // __SDECDISCRETEELEMENT_H__

