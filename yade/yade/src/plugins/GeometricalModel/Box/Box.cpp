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

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "OpenGLWrapper.hpp"
#include "Box.hpp"
#include <iostream>

Box::Box (Vector3r e) : CollisionGeometry()
{
	extents = e;
}

Box::Box () : CollisionGeometry()
{	
}

Box::~Box ()
{

}

void Box::postProcessAttributes(bool deserializing)
{
	CollisionGeometry::postProcessAttributes(deserializing);
}

void Box::registerAttributes()
{
	CollisionGeometry::registerAttributes();
	REGISTER_ATTRIBUTE(extents);
}

bool Box::collideWith(CollisionGeometry* )
{
	return true;

}

bool Box::loadFromFile(char * )
{
	return false;
}

void Box::glDraw()
{

  	glMaterialv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, diffuseColor);

	glColor3v(diffuseColor);
	glScalef(2*extents[0],2*extents[1],2*extents[2]);

	if (wire)
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

void Box::renderShadowVolumes(const Se3r& se3, const Vector3r& lightPos)
{
	//Vector3r pos = lightPos-se3->translation;
	
}