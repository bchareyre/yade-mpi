/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Bound.hpp>

class AABB : public Bound
{
	public :
		Vector3r	 halfSize
				,center;

		AABB();
		virtual ~AABB();
	
/// Serialization
	REGISTER_CLASS_AND_BASE(AABB,Bound);	
	REGISTER_ATTRIBUTES(Bound,/* no attributes */);
/// Indexable
	REGISTER_CLASS_INDEX(AABB,Bound);
};
REGISTER_SERIALIZABLE(AABB);


