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

#include"Body.hpp"
#include"Cell.hpp"
#include"BodyContainer.hpp"
#include"Engine.hpp"
#include"Material.hpp"
#include"DisplayParameters.hpp"
#include"ForceContainer.hpp"
#include"GroupRelationData.hpp"
#include"InteractionContainer.hpp"

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 255 
#endif



class Bound;

class Scene: public Serializable{
	public:
		shared_ptr<Bound> bound;
		shared_ptr<BodyContainer>		bodies;
		vector<shared_ptr<Engine> >		engines;
		vector<shared_ptr<Engine> >		initializers;
		shared_ptr<InteractionContainer> interactions;
		//! Container of shared materials. Add elements using Scene::addMaterial, not directly. Do NOT remove elements from here unless you know what you are doing!
		vector<shared_ptr<Material> > materials;
		//! Adds material to Scene::materials. It also sets id of the material accordingly and returns it.
		int addMaterial(shared_ptr<Material> m){ materials.push_back(m); m->id=(int)materials.size()-1; return m->id; }
		//! Checks that type of Body::state satisfies Material::stateTypeOk. Throws runtime_error if not. (Is called from BoundDispatcher the first time it runs)
		void checkStateTypes();
		//! update our bound; used directly instead of a BoundFunctor, since we don't derive from Body anymore
		void updateBound();

		ForceContainer forces;

		//! information on periodicity; only should be used if Scene::isPeriodic
		shared_ptr<Cell> cell;
		//! store for arbitrary Serializable objects; will set static parameters during deserialization (primarily for GLDraw functors which otherwise have no attribute access)
		vector<shared_ptr<Serializable> > miscParams; 		//! tags like mp3 tags: author, date, version, description etc.
		list<string> tags;
		//! "hash maps" of display parameters (since yade::serialization has no support for maps, emulate it via vector of strings in format key=value)
		vector<shared_ptr<DisplayParameters> > dispParams;

		shared_ptr<GroupRelationData>           grpRelationData;

		Scene();

		void moveToNextTimeStep();

		/* Functions operating on TimeStepper; they all throw exception if there is more than 1 */
		// return whether a TimeStepper is present
		bool timeStepperPresent();
		// true if TimeStepper is present and active, false otherwise
		bool timeStepperActive();
		// (de)activate TimeStepper; returns whether the operation was successful (i.e. whether a TimeStepper was found)
		bool timeStepperActivate(bool activate);

		shared_ptr<Engine> engineByName(string s);
		shared_ptr<Engine> engineByLabel(string s);

		Real dt;
		long iter;
		Real time;
		long stopAtIter;
		Real stopAtVirtTime;
		Real stopAtRealTime;
		bool isPeriodic;

		bool needsInitializers;
		// for GL selection
		Body::id_t selectedBody;
	protected :
		virtual void postProcessAttributes(bool deserializing);
	REGISTER_ATTRIBUTES(Serializable,
		(tags)
		(grpRelationData)
		(engines)
		(initializers)
		(bodies)
		(interactions)
		(materials)
		(miscParams)
		(dispParams)
		(dt)
		(iter)
		(time)
		(stopAtIter)
		(isPeriodic)
		(cell)
	);
	REGISTER_CLASS_AND_BASE(Scene,Serializable);
};

REGISTER_SERIALIZABLE(Scene);


