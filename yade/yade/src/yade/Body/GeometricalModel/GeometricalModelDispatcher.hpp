/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
 
#ifndef GEOMETRICAL_MODEL_DISPATCHER_HPP
#define GEOMETRICAL_MODEL_DISPATCHER_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Actor.hpp"
#include "DynLibDispatcher.hpp"
#include "BodyPhysicalParameters.hpp"
#include "GeometricalModel.hpp"
#include "GeometricalModelFunctor.hpp"
#include "Body.hpp"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

class GeometricalModelDispatcher : public Actor
{
	private : DynLibDispatcher
		<	TYPELIST_2( BodyPhysicalParameters , GeometricalModel ) ,
			GeometricalModelFunctor,
			void ,
			TYPELIST_3(
					const shared_ptr<BodyPhysicalParameters>&
					, shared_ptr<GeometricalModel>&
					, const Body*
				  )
		> geometricalModelDispatcher;

	private : vector<vector<string> > geometricalModelFunctors;
	public  : void addGeometricalModelFunctors(const string& str1,const string& str2,const string& str3);

	public : virtual void action(Body* b);
	
	public : virtual void postProcessAttributes(bool deserializing);
	public : virtual void registerAttributes();
	REGISTER_CLASS_NAME(GeometricalModelDispatcher);
};

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(GeometricalModelDispatcher,false);

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#endif // GEOMETRICAL_MODEL_DISPATCHER_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
