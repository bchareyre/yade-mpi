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


YADE_PLUGIN("Law2_Dem3DofGeom_RockPMPhys_Rpm");



/********************** Law2_Dem3DofGeom_RockPMPhys_Rpm ****************************/

void Law2_Dem3DofGeom_RockPMPhys_Rpm::go(shared_ptr<InteractionGeometry>& ig, shared_ptr<InteractionPhysics>& ip, Interaction* contact, MetaBody* rootBody){
	Dem3DofGeom* geom=static_cast<Dem3DofGeom*>(ig.get());
	ElasticContactInteraction* phys=static_cast<ElasticContactInteraction*>(ip.get());
	Real displN=geom->displacementN();
	if(displN>0){rootBody->interactions->requestErase(contact->getId1(),contact->getId2()); return; }
	phys->normalForce=phys->kn*displN*geom->normal;
	Real maxFsSq=phys->normalForce.SquaredLength()*pow(phys->tangensOfFrictionAngle,2);
	Vector3r trialFs=phys->ks*geom->displacementT();
	if(trialFs.SquaredLength()>maxFsSq){ geom->slipToDisplacementTMax(sqrt(maxFsSq)); trialFs*=sqrt(maxFsSq/(trialFs.SquaredLength()));} 
	applyForceAtContactPoint(phys->normalForce+trialFs,geom->contactPoint,contact->getId1(),geom->se31.position,contact->getId2(),geom->se32.position,rootBody);
}
