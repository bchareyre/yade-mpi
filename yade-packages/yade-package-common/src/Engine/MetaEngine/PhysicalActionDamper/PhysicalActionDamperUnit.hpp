/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef PHYSICALACTIONDAMPERUNIT_HPP
#define PHYSICALACTIONDAMPERUNIT_HPP

#include <yade/yade-core/PhysicalAction.hpp>
#include <yade/yade-core/PhysicalParameters.hpp>
#include <yade/yade-core/Body.hpp>
#include <yade/yade-core/EngineUnit2D.hpp>

class PhysicalActionDamperUnit : public EngineUnit2D
				 <
		 			void ,
		 			TYPELIST_3(	  const shared_ptr<PhysicalAction>&
							, const shared_ptr<PhysicalParameters>&
							, const Body*
						   )
				>
{
	REGISTER_CLASS_NAME(PhysicalActionDamperUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);
};

REGISTER_SERIALIZABLE(PhysicalActionDamperUnit,false);

#endif // __PHYSICALACTIONDAMPINGUNIT_HPP__
