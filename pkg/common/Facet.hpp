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
// #define FACET_TOPO

class Facet : public Shape {
    public:
	
	virtual ~Facet();
	
	// Postprocessed attributes 

	/// Facet's normal
	//Vector3r nf; 
	/// Normals of edges 
	Vector3r ne[3];
	/// Inscribing cirle radius
	Real icr;
	/// Length of the vertice vectors 
	Real vl[3];
	/// Unit vertice vectors
	Vector3r vu[3];

	void postLoad(Facet&);

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Facet,Shape,"Facet (triangular particle) geometry.",
		((vector<Vector3r>,vertices,vector<Vector3r>(3,Vector3r(NaN,NaN,NaN)),(Attr::triggerPostLoad | Attr::noResize),"Vertex positions in local coordinates."))
		((Vector3r,normal,Vector3r(NaN,NaN,NaN),(Attr::readonly | Attr::noSave),"Facet's normal"))
		((Real,area,NaN,(Attr::readonly | Attr::noSave),"Facet's area"))
		#ifdef FACET_TOPO
		((vector<Body::id_t>,edgeAdjIds,vector<Body::id_t>(3,Body::ID_NONE),,"Facet id's that are adjacent to respective edges [experimental]"))
		((vector<Real>,edgeAdjHalfAngle,vector<Real>(3,0),,"half angle between normals of this facet and the adjacent facet [experimental]"))
		#endif
		,
		/* ctor */ createIndex();,
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(Facet,Shape);
};
REGISTER_SERIALIZABLE(Facet);

