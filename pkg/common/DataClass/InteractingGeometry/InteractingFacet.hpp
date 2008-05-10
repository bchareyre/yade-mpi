/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef INTERACTINGFACET_HPP
#define INTERACTINGFACET_HPP

#include <yade/core/InteractingGeometry.hpp>

class InteractingFacet : public InteractingGeometry {
    public:
	/// Vertices of facet
	vector<Vector3r> vertices;
	/// Facets normal
	Vector3r normal; 
	/// Normals of edges (directed into facet)
	vector<Vector3r> edgeNormals;
	/// Center of mass 
	Vector3r cm; 

	InteractingFacet();

	virtual ~InteractingFacet();

    protected:
	void registerAttributes();
	void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(InteractingFacet);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

	REGISTER_CLASS_INDEX(InteractingFacet,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingFacet,false);

#endif // INTERACTINGFACET_HPP
