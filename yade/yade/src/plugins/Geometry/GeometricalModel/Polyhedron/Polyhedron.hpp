// /***************************************************************************
//  *   Copyright (C) 2004 by Olivier Galizzi                                 *
//  *   olivier.galizzi@imag.fr                                               *
//  *                                                                         *
//  *   This program is free software; you can redistribute it and/or modify  *
//  *   it under the terms of the GNU General Public License as published by  *
//  *   the Free Software Foundation; either version 2 of the License, or     *
//  *   (at your option) any later version.                                   *
//  *                                                                         *
//  *   This program is distributed in the hope that it will be useful,       *
//  *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
//  *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
//  *   GNU General Public License for more details.                          *
//  *                                                                         *
//  *   You should have received a copy of the GNU General Public License     *
//  *   along with this program; if not, write to the                         *
//  *   Free Software Foundation, Inc.,                                       *
//  *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
//  ***************************************************************************/
// 
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// ////////////////////////////////////////////////////////////////////////////////////////////////////
// 
// #ifndef __MESH3D_H__
// #define __MESH3D_H__
// 
// #include "InteractionGeometry.hpp"
// 
// class Polyhedron : public InteractionGeometry
// {
// 	public : string mshFileName;
// 
// 	public : vector<Vector3r> vertices;
// 	public : vector<pair<int,int> > edges;
// 	public : vector<vector<int> > tetrahedrons;
// 
// 	public : vector<vector<int> > faces;
// 	public : vector<Vector3r> fNormals;
// 	public : vector<Vector3r> vNormals;
//  	public : vector<vector<int> > triPerVertices;
// 	// construction
// 	public : Polyhedron ();
// 	public : ~Polyhedron ();
// 	
// 	public : void glDraw();
// 	public : void computeNormals();
// 	public : void loadGmshMesh(const string& fileName);
// 
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// /// Serialization										///
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// 
// 	REGISTER_CLASS_NAME(Polyhedron);
// 	protected : virtual void postProcessAttributes(bool deserializing);
// 	public : void registerAttributes();
// 
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// /// Indexable											///
// ///////////////////////////////////////////////////////////////////////////////////////////////////
// 	
// 	REGISTER_CLASS_INDEX(Polyhedron,InteractionGeometry);
// 
// };
// 
// #include "ArchiveTypes.hpp"
// using namespace ArchiveTypes;
// 
// REGISTER_SERIALIZABLE(Polyhedron,false);
// 
// #endif // __MESH3D_H__
