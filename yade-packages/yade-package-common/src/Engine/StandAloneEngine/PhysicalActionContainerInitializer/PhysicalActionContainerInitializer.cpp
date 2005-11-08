/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalActionContainerInitializer.hpp"
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-lib-factory/ClassFactory.hpp>
#include <yade/yade-core/PhysicalAction.hpp>


PhysicalActionContainerInitializer::PhysicalActionContainerInitializer() 
{
	actionParameterNames.clear();
}

PhysicalActionContainerInitializer::~PhysicalActionContainerInitializer() 
{
}

void PhysicalActionContainerInitializer::registerAttributes()
{
	REGISTER_ATTRIBUTE(actionParameterNames);
}

void PhysicalActionContainerInitializer::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	
	vector<shared_ptr<PhysicalAction> > actionParameters;
	actionParameters.clear();
	
	for(unsigned int i = 0 ; i < actionParameterNames.size() ; ++i )
		actionParameters.push_back(
			dynamic_pointer_cast<PhysicalAction>
				(ClassFactory::instance().createShared(actionParameterNames[i]))
		);
	
	ncb->actionParameters->prepare(actionParameters);
	
}


