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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "PolyhedralSweptSphere.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

PolyhedralSweptSphere::PolyhedralSweptSphere() : InteractingGeometry()
{
	createIndex();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

PolyhedralSweptSphere::~PolyhedralSweptSphere()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PolyhedralSweptSphere::registerAttributes()
{
	InteractingGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(vertices);
	REGISTER_ATTRIBUTE(faces);
	REGISTER_ATTRIBUTE(radius);

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void PolyhedralSweptSphere::postProcessAttributes(bool deserializing)
{
	if (deserializing)
		computeEdges();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
