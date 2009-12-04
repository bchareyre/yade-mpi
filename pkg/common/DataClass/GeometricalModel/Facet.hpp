/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/GeometricalModel.hpp>
#ifdef YADE_GEOMETRICALMODEL
class Facet : public GeometricalModel {
	public :
	vector<Vector3r> vertices;
	Facet ();
	virtual ~Facet ();

	REGISTER_ATTRIBUTES(GeometricalModel,(vertices))
	REGISTER_CLASS_AND_BASE(Facet,GeometricalModel);
	REGISTER_CLASS_INDEX(Facet,GeometricalModel);
};

REGISTER_SERIALIZABLE(Facet);
#else
	#error This file may be included only with the 'shape' feature enabled.
#endif

