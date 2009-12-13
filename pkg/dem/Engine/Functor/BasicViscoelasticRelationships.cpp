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
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/Scene.hpp>
#include<yade/core/GroupRelationData.hpp>


BasicViscoelasticRelationships::BasicViscoelasticRelationships()
{
}




void BasicViscoelasticRelationships::go(  const shared_ptr<PhysicalParameters>& b1 // SimpleViscoelasticBodyParameters
					, const shared_ptr<PhysicalParameters>& b2 // SimpleViscoelasticBodyParameters
					, const shared_ptr<Interaction>& interaction)
{
    if(interaction->interactionPhysics) return;

    shared_ptr<GroupRelationData> data = (Omega::instance().getScene().get())->grpRelationData;

    interaction->interactionPhysics = shared_ptr<ViscoelasticInteraction>(new ViscoelasticInteraction());
    ViscoelasticInteraction* contactPhysics = YADE_CAST<ViscoelasticInteraction*>(interaction->interactionPhysics.get());

    shared_ptr<Body> bdy1 = (*((Omega::instance().getScene().get())->bodies))[ interaction->getId1() ];
    shared_ptr<Body> bdy2 = (*((Omega::instance().getScene().get())->bodies))[ interaction->getId2() ];

    if (data->isActivated())
    {
        int msk1 = bdy1->groupMask;
        int msk2 = bdy2->groupMask;
        contactPhysics->kn = (data->exists("kn")) ? data->getParameter("kn",msk1,msk2) : 0.0;
        contactPhysics->ks = (data->exists("ks")) ? data->getParameter("ks",msk1,msk2) : 0.0;
        contactPhysics->cn = (data->exists("cn")) ? data->getParameter("cn",msk1,msk2) : 0.0;
        contactPhysics->cs = (data->exists("cs")) ? data->getParameter("cs",msk1,msk2) : 0.0;
        contactPhysics->tangensOfFrictionAngle = (data->exists("mu")) ? data->getParameter("mu",msk1,msk2) : 0.0;
    }
    else
    {
        SimpleViscoelasticBodyParameters* sdec1 = static_cast<SimpleViscoelasticBodyParameters*>(b1.get());
        SimpleViscoelasticBodyParameters* sdec2 = static_cast<SimpleViscoelasticBodyParameters*>(b2.get());

        // Check that cn is dimensionless
        if (sdec1->cn >= 1.0 || sdec2->cn >= 1.0) std::cout << "Warning: non dimensionless value for cn" << std::endl;
    
        // Arbitrary ponderations
        contactPhysics->kn = 0.5 * (sdec1->kn + sdec2->kn);
        contactPhysics->ks = 0.5 * (sdec1->ks + sdec2->ks);
        contactPhysics->cn = 0.5 * (sdec1->cn + sdec2->cn);
        contactPhysics->cs = 0.0; // not viscosity in the tangent direction
        contactPhysics->tangensOfFrictionAngle = std::tan(std::min(sdec1->frictionAngle, sdec2->frictionAngle)); 
    }

    // in fact, at this stage cn is a ponderation of the critical value cn_crit = 2sqrt(kn*meff)
    double m1,m2;
    
    if (bdy1->isClumpMember())
    {
        const shared_ptr<Body>& clump = (*((Omega::instance().getScene().get())->bodies))[ bdy1->clumpId ];
        RigidBodyParameters* clumpRBP=YADE_CAST<RigidBodyParameters*> ( clump->physicalParameters.get() );
        m1 = clumpRBP->mass;
        if (!clump->isDynamic) m1 *= 1.0e6;
    }
    else
    {  
        ParticleParameters* pp = YADE_CAST<ParticleParameters*> ( bdy1->physicalParameters.get() );
        m1 = pp->mass ; 
        if (!bdy1->isDynamic) m1 *= 1.0e6;
    }
    
    if (bdy2->isClumpMember())
    {
        const shared_ptr<Body>& clump = (*((Omega::instance().getScene().get())->bodies))[ bdy2->clumpId ];
        RigidBodyParameters* clumpRBP=YADE_CAST<RigidBodyParameters*> ( clump->physicalParameters.get() );
        m2 = clumpRBP->mass;
        if (!clump->isDynamic) m2 *= 1.0e6;
    }
    else
    { 
        ParticleParameters* pp = YADE_CAST<ParticleParameters*> ( bdy2->physicalParameters.get() );
        m2 = pp->mass; 
        if (!bdy2->isDynamic) m2 *= 1.0e6;
    }

    contactPhysics->cn *= 2.0 * sqrt(contactPhysics->kn * ((m1 * m2) / (m1 + m2)));
 
    contactPhysics->shearForce = Vector3r(0,0,0);
    contactPhysics->prevNormal = Vector3r(0,0,0);
}

YADE_PLUGIN((BasicViscoelasticRelationships));

YADE_REQUIRE_FEATURE(PHYSPAR);

