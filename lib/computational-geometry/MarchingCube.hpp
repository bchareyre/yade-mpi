/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <cmath>
#include <string>
#include <vector>
#include <lib/base/Math.hpp>

class MarchingCube
{

/// ATTRIBUTES

	private : vector<Vector3r> triangles;
	public  : const vector<Vector3r>& getTriangles() { return triangles; }
	
	private : vector<Vector3r> normals;
	public  : const vector<Vector3r>& getNormals() { return normals; }

	private : int nbTriangles;
	public  : int getNbTriangles() { return nbTriangles; }

	private : int sizeX,sizeY,sizeZ;
	private : Real isoValue;

	private : vector<vector<vector<Vector3r> > > positions;
	private : static const int edgeArray[256];
	private : static const int triTable[256][16];
	Vector3r aNormal;
	
/// PRIVATE METHOD

	/** triangulate cell (x,y,z) **/
	private : void polygonize (const vector<vector<vector<Real> > >& scalarField, int x, int y, int z);

	/** compute normals of the triangles previously found with polygonizecalcule les normales des triangles trouver dans la case (x,y,z)
		@param n : indice of the first triangle to process
	**/
	private : void computeNormal(const vector<vector<vector<Real> > >& scalarField, int x, int y, int z,int offset, int triangleNum);
	
	/** interpolate coordinates of point vect (that is on isosurface) from coordinates of points vect1 et vect2 **/
	private : void interpolate (const Vector3r& vect1, const Vector3r& vect2, Real val1, Real val2,Vector3r& vect);

	/** Same as interpolate but in 1D **/
	private : Real interpolateValue(Real val1, Real val2, Real val_cible1, Real val_cible2);

	/** Compute normal to vertice or triangle inside cell (x,y,z) **/
	private : const Vector3r& computeNormalX(const vector<vector<vector<Real> > >& scalarField, int x, int y, int z);
	private : const Vector3r& computeNormalY(const vector<vector<vector<Real> > >& scalarField, int x, int y, int z);
	private : const Vector3r& computeNormalZ(const vector<vector<vector<Real> > >& scalarField, int x, int y, int z);

/// CONSTRUCTOR/DESTRUCTOR

	public  : MarchingCube ();
	public  : ~MarchingCube ();

/// PULIC METHODS

	public  : void computeTriangulation(const vector<vector<vector<Real> > >& scalarField, Real iso);

	public  : void init(int sx, int sy, int sz, const Vector3r& min, const Vector3r& max);

	public  : void resizeScalarField(vector<vector<vector<Real> > >& scalarField, int sx, int sy, int sz);
};
