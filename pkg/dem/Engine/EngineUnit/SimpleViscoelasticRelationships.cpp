/*************************************************************************
*  Copyright (C) 2008 by Dorofeenko Sergei				 *
*  sega@users.berlios.de						 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"SimpleViscoelasticRelationships.hpp"
#include<yade/pkg-dem/SimpleViscoelasticBodyParameters.hpp>
#include<yade/pkg-dem/ViscoelasticInteraction.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>


SimpleViscoelasticRelationships::SimpleViscoelasticRelationships()
{
}


void SimpleViscoelasticRelationships::registerAttributes()
{
}


void SimpleViscoelasticRelationships::go(	  const shared_ptr<PhysicalParameters>& b1 // SimpleViscoelasticBodyParameters
					, const shared_ptr<PhysicalParameters>& b2 // SimpleViscoelasticBodyParameters
					, const shared_ptr<Interaction>& interaction)
{
    if( !interaction->isNew ) return;

    SimpleViscoelasticBodyParameters* sdec1 = static_cast<SimpleViscoelasticBodyParameters*>(b1.get());
    SimpleViscoelasticBodyParameters* sdec2 = static_cast<SimpleViscoelasticBodyParameters*>(b2.get());

    interaction->interactionPhysics = shared_ptr<ViscoelasticInteraction>(new ViscoelasticInteraction());
    ViscoelasticInteraction* contactPhysics = YADE_CAST<ViscoelasticInteraction*>(interaction->interactionPhysics.get());

    contactPhysics->kn = sdec1->kn * sdec2->kn / (sdec1->kn + sdec2->kn);
    contactPhysics->ks = sdec1->ks * sdec2->ks / (sdec1->ks + sdec2->ks);
    
    contactPhysics->cn = ( (sdec1->cn==0) ? 0 : 1/sdec1->cn ) + ( (sdec2->cn==0) ? 0 : 1/sdec2->cn );
    contactPhysics->cs = ( (sdec1->cs==0) ? 0 : 1/sdec1->cs ) + ( (sdec2->cs==0) ? 0 : 1/sdec2->cs );
    if (contactPhysics->cn) contactPhysics->cn = 1/contactPhysics->cn;
    if (contactPhysics->cs) contactPhysics->cs = 1/contactPhysics->cs;
   
    contactPhysics->tangensOfFrictionAngle		= std::tan(std::min(sdec1->frictionAngle, sdec2->frictionAngle)); 

    contactPhysics->shearForce = Vector3r(0,0,0);
    contactPhysics->prevNormal = Vector3r(0,0,0);

}

YADE_PLUGIN();
