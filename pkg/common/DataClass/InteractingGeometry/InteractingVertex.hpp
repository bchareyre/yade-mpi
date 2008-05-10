/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef INTERACTINGVERTEX_HPP
#define INTERACTINGVERTEX_HPP

#include <yade/core/InteractingGeometry.hpp>

class InteractingVertex : public InteractingGeometry {
    public:

	vector<Vector3r> normals;
	/// This vertex lies on a flat
	bool flat;

	InteractingVertex();
	virtual ~InteractingVertex();

    protected:
	void registerAttributes();
	REGISTER_CLASS_NAME(InteractingVertex);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

	REGISTER_CLASS_INDEX(InteractingVertex,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingVertex,false);

#endif // INTERACTINGVERTEX_HPP
