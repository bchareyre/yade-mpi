#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "CollisionGeometry.hpp"

#include <vector>
#include <fstream>
#include "Vector2.hpp"
#include "AABB.hpp"

using namespace std;

class Terrain : public CollisionGeometry
{
	typedef std::vector<int> tFace;

	typedef struct
	{
		int v1;
		int v2;
		int num;
	} tEdge;

	struct lessThantEdge
	{
		bool operator()(const tEdge e1, const tEdge e2) const
		{
			return e1.v1<e2.v1 || (e1.v1==e2.v1 && e1.v2<e2.v2);
		}
	};


	public : std::vector<Vector3r> vertices;
	public : std::vector<tFace> faces;
	public : std::vector<Vector3r> normals;
	public : string fileName;

	public : Vector3r min;
	public : Vector3r max;
//  	public :  inline Vector3r& min() { return _min; };
//  	public :  inline Vector3r& max() { return _max; };

	private : float cellSizeX,cellSizeZ;
	private : int nbTriPerCell,nbCells;
	private : bool pointInTriangle(Vector2r p ,std::vector<Vector2r>& tri);

	protected : void buildCollisionGeometry();

	protected : std::vector<std::vector<std::pair<float,float> > > boundingBoxes;
	public : std::vector<std::vector<std::vector<int> > > triLists;
	public : std::vector<AABB> facesAABB;

	protected : float cellSize;

	public :  void	getFaces(const AABB& aabb, std::vector<int>& faceList);
	private : std::vector<int> testedFaces;

	protected : unsigned int _glListId;
	public : inline unsigned int glListId() { return _glListId;};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

	private : bool findTag(ifstream * file, char * tag);
	private : void loadWrl(const char * fileName);
	private : unsigned int buildDisplayList();
	private : void glDrawNormals();
	private : void reOrientFaces();

	// construction
	public : Terrain ();
	public : ~Terrain ();

	public : void processAttributes();
	public : void registerAttributes();

	public : bool loadFromFile(char * fileName);
	public : void glDraw();

	REGISTER_CLASS_NAME(Terrain);
	REGISTER_CLASS_INDEX(Terrain);
};

REGISTER_SERIALIZABLE(Terrain,false);

#endif // __TERRAIN_H__


