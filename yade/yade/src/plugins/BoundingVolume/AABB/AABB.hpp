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

	public : void move(Se3r& se3);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();
	public : void update(Se3r& se3);
	public : bool overlap(const AABB& aabb);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	REGISTER_CLASS_NAME(AABB);
};

REGISTER_SERIALIZABLE(AABB,false);

#endif // __AABB_H__



