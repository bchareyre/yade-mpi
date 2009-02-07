/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/pkg-common/RigidBodyParameters.hpp>

/// Note: Shop::getViscoelasticFromSpheresInteraction can get kn,cn,ks,cs from a analytical solution of a pair spheres interaction problem.
class SimpleViscoelasticBodyParameters : public RigidBodyParameters {	
	public :

	    /// Normal elastic constants of the body
	    Real kn; 
	    /// Normal viscous constants of the body
	    Real cn; 
	    /// Shear elastic constants of the body
	    Real ks; 
	    /// Shear viscous constants of the body
	    Real cs; 
	    /// Friction angle
	    Real frictionAngle; 
	
		SimpleViscoelasticBodyParameters();
		virtual ~SimpleViscoelasticBodyParameters();

	protected :
		/// Serialization
		void registerAttributes();
	REGISTER_CLASS_NAME(SimpleViscoelasticBodyParameters);
	REGISTER_BASE_CLASS_NAME(RigidBodyParameters);
	
/// Indexable	
	REGISTER_CLASS_INDEX(SimpleViscoelasticBodyParameters,RigidBodyParameters);
};

REGISTER_SERIALIZABLE(SimpleViscoelasticBodyParameters);


