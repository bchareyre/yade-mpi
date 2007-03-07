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
	physicalActionNames.clear();
}

PhysicalActionContainerInitializer::~PhysicalActionContainerInitializer() 
{
}

void PhysicalActionContainerInitializer::registerAttributes()
{
	REGISTER_ATTRIBUTE(physicalActionNames);
}

void PhysicalActionContainerInitializer::action(Body* body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	
	vector<shared_ptr<PhysicalAction> > physicalActions;
	physicalActions.clear();
	
	for(unsigned int i = 0 ; i < physicalActionNames.size() ; ++i )
		physicalActions.push_back(
			YADE_PTR_CAST<PhysicalAction>
				(ClassFactory::instance().createShared(physicalActionNames[i]))
		);
	
	ncb->physicalActions->prepare(physicalActions);
	
}


