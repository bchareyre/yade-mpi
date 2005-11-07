/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ACTION_PARAMETER_RESET_HPP
#define ACTION_PARAMETER_RESET_HPP 

#include <yade/yade-core/Engine.hpp>

class Body;

class PhysicalActionContainerReseter : public Engine
{
	public :
		PhysicalActionContainerReseter();
		virtual ~PhysicalActionContainerReseter();
		virtual void action(Body* body);

	REGISTER_CLASS_NAME(PhysicalActionContainerReseter);
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(PhysicalActionContainerReseter,false);

#endif // ACTION_PARAMETER_RESET_HPP 

