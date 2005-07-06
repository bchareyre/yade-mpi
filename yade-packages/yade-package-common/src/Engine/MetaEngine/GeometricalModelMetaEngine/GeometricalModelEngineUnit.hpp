/***************************************************************************
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __GEOMETRICALMODELFACTORY_H__
#define __GEOMETRICALMODELFACTORY_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-serialization/Serializable.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all geometrical model factories.
 OLD: FIXME, what is written below is a description of BodyFactory. move it there
 
	It is used for creating a geometrical model from a given set of parameters.
	This is very useful when you want to load a file that contains geometrical data or when
	you want to build an object with that depends on several parameters.
	
 NEW: FIXME - delete comment that is above
 
 	It is used when you want to update GeometricalModel of your Body during runtime. Only Physical
	Parameters of the Body can affect "perfect geometrical representation"(GeometricalModel) of
	the Body. So it is given as first argument.
*/
class GeometricalModelEngineUnit : public FunctorWrapper
		<
		 void ,
		 TYPELIST_3(
		 		  const shared_ptr<PhysicalParameters>&
				, shared_ptr<GeometricalModel>&
				, const Body*
			   )
		>
{	
	REGISTER_CLASS_NAME(GeometricalModelEngineUnit);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(GeometricalModelEngineUnit,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __GEOMETRICALMODELFACTORY_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
