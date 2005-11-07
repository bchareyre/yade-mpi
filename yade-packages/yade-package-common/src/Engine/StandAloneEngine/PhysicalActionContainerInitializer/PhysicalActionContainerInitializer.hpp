/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ACTION_PARAMETER_INITIALIZER_HPP
#define ACTION_PARAMETER_INITIALIZER_HPP 

#include <yade/yade-core/Engine.hpp>

#include <vector>
#include <string>

class Body;

class PhysicalActionContainerInitializer : public Engine
{
	public :
		std::vector<std::string> actionParameterNames;
	
		PhysicalActionContainerInitializer();
		virtual ~PhysicalActionContainerInitializer();
		virtual void action(Body* body);
	
	protected : 
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(PhysicalActionContainerInitializer);
	REGISTER_BASE_CLASS_NAME(Engine);
};

REGISTER_SERIALIZABLE(PhysicalActionContainerInitializer,false);

#endif // ACTION_PARAMETER_INITIALIZER_HPP

