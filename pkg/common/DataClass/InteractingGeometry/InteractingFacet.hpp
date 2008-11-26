/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef INTERACTINGIFACET_HPP
#define INTERACTINGIFACET_HPP


#include <yade/core/InteractingGeometry.hpp>


class InteractingFacet : public InteractingGeometry {
    public:
	
	// Registered attributes
		
	/// Vertex positions in the local reference frame
	vector<Vector3r> vertices;

	InteractingFacet();
	virtual ~InteractingFacet();
	
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

	protected:

	void registerAttributes(); void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(InteractingFacet);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

	REGISTER_CLASS_INDEX(InteractingFacet,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingFacet);

#endif // INTERACTINGIFACET_HPP
