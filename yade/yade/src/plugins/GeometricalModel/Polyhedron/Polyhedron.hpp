#ifndef __MESH3D_H__
#define __MESH3D_H__

#include "CollisionGeometry.hpp"

class Polyhedron : public CollisionGeometry
{
	public : string mshFileName;

	public : vector<Vector3> vertices;
	public : vector<pair<int,int> > edges;
	public : vector<vector<int> > tetrahedrons;

	public : vector<vector<int> > faces;
	public : vector<Vector3> fNormals;
	public : vector<Vector3> vNormals;
 	public : vector<vector<int> > triPerVertices;
	// construction
	public : Polyhedron ();
	public : ~Polyhedron ();

	public : bool collideWith(CollisionGeometry* collisionModel);
	public : bool loadFromFile(char * fileName);
	public : void glDraw();
	public : void computeNormals();
	public : void loadGmshMesh(const string& fileName);

	public : void processAttributes();
	public : void registerAttributes();

	REGISTER_CLASS_NAME(Polyhedron);
	REGISTER_CLASS_INDEX(Polyhedron);

};

#include "ArchiveTypes.hpp"
using namespace ArchiveTypes;

REGISTER_SERIALIZABLE(Polyhedron,false);

#endif // __MESH3D_H__
