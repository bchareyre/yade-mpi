/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "PhysicalActionContainerInitializer.hpp"
#include<yade/core/MetaBody.hpp>
#include<yade/lib-factory/ClassFactory.hpp>
#include<yade/core/PhysicalAction.hpp>
#include<yade/core/Engine.hpp>
#include<boost/foreach.hpp>

CREATE_LOGGER(PhysicalActionContainerInitializer);

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

void PhysicalActionContainerInitializer::action(MetaBody* ncb)
{
	list<string> allNames;
	// copy physical action names that were passed by the user directly
	allNames.insert(allNames.end(),physicalActionNames.begin(),physicalActionNames.end());
	LOG_DEBUG("allNames as defined by the user: ");	FOREACH(string an,allNames) LOG_DEBUG(an);
	// loop over all engines, get Bex from them
	FOREACH(shared_ptr<Engine> e, ncb->engines){
		list<string> bex=e->getNeededBex();
		allNames.insert(allNames.end(),bex.begin(),bex.end());
		LOG_DEBUG("The following engines were inserted by "<<e->getClassName()<<":"); FOREACH(string b,bex) LOG_DEBUG(b);
	}
	LOG_DEBUG("allNames after loop over engines: ");	FOREACH(string an,allNames) LOG_DEBUG(an);
	// eliminate all duplicates
	allNames.sort();
	allNames.unique();
	LOG_DEBUG("allNames after sort and unique: ");	FOREACH(string an,allNames) LOG_DEBUG(an);

	vector<shared_ptr<PhysicalAction> > physicalActions;
	FOREACH(string physicalActionName, allNames){
		physicalActions.push_back(YADE_PTR_CAST<PhysicalAction>(ClassFactory::instance().createShared(physicalActionName)));
	}
	ncb->physicalActions->prepare(physicalActions);
	
}


YADE_PLUGIN();
