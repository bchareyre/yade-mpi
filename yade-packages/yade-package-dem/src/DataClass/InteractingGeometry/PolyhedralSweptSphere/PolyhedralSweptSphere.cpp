/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PolyhedralSweptSphere.hpp"

PolyhedralSweptSphere::PolyhedralSweptSphere() : InteractingGeometry()
{
	createIndex();
}


PolyhedralSweptSphere::~PolyhedralSweptSphere()
{

}


void PolyhedralSweptSphere::registerAttributes()
{
	InteractingGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(vertices);
	REGISTER_ATTRIBUTE(faces);
	REGISTER_ATTRIBUTE(radius);

}


void PolyhedralSweptSphere::postProcessAttributes(bool deserializing)
{
	if (deserializing)
		computeEdges();
}


void PolyhedralSweptSphere::computeEdges()
{
	set<pair<int,int>, ltPair > tmpEdges;
	for(unsigned int i=0 ; i<faces.size() ; i++)
	{
		for(unsigned int j=0 ; j<faces[i].size() ; j++)
		{
			int j2 = (j+1)%faces[i].size();
			if (faces[i][j]<faces[i][j2])
				tmpEdges.insert(pair<int,int>(faces[i][j],faces[i][j2]));
			else
				tmpEdges.insert(pair<int,int>(faces[i][j2],faces[i][j]));
		}
	}
	edges.clear();
	set<pair<int,int> >::iterator ei = tmpEdges.begin();
	set<pair<int,int> >::iterator eiEnd = tmpEdges.end();
	minEdge = Mathr::MAX_REAL;
	Real tmpMinEdge;
	for( ; ei!=eiEnd ; ++ei)
	{
		edges.push_back((*ei));
		tmpMinEdge = (vertices[(*ei).first]-vertices[(*ei).second]).length();
		if (tmpMinEdge<minEdge)
			minEdge = tmpMinEdge;
	}
}

