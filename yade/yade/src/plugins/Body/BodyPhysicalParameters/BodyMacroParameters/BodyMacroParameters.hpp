#ifndef __SDECDISCRETEELEMENT_H__
#define __SDECDISCRETEELEMENT_H__

#include "RigidBodyParameters.hpp"

class BodyMacroParameters : public RigidBodyParameters
{
	public : Real young;
	public : Real poisson;
	public : Real frictionAngle;

	public : BodyMacroParameters ();
	public : virtual ~BodyMacroParameters ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(BodyMacroParameters);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////

	REGISTER_CLASS_INDEX(BodyMacroParameters,RigidBodyParameters);
};

REGISTER_SERIALIZABLE(BodyMacroParameters,false);

#endif // __SDECDISCRETEELEMENT_H__

