#ifndef __SDECDISCRETEELEMENT_H__
#define __SDECDISCRETEELEMENT_H__

#include "RigidBody.hpp"

class SDECDiscreteElement : public RigidBody
{
	public : float kn;
	public : float ks;

	// construction
	public : SDECDiscreteElement ();
	public : ~SDECDiscreteElement ();

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	//public : void moveToNextTimeStep();

	REGISTER_CLASS_NAME(SDECDiscreteElement);
	//REGISTER_CLASS_INDEX(SDECDiscreteElement);
};

REGISTER_SERIALIZABLE(SDECDiscreteElement,false);

#endif // __SDECDISCRETEELEMENT_H__

