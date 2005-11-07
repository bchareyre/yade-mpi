/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GEOMETRICALMODEL_HPP
#define GEOMETRICALMODEL_HPP

#include <yade/yade-lib-wm3-math/Se3.hpp>
#include <yade/yade-lib-wm3-math/Vector3.hpp>
#include <yade/yade-lib-serialization/Serializable.hpp>
#include <yade/yade-lib-multimethods/Indexable.hpp>

class InteractingGeometry;

class GeometricalModel : public Serializable, public Indexable
{
	public :
		bool		 visible
				,wire
				,shadowCaster;

		Vector3f	diffuseColor;

	protected : 
		void registerAttributes();

	REGISTER_CLASS_NAME(GeometricalModel);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);
	REGISTER_INDEX_COUNTER(GeometricalModel);
};

REGISTER_SERIALIZABLE(GeometricalModel,false);

#endif //  GEOMETRICALMODEL_HPP

