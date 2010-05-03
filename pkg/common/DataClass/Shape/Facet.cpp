/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#include "Facet.hpp"

CREATE_LOGGER(Facet);

Facet::~Facet()
{
}


void Facet::postProcessAttributes(bool deserializing)
{
    if (deserializing)
    {
		Vector3r e[3] = {vertices[1]-vertices[0] ,vertices[2]-vertices[1] ,vertices[0]-vertices[2]};
		#define CHECK_EDGE(i) if(e[i].squaredNorm()==0){LOG_FATAL("Facet has coincident vertices "<<i<<" ("<<vertices[i]<<") and "<<(i+1)%3<<" ("<<vertices[(i+1)%3]<<")!");}
			CHECK_EDGE(0); CHECK_EDGE(1);CHECK_EDGE(2);
		#undef CHECK_EDGE
		nf = e[0].cross(e[1]); nf.normalize();
		for(int i=0; i<3; ++i) 
		{
			ne[i]=e[i].cross(nf); ne[i].normalize();
			vl[i]=vertices[i].norm();
			vu[i]=vertices[i]/vl[i];
		}
		Real p = e[0].norm()+e[1].norm()+e[2].norm();
		icr = e[0].norm()*ne[0].dot(e[2])/p;
	}
}
YADE_PLUGIN((Facet));
