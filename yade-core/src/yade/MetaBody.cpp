/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include <yade/yade-lib-wm3-math/Math.hpp>
#include "MetaBody.hpp"
#include "Engine.hpp"
#include "TimeStepper.hpp"

// FIXME - who is to decide which class to use by default? -- Olivier : I think nobody ! It will be done automatically while deserializing
MetaBody::MetaBody() :
	  Body()
	//, bodies(new BodyRedirectionVector)
	//, persistentInteractions(new InteractionVecSet)
	//, volatileInteractions(new InteractionVecSet)
	//, actionParameters(new PhysicalActionVectorVector)
{	
	actors.clear();
	initializers.clear();
}


void MetaBody::postProcessAttributes(bool deserializing)
{
	if (deserializing)
	{
		vector<shared_ptr<Engine> >::iterator i    = initializers.begin();
		vector<shared_ptr<Engine> >::iterator iEnd = initializers.end();
		for( ; i != iEnd ; ++i)
			if ((*i)->isActivated())
				(*i)->action(this);
				
	//	initializers.clear(); // FIXME - we want to delate ONLY some of them!
	//                                       because when you save and load file, you still want some initializers, but not all of them. Eg - you don't want VRML loader, or FEM loader, but you want BoundingVolumeMetaEngine. Maybe we need two list of initilizers? One that 'survive' between load and save, and others that are deleted on first time?
	}
}


void MetaBody::registerAttributes()
{
	Body::registerAttributes();
	REGISTER_ATTRIBUTE(actors);
	REGISTER_ATTRIBUTE(initializers);
	REGISTER_ATTRIBUTE(bodies);
	REGISTER_ATTRIBUTE(volatileInteractions);
	REGISTER_ATTRIBUTE(persistentInteractions);
	REGISTER_ATTRIBUTE(actionParameters); // FIXME - needed or not ? - Olivier : yes it is needed if there is no default initialization into constructor
}


void MetaBody::moveToNextTimeStep()
{
	vector<shared_ptr<Engine> >::iterator ai    = actors.begin();
	vector<shared_ptr<Engine> >::iterator aiEnd = actors.end();
	for( ; ai!=aiEnd ; ++ai )
	{
		if ((*ai)->isActivated())
			(*ai)->action(this);
	}
}


void MetaBody::setTimeSteppersActive(bool a)
{
	vector<shared_ptr<Engine> >::iterator ai    = actors.begin();
	vector<shared_ptr<Engine> >::iterator aiEnd = actors.end();
	for( ; ai!=aiEnd ; ++ai )
	{
		if (Omega::instance().isInheritingFrom((*ai)->getClassName(),"TimeStepper"))
			(dynamic_pointer_cast<TimeStepper>(*ai))->setActive(a);
		
/*		map<string,DynlibType>::const_iterator dli = Omega::instance().getDynlibsType().find((*ai)->getClassName());
		if (dli!=Omega::instance().getDynlibsType().end())
		{
			if ((*dli).second.baseClass=="TimeStepper")
				(dynamic_pointer_cast<TimeStepper>(*ai))->setActive(a);
		}*/
	}
}

