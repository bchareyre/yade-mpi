#ifndef __AABB_H__
#define __AABB_H__

#include "BoundingVolume.hpp"

class AABB : public BoundingVolume
{
	public : Vector3r halfSize;
	public : Vector3r center;

	private : Vector3r initialHalfSize;
	private : Vector3r initialCenter;


	// construction
	public : AABB ();
	public : AABB (Vector3r hs,Vector3r center=Vector3r(0,0,0));
	public : ~AABB ();


	public : void glDraw();


///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(AABB);	
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_INDEX(AABB,BoundingVolume);
};

REGISTER_SERIALIZABLE(AABB,false);

#endif // __AABB_H__



