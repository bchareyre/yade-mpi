/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "BoundingSphere.hpp"

BoundingSphere::BoundingSphere () : BoundingVolume()
{
	createIndex();
}

BoundingSphere::~BoundingSphere ()
{
}

void BoundingSphere::registerAttributes() // FIXME - remove that function, dispatcher shoul take care of that
{
	BoundingVolume::registerAttributes();
	REGISTER_ATTRIBUTE(radius);
	REGISTER_ATTRIBUTE(center);
}

// void BoundingSphere::update(Se3r& se3)
// {
// 	Vector3r v(radius,radius,radius);
// 	center = se3.position;
// 	min = center-v;
// 	max = center+v;	
// }
YADE_PLUGIN();
