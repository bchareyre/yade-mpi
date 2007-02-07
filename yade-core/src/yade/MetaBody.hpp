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
		#ifdef HIGHLEVEL_CLUMPS
			// body Id's are unsigned ints (shouldn't be typedef'ed somewhere?)
			vector<vector<unsigned int> > clumpMembers; // clumpMembers[i] contains bodyId's of all members of clump #i (i>=0); they need not to be sorted (this may change, though)
			vector<unsigned int> clumpParameterBodies; // clumpParameterBodies[i] is ID of body in this->bodies that has physical information for clump #i; the body has isDynamic==false, it serves only as convenient parameter storage
		#endif
	
		MetaBody ();

		void moveToNextTimeStep();

		void setTimeSteppersActive(bool a); // FIXME - wtf ?

/// Serialization
	protected :
		virtual void postProcessAttributes(bool deserializing);
		virtual void registerAttributes();
	REGISTER_CLASS_NAME(MetaBody);
	REGISTER_BASE_CLASS_NAME(Body);
};

REGISTER_SERIALIZABLE(MetaBody,false);

#endif // METABODY_HPP

