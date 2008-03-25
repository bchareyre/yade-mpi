/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko                               *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SIMPLEVISCOELASTICINTERACTION_HPP 
#define SIMPLEVISCOELASTICINTERACTION_HPP

#include<yade/core/InteractionPhysics.hpp>

class SimpleViscoelasticInteraction : public InteractionPhysics
{
	public :
	    
	    /// Normal interaction elastic constants.
	    Real kn;
	    /// Normal interaction viscous constants.
	    Real cn;
	    /// Shear interaction elastic constants.
	    Real ks;
	    /// Shear interaction viscous constants.
	    Real cs;
	    /// Coulomb friction
	    Real mu;
	    /// Unit normal of the contact plane
	    Vector3r prevNormal;
	    /// Normal force
	    Vector3r normalForce;
	    /// Incremental shear force
	    Vector3r shearForce;
	
		SimpleViscoelasticInteraction();
		virtual ~SimpleViscoelasticInteraction();
	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(SimpleViscoelasticInteraction);
	REGISTER_BASE_CLASS_NAME(InteractionPhysics);

	REGISTER_CLASS_INDEX(SimpleViscoelasticInteraction,InteractionPhysics);

};

REGISTER_SERIALIZABLE(SimpleViscoelasticInteraction,false);

#endif // SIMPLEVISCOELASTICINTERACTION_HPP

