/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PARTICLESET2MESH2D_HPP
#define PARTICLESET2MESH2D_HPP


#include <yade/yade-package-common/GeometricalModelEngineUnit.hpp>

class ParticleSet2Mesh2D : public GeometricalModelEngineUnit
{
	public :
		void go(	  const shared_ptr<PhysicalParameters>&
				, shared_ptr<GeometricalModel>&
				, const Body*);

	REGISTER_CLASS_NAME(ParticleSet2Mesh2D);
	REGISTER_BASE_CLASS_NAME(GeometricalModelEngineUnit);
};

REGISTER_SERIALIZABLE(ParticleSet2Mesh2D,false);

#endif // PARTICLESET2MESH2D_HPP

