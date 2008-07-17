/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef METABODY_HPP
#define METABODY_HPP

#include"Body.hpp"
#include"BodyContainer.hpp"
#include"Engine.hpp"
#include"DisplayParameters.hpp"

class MetaBody : public Body
{
	public :
		shared_ptr<BodyContainer>		bodies;
		vector<shared_ptr<Engine> >		engines;
		vector<shared_ptr<Engine> >		initializers; // FIXME: see MovingSupport:50
		__attribute__((__deprecated__)) shared_ptr<InteractionContainer>	persistentInteractions; // disappear, reappear according to physical (or any other non-spatial) criterion
		shared_ptr<InteractionContainer>	transientInteractions;	// disappear, reappear according to spatial criterion
		shared_ptr<PhysicalActionContainer>	physicalActions;
		vector<shared_ptr<Serializable> > miscParams; // will set static parameters during deserialization (primarily for GLDraw functors which otherwise have no attribute access)
		//! tags like mp3 tags: author, date, version, description etc.
		list<string> tags;
		//! "hash maps" of display parameters
		vector<shared_ptr<DisplayParameters> > dispParams;

		MetaBody ();

		void moveToNextTimeStep();

		void setTimeSteppersActive(bool a);
		shared_ptr<Engine> engineByName(string s);

		Real dt;
		long currentIteration;
		Real simulationTime;
		long stopAtIteration;

		bool needsInitializers;
/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(MetaBody);
	REGISTER_BASE_CLASS_NAME(Body);
};

REGISTER_SERIALIZABLE(MetaBody,false);

#endif // METABODY_HPP

