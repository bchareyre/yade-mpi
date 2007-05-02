/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef TETRAHEDRON_HPP
#define TETRAHEDRON_HPP

#include<vector>
#include<yade/core/GeometricalModel.hpp>
#include<Wm3Math.h>
#include<yade/lib-base/yadeWm3.hpp>

class Tetrahedron : public GeometricalModel
{
	public :
		std::vector<Vector3r> v; // vertices of tetrahedron

		Tetrahedron();
		Tetrahedron(Vector3r& v1,Vector3r& v2,Vector3r& v3,Vector3r& v4);
		virtual ~Tetrahedron();

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(Tetrahedron);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	REGISTER_CLASS_INDEX(Tetrahedron,GeometricalModel);
};

REGISTER_SERIALIZABLE(Tetrahedron,false);

#endif // TETRAHEDRON_HPP

