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

#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "CollisionGeometry.hpp"

class Sphere : public CollisionGeometry
{
	public : Real radius;

		int number;

	private : static vector<Vector3r> vertices;
	private : static vector<Vector3r> faces;
	private : static int glWiredSphereList;
	private : static int glSphereList;
	private : void subdivideTriangle(Vector3r& v1,Vector3r& v2,Vector3r& v3, int depth);
	private : void drawSphere(int depth);

	// construction
	public : Sphere (Real r);
	public : Sphere ();
	public : ~Sphere ();

	public : void glDraw();

	public : void buildDisplayList();
	public : void renderShadowVolumes(const Se3r& se3, const Vector3r& lightPos);
	

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Serialization										///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_NAME(Sphere);
	protected : virtual void postProcessAttributes(bool deserializing);
	public : void registerAttributes();

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Indexable											///
///////////////////////////////////////////////////////////////////////////////////////////////////
	
	REGISTER_CLASS_INDEX(Sphere,CollisionGeometry);
	
};

REGISTER_SERIALIZABLE(Sphere,false);

#endif // __SPHERE_H__
