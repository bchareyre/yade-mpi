/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
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

#ifndef __INTERACTIONPHYSICSFUNCTOR_H__
#define __INTERACTIONPHYSICSFUNCTOR_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/EngineUnit2D.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all interaction functor.

	Every functions that describe interaction between two InteractionGeometries must derive from InteractionGeometryEngineUnit.
*/

class InteractionPhysicsEngineUnit : 	public EngineUnit2D
					<
		 				void ,
		 				TYPELIST_3(	  const shared_ptr<PhysicalParameters>&
								, const shared_ptr<PhysicalParameters>&
								, const shared_ptr<Interaction>&
			   				  ) 
					>
{
	REGISTER_CLASS_NAME(InteractionPhysicsEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);

};

REGISTER_SERIALIZABLE(InteractionPhysicsEngineUnit,false);

#endif // __INTERACTIONPHYSICSFUNCTOR_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
