#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "CollisionGeometry.hpp"

class Sphere : public CollisionGeometry
{
	public : float radius;

		int number;

	private : static vector<Vector3r> vertices;
	private : static vector<Vector3r> faces;
	private : static int glWiredSphereList;
	private : static int glSphereList;
	private : void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
	private : void drawSphere(int depth);

	// construction
	public : Sphere (float r);
	public : Sphere ();
	public : ~Sphere ();

	public : bool collideWith(CollisionGeometry* collisionGeometry);
	public : void glDraw();

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(Sphere);
	REGISTER_CLASS_INDEX(Sphere);
};

REGISTER_SERIALIZABLE(Sphere,false);

#endif // __SPHERE_H__
