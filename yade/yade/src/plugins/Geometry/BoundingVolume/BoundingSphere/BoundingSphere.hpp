#ifndef __BOUNDINGSPHERE_H__
#define __BOUNDINGSPHERE_H__

#include "BodyBoundingVolume.hpp"

class BoundingSphere : public BodyBoundingVolume
{
	public : Real radius;
	public : Vector3r center;

	// construction
	public : BoundingSphere(Real r=0);

	public : ~BoundingSphere ();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(BoundingSphere);	
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_INDEX(BoundingSphere,BodyBoundingVolume);
};

REGISTER_SERIALIZABLE(BoundingSphere,false);

#endif // __BOUNDINGSPHERE_H__
