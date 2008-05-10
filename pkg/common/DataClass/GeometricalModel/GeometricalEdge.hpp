/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifndef GEOMETRICALEDGE_HPP
#define GEOMETRICALEDGE_HPP

#include<yade/core/GeometricalModel.hpp>

class GeometricalEdge : public GeometricalModel {
	public :
	    Vector3r edge;

	    GeometricalEdge ();
	    virtual ~GeometricalEdge ();

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(GeometricalEdge);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	REGISTER_CLASS_INDEX(GeometricalEdge,GeometricalModel);
};

REGISTER_SERIALIZABLE(GeometricalEdge,false);

#endif // GEOMETRICALEDGE_HPP

