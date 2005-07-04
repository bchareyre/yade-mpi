/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/GeometricalModel.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <vector>
#include <fstream>
#include <yade-lib-wm3-math/Vector2.hpp>
#include <yade-common/AABB.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class Terrain : public GeometricalModel
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

	private : Real cellSizeX,cellSizeZ;
	private : int nbTriPerCell,nbCells;
	private : bool pointInTriangle(Vector2r p ,std::vector<Vector2r>& tri);

	protected : void buildInteractionGeometry();

	protected : std::vector<std::vector<std::pair<Real,Real> > > boundingBoxes;
	public : std::vector<std::vector<std::vector<int> > > triLists;
	public : std::vector<AABB> facesAABB;

	protected : Real cellSize;

	public :  void	getFaces(const AABB& aabb, std::vector<int>& faceList);
	private : std::vector<int> testedFaces;

	protected : unsigned int _glListId;
	public : inline unsigned int glListId() { return _glListId;};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

	private : bool findTag(ifstream * file, char * tag);
	private : void loadWrl(const char * fileName);
	private : void buildDisplayList();
	private : void glDrawNormals();
	private : void reOrientFaces();

	// construction
	public : Terrain ();
	public : virtual ~Terrain ();

	public : void glDraw();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(Terrain);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();
	
///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_INDEX(Terrain,GeometricalModel);
	
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(Terrain,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __TERRAIN_H__

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////


