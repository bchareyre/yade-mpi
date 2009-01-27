/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include <yade/pkg-common/RigidBodyParameters.hpp>

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
	    /// Coulomb friction
	    Real mu; 
	
		SimpleViscoelasticBodyParameters();
		virtual ~SimpleViscoelasticBodyParameters();

	    /// Set viscoelastic parameters kn,cn,ks,cs of the sphere from its
	    /// mass m, collision time tc and restitution coefficients en,es.
	    void setViscoelastic(Real m, Real tc, Real en, Real es);

	protected :
		/// Serialization
		void registerAttributes();
	REGISTER_CLASS_NAME(SimpleViscoelasticBodyParameters);
	REGISTER_BASE_CLASS_NAME(RigidBodyParameters);
	
/// Indexable	
	REGISTER_CLASS_INDEX(SimpleViscoelasticBodyParameters,RigidBodyParameters);
};

REGISTER_SERIALIZABLE(SimpleViscoelasticBodyParameters);


