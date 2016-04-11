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


void Facet::postLoad(Facet&)
{
	// if this fails, it means someone did vertices push_back, but they are resized to 3 at Facet initialization already
	// in the future, a fixed-size array should be used instead of vector<Vector3r> for vertices
	// this is prevented by yade::serialization now IIRC
	if(vertices.size()!=3){ throw runtime_error(("Facet must have exactly 3 vertices (not "+boost::lexical_cast<string>(vertices.size())+")").c_str()); }
	if(std::isnan(vertices[0][0])) return;  // not initialized, nothing to do
	Vector3r e[3] = {vertices[1]-vertices[0] ,vertices[2]-vertices[1] ,vertices[0]-vertices[2]};
	#define CHECK_EDGE(i) if(e[i].squaredNorm()==0){LOG_FATAL("Facet has coincident vertices "<<i<<" ("<<vertices[i]<<") and "<<(i+1)%3<<" ("<<vertices[(i+1)%3]<<")!");}
		CHECK_EDGE(0); CHECK_EDGE(1);CHECK_EDGE(2);
	#undef CHECK_EDGE
	normal = e[0].cross(e[1]);
	area = .5*normal.norm();
	normal /= 2*area;
	for(int i=0; i<3; ++i){
		ne[i]=e[i].cross(normal); ne[i].normalize();
		vl[i]=vertices[i].norm();
		vu[i]=vertices[i]/vl[i];
	}
	Real p = e[0].norm()+e[1].norm()+e[2].norm();
	icr = e[0].norm()*ne[0].dot(e[2])/p;
}

YADE_PLUGIN((Facet));
