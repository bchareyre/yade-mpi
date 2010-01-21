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
		//! user-definable label, to convenienty retrieve this particular engine instance even if multiple engines of the same type exist
		string label;
		//! high-level profiling information; not serializable
		TimingInfo timingInfo; 
		//! precise profiling information (timing of fragments of the engine)
		shared_ptr<TimingDeltas> timingDeltas;
		Engine(): scene(NULL) {};
		virtual ~Engine() {};
	
		virtual bool isActivated(Scene*) { return true; };
		virtual void action(Scene*) { throw; };
	private:
		// py access funcs	
		TimingInfo::delta timingInfo_nsec_get(){return timingInfo.nsec;};
		void timingInfo_nsec_set(TimingInfo::delta d){ timingInfo.nsec=d;}
		long timingInfo_nExec_get(){return timingInfo.nExec;};
		void timingInfo_nExec_set(long d){ timingInfo.nExec=d;}
		//void explicitAction(){ scene=Omega::instance().getScene().get(); this->action(scene); }

	YADE_CLASS_BASE_ATTRS_PY(Engine,Serializable,(label),
		.add_property("execTime",&Engine::timingInfo_nsec_get,&Engine::timingInfo_nsec_set)
		.add_property("execCount",&Engine::timingInfo_nExec_get,&Engine::timingInfo_nExec_set)
		.def_readonly("timingDeltas",&Engine::timingDeltas)
		//.def("__call__",&Engine::explicitAction);
	)
};
REGISTER_SERIALIZABLE(Engine);



