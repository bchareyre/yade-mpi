#ifndef __BOUNDINGSPHERE_H__
#define __BOUNDINGSPHERE_H__

#include "BoundingVolume.hpp"

class BoundingSphere : public BoundingVolume
{
	public : float radius;
	public : Vector3r center;

	// construction
	public : BoundingSphere(float r=0);

	public : ~BoundingSphere ();

	public : void move(Se3r& se3);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();
	public : void update(Se3r& se3);

	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

	REGISTER_CLASS_NAME(BoundingSphere);	
	REGISTER_CLASS_INDEX(BoundingSphere);
};

REGISTER_SERIALIZABLE(BoundingSphere,false);

#endif // __BOUNDINGSPHERE_H__
