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

#ifndef __GLDRAWBOUNDINGVOLUMEFUNCTOR_HPP__
#define __GLDRAWBOUNDINGVOLUMEFUNCTOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-multimethods/FunctorWrapper.hpp>
#include <yade/yade-core/BoundingVolume.hpp>
#include <yade/yade-core/EngineUnit1D.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class GLDrawBoundingVolumeFunctor : 	public EngineUnit1D
					<
		 				void ,
		 				TYPELIST_1(const shared_ptr<BoundingVolume>&) 
					>
{
	public : virtual ~GLDrawBoundingVolumeFunctor() {};
	REGISTER_CLASS_NAME(GLDrawBoundingVolumeFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(GLDrawBoundingVolumeFunctor,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __GLDRAWBOUNDINGVOLUMEFUNCTOR_HPP__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

