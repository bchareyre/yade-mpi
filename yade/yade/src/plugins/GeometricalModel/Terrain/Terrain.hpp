#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include "CollisionModel.hpp"

#include <vector>
#include <fstream>
#include "Vector2.hpp"
#include "AABB.hpp"

using namespace std;

class Terrain : public CollisionModel
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


	public : std::vector<Vector3> vertices;	
	public : std::vector<tFace> faces;	
	public : std::vector<Vector3> normals;	
	public : string fileName;
	
	public : Vector3 min;
	public : Vector3 max;
//  	public :  inline Vector3& min() { return _min; };
//  	public :  inline Vector3& max() { return _max; };
	
	private : float cellSizeX,cellSizeZ;
	private : int nbTriPerCell,nbCells;
	private : bool pointInTriangle(Vector2 p ,std::vector<Vector2>& tri);
	
	protected : void buildCollisionModel();
	
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

REGISTER_CLASS(Terrain,false);
REGISTER_CLASS_TO_MULTI_METHODS_MANAGER(Terrain);

#endif // __TERRAIN_H__


