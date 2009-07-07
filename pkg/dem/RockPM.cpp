/*************************************************************************
*    Copyright (C) 2009 Anton Gladkyy gladky.anton@gmail.com             *
*                                                                        *
*    This program is free software: you can redistribute it and/or modify*
*    it under the terms of the GNU General Public License as published by*
*    the Free Software Foundation, either version 3 of the License, or   *
*    (at your option) any later version.                                 *
*                                                                        *
*    This program is distributed in the hope that it will be useful,     *
*    but WITHOUT ANY WARRANTY; without even the implied warranty of      *
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the       *
*    GNU General Public License for more details.                        *
*                                                                        *
*    You should have received a copy of the GNU General Public License   *
*    along with this program.  If not, see <http://www.gnu.org/licenses/>*
**************************************************************************/

#include"RockPM.hpp"
#include"yade/pkg-dem/ElasticContactLaw.hpp"
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>


YADE_PLUGIN("Law2_Dem3DofGeom_RockPMPhys_Rpm", "RpmMat","Ip2_RpmMat_RpmMat_RpmPhys","RpmPhys");



/********************** Law2_Dem3DofGeom_RockPMPhys_Rpm ****************************/

void Law2_Dem3DofGeom_RockPMPhys_Rpm::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, MetaBody* rootBody){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	
	RpmPhys* phys=static_cast<RpmPhys*>(ip.get());
	
	Real displN=geom->displacementN();
	
	if(displN>0){rootBody->interactions->requestErase(contact->getId1(),contact->getId2()); return; }
	
	phys->normalForce=phys->kn*displN*geom->normal;
	
	Real maxFsSq=phys->normalForce.SquaredLength()*pow(phys->tangensOfFrictionAngle,2);
	
	Vector3r trialFs=phys->ks*geom->displacementT();
	
	if(trialFs.SquaredLength()>maxFsSq){ geom->slipToDisplacementTMax(sqrt(maxFsSq)); trialFs*=sqrt(maxFsSq/(trialFs.SquaredLength()));} 
	
	applyForceAtContactPoint(phys->normalForce+trialFs,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,rootBody);
	
}

/*
void Law2_Dem3DofGeom_SimplePhys_Simple::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, MetaBody* rootBody){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	
	RpmPhys* phys=static_cast<RpmPhys*>(ip.get());
	
	Real displN=geom->displacementN();
	
	if(displN>0){rootBody->interactions->requestErase(contact->getId1(),contact->getId2()); return; }
	
	phys->normalForce=phys->kn*displN*geom->normal;
	
	Real maxFsSq=phys->normalForce.SquaredLength()*pow(phys->tangensOfFrictionAngle,2);
	
	Vector3r trialFs=phys->ks*geom->displacementT();
	
	if(trialFs.SquaredLength()>maxFsSq){ geom->slipToDisplacementTMax(sqrt(maxFsSq)); trialFs*=sqrt(maxFsSq/(trialFs.SquaredLength()));} 
	
	applyForceAtContactPoint(phys->normalForce+trialFs,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,rootBody);
	
}
*/


CREATE_LOGGER(Ip2_RpmMat_RpmMat_RpmPhys);

void Ip2_RpmMat_RpmMat_RpmPhys::go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction){
	if(interaction->interactionPhysics) return; 

	Dem3DofGeom* contGeom=YADE_CAST<Dem3DofGeom*>(interaction->interactionGeometry.get());
	assert(contGeom);

	const shared_ptr<BodyMacroParameters>& elast1=static_pointer_cast<BodyMacroParameters>(pp1);
	const shared_ptr<BodyMacroParameters>& elast2=static_pointer_cast<BodyMacroParameters>(pp2);

	Real E12=2*elast1->young*elast2->young/(elast1->young+elast2->young);
	Real minRad=(contGeom->refR1<=0?contGeom->refR2:(contGeom->refR2<=0?contGeom->refR1:min(contGeom->refR1,contGeom->refR2)));
	Real S12=Mathr::PI*pow(minRad,2);
	shared_ptr<RpmPhys> contPhys(new RpmPhys());
	contPhys->E=E12;
	contPhys->G=E12;
	contPhys->tanFrictionAngle=tan(.5*(elast1->frictionAngle+elast2->frictionAngle));
	contPhys->crossSection=S12;
	contPhys->kn=contPhys->E*contPhys->crossSection;
	contPhys->ks=contPhys->G*contPhys->crossSection;
	interaction->interactionPhysics=contPhys;
}

RpmPhys::~RpmPhys(){};

