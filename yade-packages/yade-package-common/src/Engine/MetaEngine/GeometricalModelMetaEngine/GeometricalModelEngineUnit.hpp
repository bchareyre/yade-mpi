/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GEOMETRICAL_MODEL_ENGINE_UNIT_HPP
#define GEOMETRICAL_MODEL_ENGINE_UNIT_HPP


#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/EngineUnit2D.hpp>


/*! \brief
	Abstract interface for building GeometricalModel depending on PhysicalParameters (currently used for updating geometry if physics has changed and affects it).

 	It is used when you want to update GeometricalModel of your Body during runtime. Only Physical
	Parameters of the Body can affect "perfect geometrical representation"(GeometricalModel) of
	the Body. So it is given as first argument.


DEPRECATED explanation (will move do BodyFactory perhaps?):

	It is used for creating a geometrical model from a given set of parameters.
	This is very useful when you want to load a file that contains geometrical data or when
	you want to build an object with that depends on several parameters.
	
*/
class GeometricalModelEngineUnit : 	public EngineUnit2D
					<
		 				void ,
		 				TYPELIST_3(	  const shared_ptr<PhysicalParameters>&
								, shared_ptr<GeometricalModel>&
								, const Body*
			   				  )
					>
{	
	REGISTER_CLASS_NAME(GeometricalModelEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);
};

REGISTER_SERIALIZABLE(GeometricalModelEngineUnit,false);

#endif //  GEOMETRICAL_MODEL_ENGINE_UNIT_HPP

