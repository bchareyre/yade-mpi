/*************************************************************************
*  Copyright (C) 2009 by Sergei Dorofeenko                               *
*  sega@users.berlios.de                                                 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ViscoelasticInteraction.hpp>
#include<yade/pkg-common/RigidBodyParameters.hpp>
YADE_PLUGIN("ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw");

void ef2_Spheres_Viscoelastic_SimpleViscoelasticContactLaw::go(shared_ptr<InteractionGeometry>& _geom, shared_ptr<InteractionPhysics>& _phys, Interaction* I, MetaBody* rootBody){

	SpheresContactGeometry* geom=static_cast<SpheresContactGeometry*>(_geom.get());
	ViscoelasticInteraction* phys=static_cast<ViscoelasticInteraction*>(_phys.get());

	int id1 = I->getId1();
	int id2 = I->getId2();
	
	shared_ptr<BodyContainer>& bodies = rootBody->bodies;

	RigidBodyParameters* de1 = YADE_CAST<RigidBodyParameters*>((*bodies)[id1]->physicalParameters.get());
	RigidBodyParameters* de2 = YADE_CAST<RigidBodyParameters*>((*bodies)[id2]->physicalParameters.get());

	Vector3r& shearForce 			= phys->shearForce;

	if (I->isFresh(rootBody)) shearForce=Vector3r(0,0,0);

	Real dt = Omega::instance().getTimeStep();

	Vector3r axis = phys->prevNormal.Cross(geom->normal);
	shearForce -= shearForce.Cross(axis);
	Real angle = dt*0.5*geom->normal.Dot(de1->angularVelocity + de2->angularVelocity);
	axis = angle*geom->normal;
	shearForce -= shearForce.Cross(axis);

	Vector3r x				= geom->contactPoint;
	Vector3r c1x				= (x - de1->se3.position);
	Vector3r c2x				= (x - de2->se3.position);
	 /// The following definition of c1x and c2x is to avoid "granular ratcheting" 
	///  (see F. ALONSO-MARROQUIN, R. GARCIA-ROJO, H.J. HERRMANN, 
	///   "Micro-mechanical investigation of granular ratcheting, in Cyclic Behaviour of Soils and Liquefaction Phenomena",
	///   ed. T. Triantafyllidis (Balklema, London, 2004), p. 3-10 - and a lot more papers from the same authors)
			//Vector3r _c1x_	=  geom->radius1*geom->normal;
			//Vector3r _c2x_	= -geom->radius2*geom->normal;
			//Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(_c2x_)) - (de1->velocity+de1->angularVelocity.Cross(_c1x_));
	Vector3r relativeVelocity		= (de2->velocity+de2->angularVelocity.Cross(c2x)) - (de1->velocity+de1->angularVelocity.Cross(c1x));
	Real     normalVelocity			= geom->normal.Dot(relativeVelocity);
	Vector3r shearVelocity			= relativeVelocity-normalVelocity*geom->normal;
	shearForce 			       -= (phys->ks*dt+phys->cs)*shearVelocity;

	phys->normalForce = ( phys->kn * std::max(geom->penetrationDepth,(Real) 0) - phys->cn * normalVelocity ) * geom->normal;
	phys->prevNormal = geom->normal;

	Real maxFs = phys->normalForce.SquaredLength() * std::pow(phys->tangensOfFrictionAngle,2);
	if( shearForce.SquaredLength() > maxFs )
	{
		maxFs = Mathr::Sqrt(maxFs) / shearForce.Length();
		shearForce *= maxFs;
	}

	Vector3r f				= phys->normalForce + shearForce;
	addForce (id1,-f,rootBody);
	addForce (id2, f,rootBody);
	addTorque(id1,-c1x.Cross(f),rootBody);
	addTorque(id2, c2x.Cross(f),rootBody);
}
