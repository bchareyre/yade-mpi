/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "InteractingFacet.hpp"

InteractingFacet::InteractingFacet() : InteractingGeometry()
{
    createIndex();
}

InteractingFacet::~InteractingFacet()
{
}

void InteractingFacet::registerAttributes()
{
    InteractingGeometry::registerAttributes();
    REGISTER_ATTRIBUTE(vertices);
}

void InteractingFacet::postProcessAttributes(bool deserializing)
{
    if (deserializing)
    {
	normal = vertices[1].UnitCross(vertices[0]);
	for (int i=0, e=vertices.size()+1; i<e; ++i)
	{
	    Vector3r edge,p0;
	    if (i==0)
	    {
		edge = vertices[0];
		p0 = vertices[e-2];
		cm = vertices[0];
	    }
	    else if (i==e-1)
	    {
		edge = vertices[e-2];
		p0 = vertices[0];
		cm /= e;
	    }
	    else
	    {
		edge = vertices[i] - vertices[i-1];
		p0 = -vertices[i];
		cm += vertices[i];
	    }
		
	    Vector3r nml = p0 - edge * edge.Dot(p0) / edge.SquaredLength();
	    nml.Normalize();
	    edgeNormals.push_back(nml);
	}
    }
}
YADE_PLUGIN();
