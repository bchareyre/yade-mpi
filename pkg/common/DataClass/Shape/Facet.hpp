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
	
	// Registered attributes
		
	/// Vertex positions in the local reference frame
	vector<Vector3r> vertices;

	Facet();
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
	#ifdef FACET_TOPO
		//! facet id's that are adjacent to respective edges
		vector<body_id_t> edgeAdjIds;
		//! half angle between normals of this facet and the adjacent facet
		vector<Real> edgeAdjHalfAngle;
	#endif

	void postProcessAttributes(bool deserializing);

	YADE_CLASS_BASE_DOC_ATTRS(Facet,Shape,"Facet (triangular particle) geometry.",
		((vertices,"Vertex positions in local coordinates."))
		#ifdef FACET_TOPO
		((edgeAdjIds,"Facet id's that are adjacent to respective edges [experimental]"))
		((edgeAdjHalfAngle,"half angle between normals of this facet and the adjacent facet [experimental]"))
		#endif
	);
	DECLARE_LOGGER;
	REGISTER_CLASS_INDEX(Facet,Shape);
};

REGISTER_SERIALIZABLE(Facet);

