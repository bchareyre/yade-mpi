#ifndef __MESH2D_H__
#define __MESH2D_H__

#include "CollisionModel.hpp"

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
REGISTER_CLASS(Edge,true);

class Mesh2D : public CollisionModel
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

	public : bool collideWith(CollisionModel* collisionModel);
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

REGISTER_CLASS(Mesh2D,false);
REGISTER_CLASS_TO_MULTI_METHODS_MANAGER(Mesh2D);

#endif // __MESH2D_H__
