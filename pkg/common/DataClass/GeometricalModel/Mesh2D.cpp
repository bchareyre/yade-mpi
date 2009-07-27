/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "Mesh2D.hpp"
#include <iostream>


Mesh2D::Mesh2D () : GeometricalModel()
{		
	createIndex();
}


Mesh2D::~Mesh2D ()
{

}


void Mesh2D::postProcessAttributes(bool deserializing)
{
	GeometricalModel::postProcessAttributes(deserializing);
	
	if(deserializing)
	{
	
		fNormals.resize(faces.size());
		vNormals.resize(vertices.size());
		triPerVertices.resize(vertices.size());
		for(unsigned int i=0;i<faces.size();i++)
		{
			triPerVertices[faces[i][0]].push_back(i);
			triPerVertices[faces[i][1]].push_back(i);
			triPerVertices[faces[i][2]].push_back(i);
		}
	}
}







YADE_PLUGIN("Mesh2D");