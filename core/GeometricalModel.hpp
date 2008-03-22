/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GEOMETRICALMODEL_HPP
#define GEOMETRICALMODEL_HPP

#include<yade/lib-base/yadeWm3Extra.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>

class InteractingGeometry;

class GeometricalModel : public Serializable, public Indexable
{
	public :
		bool		 visible
				,wire
				,shadowCaster;

		Vector3r	diffuseColor;

		GeometricalModel(): visible(true),wire(false),shadowCaster(false),diffuseColor(Vector3r(1,1,1)){}

	protected : 
		void registerAttributes();

	REGISTER_CLASS_NAME(GeometricalModel);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);
	REGISTER_INDEX_COUNTER(GeometricalModel);
};

REGISTER_SERIALIZABLE(GeometricalModel,false);

#endif //  GEOMETRICALMODEL_HPP

