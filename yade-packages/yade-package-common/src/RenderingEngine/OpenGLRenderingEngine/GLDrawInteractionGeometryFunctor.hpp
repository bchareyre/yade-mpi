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

#ifndef __GLDRAWINTERACTIONGEOMETRYFUNCTOR_HPP__
#define __GLDRAWINTERACTIONGEOMETRYFUNCTOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/EngineUnit1D.hpp>
#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>
#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLDrawInteractionGeometryFunctor :	public EngineUnit1D
						<
		 					void ,
		 					TYPELIST_2(const shared_ptr<InteractingGeometry>&, const shared_ptr<PhysicalParameters>&) 
						>
{
	public : virtual ~GLDrawInteractionGeometryFunctor() {};
	REGISTER_CLASS_NAME(GLDrawInteractionGeometryFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(GLDrawInteractionGeometryFunctor,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __GLDRAWINTERACTIONGEOMETRYFUNCTOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

