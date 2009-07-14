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
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/DemXDofGeom.hpp>
#include<yade/extra/Shop.hpp>


YADE_PLUGIN("Law2_Dem3DofGeom_RockPMPhys_Rpm", "RpmMat","Ip2_RpmMat_RpmMat_RpmPhys","RpmPhys");



/********************** Law2_Dem3DofGeom_RockPMPhys_Rpm ****************************/
CREATE_LOGGER(Law2_Dem3DofGeom_RockPMPhys_Rpm);

void Law2_Dem3DofGeom_RockPMPhys_Rpm::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, MetaBody* rootBody){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	RpmPhys* phys=static_cast<RpmPhys*>(ip.get());

	Real displN=geom->displacementN();	
	const shared_ptr<Body>& body1=Body::byId(contact->getId1(),rootBody);
	const shared_ptr<Body>& body2=Body::byId(contact->getId2(),rootBody);
	assert(body1);
	assert(body2);
	const shared_ptr<RpmMat>& rbp1=YADE_PTR_CAST<RpmMat>(body1->physicalParameters);
	const shared_ptr<RpmMat>& rbp2=YADE_PTR_CAST<RpmMat>(body2->physicalParameters);

	//check, whether one of bodies is damaged
	if ((rbp1->isDamaged) || (rbp2->isDamaged)) {
		phys->isCohesive = false;
	}
				
				
	if(displN<=0){
		/*Normal Interaction*/
		phys->normalForce=phys->kn*displN*geom->normal;
		Real maxFsSq=phys->normalForce.SquaredLength()*pow(phys->tanFrictionAngle,2);
		Vector3r trialFs=phys->ks*geom->displacementT();
		if(trialFs.SquaredLength()>maxFsSq){ geom->slipToDisplacementTMax(sqrt(maxFsSq)); trialFs*=sqrt(maxFsSq/(trialFs.SquaredLength()));} 
		applyForceAtContactPoint(phys->normalForce+trialFs,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,rootBody);
		/*Normal Interaction_____*/
		
		if ((phys->isCohesive)&&(displN<(-phys->lengthMaxCompression))) {
			//LOG_WARN(displN<<"__COMRESS!!!__");
			phys->isCohesive = false;
			rbp1->isDamaged=true;
			rbp2->isDamaged=true;
		}
	} else {
		if (phys->isCohesive) {
			phys->normalForce=phys->kn*displN*geom->normal;
			if (displN>(phys->lengthMaxTension)) {
				//LOG_WARN(displN<<"__TENSION!!!__");
				phys->isCohesive = false;
				rbp1->isDamaged=true;
				rbp2->isDamaged=true;
			} else {
				applyForceAtContactPoint(phys->normalForce,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,rootBody);
			}
			return;
		} else {
			rootBody->interactions->requestErase(contact->getId1(),contact->getId2());
			return;
		}
	}
	
}


CREATE_LOGGER(Ip2_RpmMat_RpmMat_RpmPhys);

void Ip2_RpmMat_RpmMat_RpmPhys::go(const shared_ptr<PhysicalParameters>& pp1, const shared_ptr<PhysicalParameters>& pp2, const shared_ptr<Interaction>& interaction){
	if(interaction->interactionPhysics) return; 

	Dem3DofGeom* contGeom=YADE_CAST<Dem3DofGeom*>(interaction->interactionGeometry.get());
	
	assert(contGeom);
	
	const shared_ptr<RpmMat>& rpm1=YADE_PTR_CAST<RpmMat>(pp1);
	const shared_ptr<RpmMat>& rpm2=YADE_PTR_CAST<RpmMat>(pp2);
	
	const shared_ptr<BodyMacroParameters>& elast1=static_pointer_cast<BodyMacroParameters>(pp1);
	const shared_ptr<BodyMacroParameters>& elast2=static_pointer_cast<BodyMacroParameters>(pp2);

	bool initCohesive = rpm1->initCohesive*rpm2->initCohesive;
	
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
	
	contPhys->lengthMaxCompression=S12*(0.5*(rpm1->stressCompressMax+rpm2->stressCompressMax))/(contPhys->kn);
	contPhys->lengthMaxTension=S12*(0.5*(rpm1->stressTensionMax+rpm2->stressTensionMax))/(contPhys->kn);
	
	initDistance = contGeom->displacementN();

	if ((rpm1->exampleNumber==rpm2->exampleNumber)&&(initDistance<(contPhys->lengthMaxTension))&&(initCohesive)) {
		contPhys->isCohesive=true;
		//LOG_WARN("InitDistance="<<initDistance);
		//LOG_WARN("lengthMaxCompression="<<contPhys->lengthMaxCompression);
		//LOG_WARN("lengthMaxTension="<<contPhys->lengthMaxTension);
	}

	
	interaction->interactionPhysics=contPhys;
}

RpmPhys::~RpmPhys(){};

