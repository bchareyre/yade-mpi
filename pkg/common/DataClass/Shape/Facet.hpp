/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once


#include<yade/core/Shape.hpp>
#include<yade/core/Body.hpp>

// define this to have topology information about facets enabled;
// it is necessary for FacetTopologyAnalyzer
#define FACET_TOPO

class Facet : public Shape {
    public:
	
	virtual ~Facet();
	
	// Postprocessed attributes 

	/// Facet's normal
	Vector3r nf; 
	/// Normals of edges 
	Vector3r ne[3];
	/// Inscribing cirle radius
	Real icr;
	/// Length of the vertice vectors 
	Real vl[3];
	/// Unit vertice vectors
	Vector3r vu[3];

	void postProcessAttributes(bool deserializing);

	void setVertices(const vector<Vector3r>& v){
		if(v.size()!=3) throw runtime_error("Facet must have exactly 3 vertices.");
		assert(vertices.size()==3);
		for(int i=0; i<3; i++) vertices[i]=v[i];
		Facet::postProcessAttributes(true);
	}
	vector<Vector3r> getVertices(){ vector<Vector3r> ret(3); for(int i=0; i<3; i++) ret[i]=vertices[i]; return ret;}

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Facet,Shape,"Facet (triangular particle) geometry.",
		((vector<Vector3r>,vertices,vector<Vector3r>(3),"[overridden below]"))
		#ifdef FACET_TOPO
		((vector<body_id_t>,edgeAdjIds,vector<body_id_t>(3,Body::ID_NONE),"Facet id's that are adjacent to respective edges [experimental]"))
		((vector<Real>,edgeAdjHalfAngle,vector<Real>(3,0),"half angle between normals of this facet and the adjacent facet [experimental]"))
		#endif
		,
		/* ctor */ createIndex();,
		/* must be separate to call postProcessAttributes(0) when changed, to keep internal data consistent */
		.add_property("vertices",&Facet::getVertices,&Facet::setVertices,"Vertex positions in local coordinates.")
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(Facet,Shape);
};
REGISTER_SERIALIZABLE(Facet);

