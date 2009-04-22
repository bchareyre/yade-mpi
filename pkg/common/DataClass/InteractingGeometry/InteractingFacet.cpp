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
	#ifdef FACET_TOPO
		edgeAdjIds.resize(3,Body::ID_NONE);	
		edgeAdjHalfAngle.resize(3,0);
	#endif
}

InteractingFacet::~InteractingFacet()
{
}

void InteractingFacet::registerAttributes()
{
    InteractingGeometry::registerAttributes();
    REGISTER_ATTRIBUTE(vertices);
	#ifdef FACET_TOPO
		REGISTER_ATTRIBUTE(edgeAdjIds);
		REGISTER_ATTRIBUTE(edgeAdjHalfAngle);
	#endif
}

void InteractingFacet::postProcessAttributes(bool deserializing)
{
    if (deserializing)
    {
		Vector3r e[3] = {vertices[1]-vertices[0] ,vertices[2]-vertices[1] ,vertices[0]-vertices[2]};
		nf = e[0].UnitCross(e[1]);
		for(int i=0; i<3; ++i) 
		{
			ne[i]=e[i].UnitCross(nf);
			vl[i]=vertices[i].Length();
			vu[i]=vertices[i]/vl[i];
		}
		Real p = e[0].Length()+e[1].Length()+e[2].Length();
		icr = e[0].Length()*ne[0].Dot(e[2])/p;
	}
}
YADE_PLUGIN();
