#ifndef __BOUNDINGSPHERE_H__
#define __BOUNDINGSPHERE_H__

#include "BoundingVolume.hpp"

class BoundingSphere : public BoundingVolume
{
	public : float radius;
	public : Vector3 center;
	
	// construction
	public : BoundingSphere(float r=0);
	
	public : ~BoundingSphere ();
	
	public : void move(Se3& se3);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();	
	public : void update(Se3& se3);
	
	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(BoundingSphere);
};

//EXPORT_DYNLIB(BoundingSphere);
REGISTER_CLASS(BoundingSphere,false);

#endif // __BOUNDINGSPHERE_H__
