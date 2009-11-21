/*************************************************************************
*  Copyright (C) 2008 by Jerome Duriez                                   *
*  jerome.duriez@hmg.inpg.fr                                             *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg-common/RigidBodyParameters.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/lib-miniWm3/Wm3Math.h>

#include"CinemDNCEngine.hpp"


CinemDNCEngine::CinemDNCEngine()
{
	rotationAxis=Vector3r(0,0,1);
	shearSpeed=0;
	theta=0;
	thetalim=9.0/10.0*Mathr::PI/2.0;
	id_boxhaut=3;
}


void CinemDNCEngine::applyCondition(MetaBody * body)
{
	if(theta<=thetalim)
	{
		applyRotTranslation(body);	// to let move the lateral walls
		applyTranslation(body);		// to let move the upper wall
	}
	
}



void CinemDNCEngine::applyRotTranslation(MetaBody * ncb)
{
	shared_ptr<BodyContainer> bodies = ncb->bodies;

	Yplaqsup=((*bodies)[id_boxhaut]->physicalParameters.get())->se3.position.Y();	// the height of the sample, which may be different from the initial "height" defined in the Preprocessor
	
	subscribedBodies.clear();
	subscribedBodies.push_back(0);	// 0 and 2 are in my case the ids of the lateral walls
	subscribedBodies.push_back(2);
	std::vector<int>::const_iterator ii = subscribedBodies.begin();
	std::vector<int>::const_iterator iiEnd = subscribedBodies.end();

	Real dt = Omega::instance().getTimeStep();
	Real dx = shearSpeed*dt;
	Real dtheta;
	dtheta=Mathr::ATan(dx/(Yplaqsup+dx*Mathr::Tan(theta)+Yplaqsup*Mathr::Pow(Mathr::Tan(theta),2)));
	Quaternionr q;
	q.FromAxisAngle(rotationAxis,-dtheta);

	for(;ii!=iiEnd;++ii)
	{
		RigidBodyParameters * rb = dynamic_cast<RigidBodyParameters*>((*bodies)[*ii]->physicalParameters.get());

// La partie de rotation :
		rb->se3.orientation	= q*rb->se3.orientation;
		rb->se3.orientation.Normalize();
/*		rb->angularVelocity	= rotationAxis*angularVelocity;*/
		rb->velocity		= Vector3r(0,0,0);
// La partie de translation :
		Real YcentreW1;		//the altitude of the center of both lateral walls
		YcentreW1=((*bodies)[0]->physicalParameters.get())->se3.position.Y();
		rb->se3.position+=dt*(shearSpeed*YcentreW1/Yplaqsup)*Vector3r(1,0,0);
	}
	theta+=dtheta;
}


void CinemDNCEngine::applyTranslation(MetaBody * ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	int id_boxhaut=3;
	Real dt = Omega::instance().getTimeStep();

	if(ParticleParameters* p = dynamic_cast<ParticleParameters*>((*bodies)[id_boxhaut]->physicalParameters.get()))
	{
		p->se3.position		+= dt*shearSpeed*Vector3r(1,0,0);	// by def of shearSpeed
		p->velocity		=  shearSpeed*Vector3r(1,0,0);
	}
	else if(PhysicalParameters* b = dynamic_cast<PhysicalParameters*>((*bodies)[id_boxhaut]->physicalParameters.get()))
	{ // NOT everyone has velocity !
		b->se3.position		+= dt*shearSpeed*Vector3r(1,0,0);
	}
	else
	{
		std::cerr << "TranslationEngine::applyCondition, WARNING! dynamic_cast failed! for id: " << id_boxhaut << std::endl;
	}

}




YADE_REQUIRE_FEATURE(PHYSPAR);

