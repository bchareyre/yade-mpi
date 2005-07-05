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

#include "BoundingSphere.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-lib-opengl/OpenGLWrapper.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

BoundingSphere::BoundingSphere () : BoundingVolume()
{
	createIndex();
}

BoundingSphere::~BoundingSphere ()
{
}

void BoundingSphere::registerAttributes() // FIXME - remove that function, dispatcher shoul take care of that
{
	BoundingVolume::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
	REGISTER_ATTRIBUTE(center);
}

// void BoundingSphere::update(Se3r& se3)
// {
// 	Vector3r v(radius,radius,radius);
// 	center = se3.position;
// 	min = center-v;
// 	max = center+v;	
// }
