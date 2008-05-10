/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef INTERACTINGEDGE_HPP
#define INTERACTINGEDGE_HPP

#include <yade/core/InteractingGeometry.hpp>

class InteractingEdge : public InteractingGeometry {
    public:
	Vector3r edge;
	/// Squared length of edge
	Real squaredLength;
	Vector3r normal1,normal2;
	/// "both" is true if both normal1 and normal2 are set.
	bool both;
	/// "flat" is true if normal1 == -normal2 (edge lies on a flat)
	bool flat;

	InteractingEdge();
	virtual ~InteractingEdge();

    protected:
	void registerAttributes();
	void postProcessAttributes(bool deserializing);
	REGISTER_CLASS_NAME(InteractingEdge);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);

	REGISTER_CLASS_INDEX(InteractingEdge,InteractingGeometry);
};

REGISTER_SERIALIZABLE(InteractingEdge,false);

#endif // INTERACTINGEDGE_HPP
