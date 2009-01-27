/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/StandAloneEngine.hpp>

#include <vector>
#include <string>

class Body;

class PhysicalActionContainerInitializer : public StandAloneEngine
{
	public :
		std::vector<std::string> physicalActionNames;
	
		PhysicalActionContainerInitializer();
		virtual ~PhysicalActionContainerInitializer();
		virtual void action(MetaBody*);
	
	protected : 
		virtual void registerAttributes();

	DECLARE_LOGGER;

	REGISTER_CLASS_NAME(PhysicalActionContainerInitializer);
	REGISTER_BASE_CLASS_NAME(StandAloneEngine);
};

REGISTER_SERIALIZABLE(PhysicalActionContainerInitializer);


