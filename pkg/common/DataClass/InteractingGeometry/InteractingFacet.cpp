/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "InteractingFacet.hpp"

CREATE_LOGGER(InteractingFacet);

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


void InteractingFacet::postProcessAttributes(bool deserializing)
{
    if (deserializing)
    {
		Vector3r e[3] = {vertices[1]-vertices[0] ,vertices[2]-vertices[1] ,vertices[0]-vertices[2]};
		#define CHECK_EDGE(i) if(e[i].SquaredLength()==0){LOG_FATAL("InteractingFacet has coincident vertices "<<i<<" ("<<vertices[i]<<") and "<<(i+1)%3<<" ("<<vertices[(i+1)%3]<<")!");}
			CHECK_EDGE(0); CHECK_EDGE(1);CHECK_EDGE(2);
		#undef CHECK_EDGE
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
YADE_PLUGIN((InteractingFacet));
