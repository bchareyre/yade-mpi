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

#include "Tetrahedron2PolyhedralSweptSphere.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-common/Tetrahedron.hpp>
#include <yade/yade-common/PolyhedralSweptSphere.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

void Tetrahedron2PolyhedralSweptSphere::go(	const shared_ptr<GeometricalModel>& gm,
				shared_ptr<InteractingGeometry>& ig,
				const Se3r& se3,
				const Body*	)
{
	Tetrahedron* tet = static_cast<Tetrahedron*>(gm.get());
	PolyhedralSweptSphere* pss = static_cast<PolyhedralSweptSphere*>(ig.get());
	
}
	
////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
