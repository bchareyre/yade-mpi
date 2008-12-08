/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"MetaBody.hpp"
#include<yade/core/Engine.hpp>
#include<yade/core/TimeStepper.hpp>

#include<Wm3Math.h>
#include<yade/lib-base/yadeWm3.hpp>
#include<boost/foreach.hpp>
#include<boost/date_time/posix_time/posix_time.hpp>
#include<boost/algorithm/string.hpp>


/* this is meant to improve usability: MetaBody is ready by default (so is Omega by that token)
 * and different type of containers can still be used instead by explicit assignment */
#include<yade/core/BodyRedirectionVector.hpp>
#include<yade/core/InteractionVecSet.hpp>
#include<yade/core/PhysicalActionVectorVector.hpp>

// POSIX-only
#include<pwd.h>
#include<unistd.h>

MetaBody::MetaBody() :
	  Body(),bodies(new BodyRedirectionVector),persistentInteractions(new InteractionVecSet),transientInteractions(new InteractionVecSet),physicalActions(new PhysicalActionVectorVector)
{	
	engines.clear();
	initializers.clear();
	needsInitializers=true;
	currentIteration=0;
	simulationTime=0;
	stopAtIteration=0;
	isDynamic=false;
	dt=1e-8;

	// fill default tags
	struct passwd* pw;
	char hostname[HOST_NAME_MAX];
	gethostname(hostname,HOST_NAME_MAX);
	pw=getpwuid(geteuid()); if(!pw) throw runtime_error("getpwuid(geteuid()) failed!");
	// a few default tags
	// real name: will have all non-ASCII characters replaced by ? since serialization doesn't handle that
	// the standard GECOS format is Real Name,,, - first command and after will be discarded
	string gecos(pw->pw_gecos), gecos2; size_t p=gecos.find(","); if(p!=string::npos) boost::algorithm::erase_tail(gecos,gecos.size()-p); for(size_t i=0;i<gecos.size();i++){gecos2.push_back(((unsigned char)gecos[i])<128 ? gecos[i] : '?'); }
	tags.push_back(boost::algorithm::replace_all_copy(string("author=")+gecos2+" ("+string(pw->pw_name)+"@"+hostname+")"," ","~"));
	tags.push_back(string("isoTime="+boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time())));
	tags.push_back(string("id="+boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time())+"p"+lexical_cast<string>(getpid())));
	tags.push_back(string("description="));
}



void MetaBody::postProcessAttributes(bool deserializing)
{
	// this is now checked in MoveToNextTimeStep
	//runInitializers();	
	
	//	initializers.clear(); // FIXME - we want to delate ONLY some of them!
	//                                       because when you save and load file, you still want some initializers, but not all of them. Eg - you don't want VRML loader, or FEM loader, but you want BoundingVolumeMetaEngine. Maybe we need two list of initilizers? One that 'survive' between load and save, and others that are deleted on first time?
}



void MetaBody::moveToNextTimeStep()
{
	if(needsInitializers){
		FOREACH(shared_ptr<Engine> e, initializers){ if(e->isActivated()) e->action(this); }
		needsInitializers=false;
	}
	FOREACH(const shared_ptr<Engine>& e, engines){ if(e->isActivated()){ e->action(this); }}
}

shared_ptr<Engine> MetaBody::engineByName(string s){
	FOREACH(shared_ptr<Engine> e, engines){
		if(e->getClassName()==s) return e;
	}
	return shared_ptr<Engine>();
}


void MetaBody::setTimeSteppersActive(bool a)
{
	FOREACH(shared_ptr<Engine> e, engines){
		if (Omega::instance().isInheritingFrom(e->getClassName(),"TimeStepper"))
			(dynamic_pointer_cast<TimeStepper>(e))->setActive(a);
	}
}

