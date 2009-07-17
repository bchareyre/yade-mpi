/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/BoundingVolume.hpp>

class BoundingSphere : public BoundingVolume
{
	public :
		Real		radius;
		Vector3r	center;

		BoundingSphere();
		virtual ~BoundingSphere();

	REGISTER_ATTRIBUTES(BoundingVolume,(radius)(center));
	REGISTER_CLASS_AND_BASE(BoundingSphere,BoundingVolume);
	REGISTER_CLASS_INDEX(BoundingSphere,BoundingVolume);
};
REGISTER_SERIALIZABLE(BoundingSphere);


