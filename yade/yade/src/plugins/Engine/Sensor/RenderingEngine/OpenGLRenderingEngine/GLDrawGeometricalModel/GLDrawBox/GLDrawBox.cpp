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

#include "GLDrawBox.hpp"
#include <yade-common/Box.hpp>
#include <yade-lib-opengl/OpenGLWrapper.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

void GLDrawBox::go(const shared_ptr<GeometricalModel>& gm, const shared_ptr<PhysicalParameters>&,bool wire)
{
	// FIXME : check that : one of those 2 lines are useless
  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gm->diffuseColor);
	glColor3v(gm->diffuseColor);
	
	Vector3r &extents = (static_cast<Box*>(gm.get()))->extents;
	
	glScalef(2*extents[0],2*extents[1],2*extents[2]);

	if (gm->wire || wire)
	{
		glDisable(GL_LIGHTING);
		glutWireCube(1);
	}
	else
	{
		glEnable(GL_LIGHTING);
		glutSolidCube(1);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

