#ifndef __AABB_H__
#define __AABB_H__

#include "BoundingVolume.hpp"

class AABB : public BoundingVolume
{
	public : Vector3 halfSize;
	public : Vector3 center;

	private : Vector3 initialHalfSize;
	private : Vector3 initialCenter;


	// construction
	public : AABB ();
	public : AABB (Vector3 hs,Vector3 center=Vector3(0,0,0));
	public : ~AABB ();

	public : void move(Se3& se3);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();
	public : void update(Se3& se3);
	public : bool overlap(const AABB& aabb);

	public : void processAttributes();
	public : void registerAttributes();
	REGISTER_CLASS_NAME(AABB);
};

REGISTER_SERIALIZABLE(AABB,false);

#endif // __AABB_H__



