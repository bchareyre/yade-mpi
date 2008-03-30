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

#include "Body.hpp"
#include "BodyContainer.hpp"
#include "Engine.hpp"

class MetaBody : public Body
{
	public : // FIXME - why public?
		shared_ptr<BodyContainer>		bodies;
		vector<shared_ptr<Engine> >		engines;
		vector<shared_ptr<Engine> >		initializers; // FIXME: see MovingSupport:50
		shared_ptr<InteractionContainer>	persistentInteractions; // disappear, reappear according to physical (or any other non-spatial) criterion
		shared_ptr<InteractionContainer>	transientInteractions;	// disappear, reappear according to spatial criterion
		shared_ptr<PhysicalActionContainer>	physicalActions;

		MetaBody ();

		void moveToNextTimeStep();

		void setTimeSteppersActive(bool a); // FIXME - wtf ?

		/* Recovery stuff: copy of state variables from Omega that should be copied back over. 	
		 *	Copying is being done ine Omega::loadSimulation and Omega::saveSimulation -- keep vars in sync.
		 * */
		bool recover;
		long recoverCurrentIteration;
		long recoverStopAtIteration;
		Real recoverSimulationTime;

		Real dt;


		void runInitializers();
/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(MetaBody);
	REGISTER_BASE_CLASS_NAME(Body);
};

REGISTER_SERIALIZABLE(MetaBody,false);

#endif // METABODY_HPP

