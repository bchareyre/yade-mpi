/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#include<yade/core/GeometricalModel.hpp>


class Box : public GeometricalModel
{
	public :
		Vector3r extents;

		Box ();
		virtual ~Box ();
	
/// Serialization
	
	REGISTER_CLASS_NAME(Box);
	REGISTER_BASE_CLASS_NAME(GeometricalModel);

	public : void registerAttributes();
	
/// Indexable
	
	REGISTER_CLASS_INDEX(Box,GeometricalModel);

};

REGISTER_SERIALIZABLE(Box);


