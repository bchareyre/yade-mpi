/*************************************************************************
*  Copyright (C) 2009 by Vincent Richefeu				 *
*  Vincent.Richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"BasicViscoelasticRelationships.hpp"
#include<yade/pkg-dem/SimpleViscoelasticBodyParameters.hpp>
#include<yade/pkg-dem/ViscoelasticInteraction.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>


BasicViscoelasticRelationships::BasicViscoelasticRelationships()
{
}


void BasicViscoelasticRelationships::registerAttributes()
{
}


void BasicViscoelasticRelationships::go(  const shared_ptr<PhysicalParameters>& b1 // SimpleViscoelasticBodyParameters
					, const shared_ptr<PhysicalParameters>& b2 // SimpleViscoelasticBodyParameters
					, const shared_ptr<Interaction>& interaction)
{
    if(interaction->interactionPhysics) return;

    SimpleViscoelasticBodyParameters* sdec1 = static_cast<SimpleViscoelasticBodyParameters*>(b1.get());
    SimpleViscoelasticBodyParameters* sdec2 = static_cast<SimpleViscoelasticBodyParameters*>(b2.get());

    interaction->interactionPhysics = shared_ptr<ViscoelasticInteraction>(new ViscoelasticInteraction());
    ViscoelasticInteraction* contactPhysics = YADE_CAST<ViscoelasticInteraction*>(interaction->interactionPhysics.get());

    // Check that cn is dimensionless
    if (sdec1->cn >= 1.0) std::cout << "Warning: non dimensionless value for cn" << std::endl;

    // Arbitrare ponderation...
    contactPhysics->kn = 0.5 * (sdec1->kn + sdec2->kn);
    contactPhysics->ks = 0.5 * (sdec1->ks + sdec2->ks);
    contactPhysics->cn = 0.5 * (sdec1->cn + sdec2->cn);
    contactPhysics->cs = 0.0; // not viscosity in the tangent direction

    // in fact, dimensionless cn is a ponderation of the critical value cn_crit = 2sqrt(kn*meff)
    contactPhysics->cn *= 2.0 * sqrt(contactPhysics->kn * ((sdec1->mass * sdec2->mass) / (sdec1->mass + sdec2->mass)));
 
    contactPhysics->tangensOfFrictionAngle = std::tan(std::min(sdec1->frictionAngle, sdec2->frictionAngle)); 

    contactPhysics->shearForce = Vector3r(0,0,0);
    contactPhysics->prevNormal = Vector3r(0,0,0);

}

YADE_PLUGIN();
