#ifndef __SDECDISCRETEELEMENT_H__
#define __SDECDISCRETEELEMENT_H__

#include "RigidBodyParameters.hpp"

class SDECParameters : public RigidBodyParameters
{
	public : Real kn;
	public : Real ks;

	public : SDECParameters ();
	public : virtual ~SDECParameters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(SDECParameters);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_INDEX(SDECParameters,RigidBodyParameters);
};

REGISTER_SERIALIZABLE(SDECParameters,false);

#endif // __SDECDISCRETEELEMENT_H__

