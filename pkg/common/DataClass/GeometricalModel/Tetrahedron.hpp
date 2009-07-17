/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<vector>
#include<yade/core/GeometricalModel.hpp>
#include<Wm3Math.h>
#include<yade/lib-base/yadeWm3.hpp>

class Tetrahedron : public GeometricalModel
{
	public :
	// FIXME - we can go back to C array when we migrate to boost::serialization. yade::serialization does not support C arrays
		std::vector<Vector3r> v; // vertices of tetrahedron
//		Vector3r v[4]; // vertices of tetrahedron

		Tetrahedron();
		Tetrahedron(const Vector3r& v0, const Vector3r& v1, const Vector3r& v2, const Vector3r& v3);
		void dump(void) const{std::cerr<<"["; for(int i=0; i<4; i++){ std::cerr<<"("; for(int j=0; j<3; j++) std::cerr<<v[i][j]<<","; std::cerr<<")";} std::cerr<<"]"<<std::endl;}
		virtual ~Tetrahedron();

	REGISTER_ATTRIBUTES(GeometricalModel,(v));
	REGISTER_CLASS_NAME(Tetrahedron);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	REGISTER_CLASS_INDEX(Tetrahedron,GeometricalModel);
};

REGISTER_SERIALIZABLE(Tetrahedron);


