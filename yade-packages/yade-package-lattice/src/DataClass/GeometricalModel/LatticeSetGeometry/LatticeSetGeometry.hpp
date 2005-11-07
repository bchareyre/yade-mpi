/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef LATTICE_SET_GEOMETRY_HPP
#define LATTICE_SET_GEOMETRY_HPP 

#include <yade/yade-core/GeometricalModel.hpp>

class LatticeSetGeometry : public GeometricalModel
{
	public :
		LatticeSetGeometry();
		virtual ~LatticeSetGeometry ();
	
/// Serialization
	REGISTER_CLASS_NAME(LatticeSetGeometry);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);
	
/// Indexable
	REGISTER_CLASS_INDEX(LatticeSetGeometry,GeometricalModel);
};

REGISTER_SERIALIZABLE(LatticeSetGeometry,false);

#endif // __LATTICESETGEOMETRY_HPP__

