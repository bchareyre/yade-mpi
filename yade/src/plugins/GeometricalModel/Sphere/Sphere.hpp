#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "CollisionModel.hpp"

class Sphere : public CollisionModel
{
	public : float radius;

	private : static vector<Vector3> vertices;
	private : static vector<Vector3> faces;
	private : static int glWiredSphereList;
	private : static int glSphereList;
	private : void subdivideTriangle(Vector3& v1,Vector3& v2,Vector3& v3, int depth);
	private : void drawSphere(int depth);
	
	// construction
	public : Sphere (float r);
	public : Sphere ();
	public : ~Sphere ();
	
	public : bool collideWith(CollisionModel* collisionModel);
	public : void glDraw();	
	
	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(Sphere);
};

REGISTER_CLASS(Sphere,false);

#endif // __SPHERE_H__
