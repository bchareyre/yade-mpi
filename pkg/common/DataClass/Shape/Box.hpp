/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#include<yade/core/Shape.hpp>


class Box : public Shape
{
	public :
		Vector3r extents;

		Box ();
		Box(const Vector3r& _extents): extents(_extents){}
		virtual ~Box ();	
	
	REGISTER_ATTRIBUTES(Shape,(extents));
	REGISTER_CLASS_INDEX(Box,Shape);
	REGISTER_CLASS_NAME(Box);
	REGISTER_BASE_CLASS_NAME(Shape);


};

REGISTER_SERIALIZABLE(Box);


