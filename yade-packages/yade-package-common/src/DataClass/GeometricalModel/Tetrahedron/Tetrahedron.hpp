/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TETRAHEDRON_HPP
#define TETRAHEDRON_HPP

#include <yade/yade-core/GeometricalModel.hpp>
#include <yade/yade-lib-wm3-math/Math.hpp>

class Tetrahedron : public GeometricalModel
{
	public :
		Vector3r	v1,v2,v3,v4; // vertices of tetrahedron

		Tetrahedron();
		Tetrahedron(Vector3r& p1,Vector3r& p2,Vector3r& p3,Vector3r& p4);
		virtual ~Tetrahedron();

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(Tetrahedron);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	REGISTER_CLASS_INDEX(Tetrahedron,GeometricalModel);
};

REGISTER_SERIALIZABLE(Tetrahedron,false);

#endif // TETRAHEDRON_HPP

