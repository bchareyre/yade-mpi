#ifndef __MESH2D_H__
#define __MESH2D_H__

#include "CollisionGeometry.hpp"

#define offset(i,j) ((i)*height+(j))

class Edge : public Serializable
{
	public : int first;
	public : int second;
	public : Edge() {};
	public : Edge(int f,int s) :first(f), second(s) {};
	public : void processAttributes() {};
	public : void registerAttributes()
	{
		REGISTER_ATTRIBUTE(first);
		REGISTER_ATTRIBUTE(second);
	};
	REGISTER_CLASS_NAME(Edge);

};
REGISTER_SERIALIZABLE(Edge,true);

class Mesh2D : public CollisionGeometry
{
	public : vector<Vector3> vertices;
	public : vector<Edge> edges;
	public : int width,height;
	public : vector<vector<int> > faces;
	public : vector<Vector3> fNormals;
	public : vector<Vector3> vNormals;
 	public : vector<vector<int> > triPerVertices;
	// construction
	public : Mesh2D ();
	public : ~Mesh2D ();

	public : bool collideWith(CollisionGeometry* collisionGeometry);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();
	public : void computeNormals();

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(Mesh2D);
	REGISTER_CLASS_INDEX(Mesh2D);

};

#include "ArchiveTypes.hpp"
using namespace ArchiveTypes;

REGISTER_SERIALIZABLE(Mesh2D,false);

#endif // __MESH2D_H__
