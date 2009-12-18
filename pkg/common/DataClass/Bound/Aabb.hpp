/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/Bound.hpp>

/*! Representation of bound by min and max points.

This class is redundant, since it has no data members; don't delete it, though,
as Bound::{min,max} might move here one day.

*/
class Aabb : public Bound{
	public :
		Aabb();
		virtual ~Aabb();
	
	REGISTER_CLASS_AND_BASE(Aabb,Bound);	
	REGISTER_ATTRIBUTES(Bound, /* (min)(max) */ );  // not necessary to store min and max, but it is handy for debugging/python inspection 
	REGISTER_CLASS_INDEX(Aabb,Bound);
};
REGISTER_SERIALIZABLE(Aabb);


