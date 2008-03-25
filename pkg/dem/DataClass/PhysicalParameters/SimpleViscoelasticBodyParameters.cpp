/*************************************************************************
*  Copyright (C) 2008 by Sergei Dorofeenko				 *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "SimpleViscoelasticBodyParameters.hpp"

SimpleViscoelasticBodyParameters::SimpleViscoelasticBodyParameters() : RigidBodyParameters()
{
	createIndex();
}

SimpleViscoelasticBodyParameters::~SimpleViscoelasticBodyParameters()
{
}

void SimpleViscoelasticBodyParameters::registerAttributes()
{
	RigidBodyParameters::registerAttributes();
	REGISTER_ATTRIBUTE(kn);
	REGISTER_ATTRIBUTE(ks);
	REGISTER_ATTRIBUTE(cn);
	REGISTER_ATTRIBUTE(cs);
	REGISTER_ATTRIBUTE(mu);
}

void SimpleViscoelasticBodyParameters::setViscoelastic(Real m, Real tc, Real en, Real es)
{
    kn = m/tc/tc * ( Mathr::PI*Mathr::PI + Mathr::Pow(Mathr::Log(en),2) );
    cn = -2.0*m/tc * Mathr::Log(en);
    ks = 2.0/7.0 * m/tc/tc * ( Mathr::PI*Mathr::PI + Mathr::Pow(Mathr::Log(es),2) );
    cs = -2.0/7.0 * m/tc * Mathr::Log(es);

    if (Math<Real>::FAbs(cn) <= Math<Real>::ZERO_TOLERANCE ) cn=0;
    if (Math<Real>::FAbs(cs) <= Math<Real>::ZERO_TOLERANCE ) cs=0;
}
YADE_PLUGIN();

