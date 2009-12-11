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
class FacetModel : public GeometricalModel {
	public :
	vector<Vector3r> vertices;
	FacetModel ();
	virtual ~FacetModel ();

	REGISTER_ATTRIBUTES(GeometricalModel,(vertices))
	REGISTER_CLASS_AND_BASE(FacetModel,GeometricalModel);
	REGISTER_CLASS_INDEX(FacetModel,GeometricalModel);
};

REGISTER_SERIALIZABLE(FacetModel);
#else
	#error This file may be included only with the 'shape' feature enabled.
#endif

