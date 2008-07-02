/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALPARAMETERS_HPP
#define PHYSICALPARAMETERS_HPP

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>

class PhysicalParameters : public Serializable, public Indexable
{
	public:
			PhysicalParameters() : se3(Vector3r(0,0,0), Quaternionr(1,0,0,0)) {}
		Se3r se3;

	protected :
		void registerAttributes();
	REGISTER_CLASS_NAME(PhysicalParameters);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);
	REGISTER_INDEX_COUNTER(PhysicalParameters);
};

REGISTER_SERIALIZABLE(PhysicalParameters,false);

#endif // __BodyPhysicalParameters_HPP__

