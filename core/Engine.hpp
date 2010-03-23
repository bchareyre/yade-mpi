/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-serialization/Serializable.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Timing.hpp>

#include<boost/foreach.hpp>
#ifndef FOREACH
#define FOREACH BOOST_FOREACH
#endif

class Body;
class Scene;

class Engine: public Serializable{
	public:
		// pointer to the simulation, set at every step by Scene::moveToNextTimeStep
		Scene* scene;
		//! high-level profiling information; not serializable
		TimingInfo timingInfo; 
		//! precise profiling information (timing of fragments of the engine)
		shared_ptr<TimingDeltas> timingDeltas;
		virtual ~Engine() {};
	
		virtual bool isActivated() { return true; };
		virtual void action() { throw; };
	private:
		// py access funcs	
		TimingInfo::delta timingInfo_nsec_get(){return timingInfo.nsec;};
		void timingInfo_nsec_set(TimingInfo::delta d){ timingInfo.nsec=d;}
		long timingInfo_nExec_get(){return timingInfo.nExec;};
		void timingInfo_nExec_set(long d){ timingInfo.nExec=d;}
		void explicitAction(){ scene=Omega::instance().getScene().get(); this->action(); }

	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(Engine,Serializable,"Basic execution unit of simulation, called from the simulation loop (O.engines)",
		((string,label,,"Textual label for this object; must be valid python identifier, you can refer to it directly from python.")),
		/* ctor */ scene=Omega::instance().getScene().get() ,
		/* py */
		.add_property("execTime",&Engine::timingInfo_nsec_get,&Engine::timingInfo_nsec_set,"Cummulative time this Engine took to run (only used if O.timingEnabled==True).")
		.add_property("execCount",&Engine::timingInfo_nExec_get,&Engine::timingInfo_nExec_set,"Cummulative count this engine was run (only used if O.timingEnabled==True).")
		.def_readonly("timingDeltas",&Engine::timingDeltas,"Detailed information about timing inside the Engine itself. Empty unless enabled in the source code and O.timingEnabled==True.")
		.def("__call__",&Engine::explicitAction)
	);
};
REGISTER_SERIALIZABLE(Engine);



