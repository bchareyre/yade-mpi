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
// #include "Polyhedron.hpp"
// #include "OpenGLWrapper.hpp"
// #include <fstream>
// #include <map>
// 
// using namespace std;
// 
// Polyhedron::Polyhedron () : BodyInteractionGeometry()
// {
// 	createIndex();
// }
// 
// Polyhedron::~Polyhedron ()
// {
// 
// }
// 
// void Polyhedron::postProcessAttributes(bool deserializing)
// {
// 	BodyInteractionGeometry::postProcessAttributes(deserializing);
// 
// 	if(deserializing)
// 	{
// 		loadGmshMesh(mshFileName);
// 	
// 		fNormals.resize(faces.size());
// 		cerr << "process"<<endl;
// 		/*vNormals.resize(vertices.size());
// 		triPerVertices.resize(vertices.size());
// 		for(unsigned int i=0;i<faces.size();i++)
// 		{
// 			triPerVertices[faces[i][0]].push_back(i);
// 			triPerVertices[faces[i][1]].push_back(i);
// 			triPerVertices[faces[i][2]].push_back(i);
// 		}*/
// 	}
// }
// 
// void Polyhedron::registerAttributes()
// {
// 	BodyInteractionGeometry::registerAttributes();
// 	REGISTER_ATTRIBUTE(mshFileName);
// }
// 
// 
// 
// void Polyhedron::glDraw()
// {
// 	glColor3v(diffuseColor);
// 
// 	//wire=true;
// 	if (wire)
// 	{
// 		glDisable(GL_LIGHTING);
// 
// 		glBegin(GL_LINES);
// 			for(unsigned int i=0;i<edges.size();i++)
// 			{
// 				glVertex3v(vertices[edges[i].first]);
// 				glVertex3v(vertices[edges[i].second]);
// 			}
// 		glEnd();
// 	}
// 	else
// 	{
// 		glShadeModel(GL_SMOOTH);
// 		GLfloat matSpecular[] = { 1.0,1.0,1.0,1.0};
// 		GLfloat matShininess[] = { 50.0};
// 		glMaterialfv(GL_FRONT,GL_SPECULAR,matSpecular);
// 		glMaterialfv(GL_FRONT,GL_SHININESS,matShininess);
// 
// 		glEnable(GL_LIGHTING);
// 		glDisable(GL_CULL_FACE);
// 		computeNormals();
// 		glBegin(GL_TRIANGLES);
// 			for(unsigned int i=0;i<faces.size();i++)
// 			{
// 				int v1 = faces[i][0];
// 				int v2 = faces[i][1];
// 				int v3 = faces[i][2];
// 				//glNormal3v(vNormals[v1]);
// 				glNormal3v(fNormals[i]);
// 				glVertex3v(vertices[v1]);
// 				//glNormal3v(vNormals[v2]);
// 				glVertex3v(vertices[v2]);
// 				//glNormal3v(vNormals[v3]);
// 				glVertex3v(vertices[v3]);
// 			}
// 		glEnd();
// 	}
// }
// 
// void Polyhedron::computeNormals()
// {
// 
// 	for(unsigned int i=0;i<faces.size();i++)
// 	{
// 		Vector3r v1 = vertices[faces[i][0]];
// 		Vector3r v2 = vertices[faces[i][1]];
// 		Vector3r v3 = vertices[faces[i][2]];
// 		fNormals[i] = (v2-v1).cross(v3-v1);
// 	}
// 	/*for(unsigned int i=0;i<vertices.size();i++)
// 	{
// 		int size = triPerVertices[i].size();
// 		vNormals[i] = fNormals[triPerVertices[i][0]];
// 		for(int j=1;j<size;j++)
// 			vNormals[i] += fNormals[triPerVertices[i][j]];
// 		vNormals[i] /= size;
// 	}*/
// }
// 
// 
// 
// 
// void Polyhedron::loadGmshMesh(const string& fileName)
// {
// 
// 	// all the vertices
// 	//std::set<NumberedVertice,lessThantNumberedVertice> vertices;
// 	int nbVertices;
// 
// 	map<int,int> vTranslationTable;
// 
// 	// all others elements : edges, faces, tetrahedrons
// 	std::vector<std::vector<int> > elements;
// 	std::vector<int> elementsType;
// 	int nbElements,type;
// 
// 	int n,phys,elem,nNodes;
// 	char buffer[100];
// 
// 	ifstream file(fileName.c_str());
// 
// 	// skipping $NOD
// 	file.getline(buffer,10);
// 
// 	// read number of vertices
// 	file >> nbVertices;
// 
// 	vertices.resize(nbVertices);
// 
// 	// read all vertices
// 	for(int i=0;i<nbVertices;i++)
// 	{
// 		file >> n >> vertices[i][0] >> vertices[i][1] >> vertices[i][2];
// 		vTranslationTable[n] = i;
// 	}
// 
// 	// skipping return after last number
// 	file.get();
// 	// skipping $ENDNOD
// 	file.getline(buffer,10);
// 	// skipping $ELM
// 	file.getline(buffer,10);
// 
// 	// read number of total elements
// 	file >> nbElements;
// 
// 	int nbTetras = 0;
// 	int nbFaces  = 0;
// 	int nbEdges  = 0;
// 
// 	// loading all edges, faces, tetrahedrons
// 	elements.resize(nbElements);
// 	elementsType.resize(nbElements);
// 	for(int i=0;i<nbElements;i++)
// 	{
// 		file >> n >> type >> phys >> elem >> nNodes;
// 		elementsType[i] = type;
// 		elements[i].resize(nNodes);
// 		for(int j=0;j<nNodes;j++)
// 			file >> elements[i][j];
// 
// 		switch (type)
// 		{
// 			case 4 : nbTetras++; break;
// 			case 2 : nbFaces++; break;
// 			case 1 : nbEdges++; break;
// 			default : break;
// 		}
// 	}
// 
// 	// skipping return after last number
// 	file.get();
// 	//skipping $ELM
// 	file.getline(buffer,10);
// 
// 	file.close();
// 
// 	tetrahedrons.resize(nbTetras);
// 	faces.resize(nbFaces);
// 	edges.resize(nbEdges);
// 	for(int i=0,e=0,f=0,t=0;i<nbElements;i++)
// 	{
// 		switch (elementsType[i])
// 		{
// 			case 4 :
// 				tetrahedrons[t].push_back(vTranslationTable[elements[i][0]]);
// 				tetrahedrons[t].push_back(vTranslationTable[elements[i][1]]);
// 				tetrahedrons[t].push_back(vTranslationTable[elements[i][2]]);
// 				tetrahedrons[t].push_back(vTranslationTable[elements[i][3]]);
// 				t++;
// 				break;
// 			case 2 :
// 				faces[f].push_back(vTranslationTable[elements[i][0]]);
// 				faces[f].push_back(vTranslationTable[elements[i][1]]);
// 				faces[f].push_back(vTranslationTable[elements[i][2]]);
// 				f++;
// 				break;
// 			case 1 :
// 				edges[e] = pair<int,int>(vTranslationTable[elements[i][0]],vTranslationTable[elements[i][1]]);
// 				e++;
// 				break;
// 			default : break;
// 		}
// 	}
// }
