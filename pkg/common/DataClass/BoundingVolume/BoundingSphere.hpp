/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BOUNDINGSPHERE_HPP
#define BOUNDINGSPHERE_HPP

#include<yade/core/BoundingVolume.hpp>

class BoundingSphere : public BoundingVolume
{
	public :
		Real		radius;
		Vector3r	center;

		BoundingSphere();
		virtual ~BoundingSphere();

/// Serialization
		void registerAttributes();

	REGISTER_CLASS_NAME(BoundingSphere);
	REGISTER_BASE_CLASS_NAME(BoundingVolume);
	
/// Indexable
	REGISTER_CLASS_INDEX(BoundingSphere,BoundingVolume);
};

REGISTER_SERIALIZABLE(BoundingSphere,false);

#endif // __BOUNDINGSPHERE_H__

