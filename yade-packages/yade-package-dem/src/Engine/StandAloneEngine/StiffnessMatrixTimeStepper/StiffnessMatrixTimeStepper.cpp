/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "StiffnessMatrixTimeStepper.hpp"
#include "BodyMacroParameters.hpp"
#include "ElasticContactInteraction.hpp"
#include "SpheresContactGeometry.hpp"
#include "MacroMicroElasticRelationships.hpp"
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/Sphere.hpp>
#include <StiffnessMatrix.hpp>


StiffnessMatrixTimeStepper::StiffnessMatrixTimeStepper() : TimeStepper() , sdecContactModel(new MacroMicroElasticRelationships), actionParameterStiffnessMatrix(new StiffnessMatrix)
{
cerr << "StiffnessMatrixTimeStepper()"  << endl;
	stiffnessMatrixClassIndex = actionParameterStiffnessMatrix->getClassIndex();
	sdecGroupMask = 1;
	//computedOnce = false;
	MaxDt = 0.0003;
	
}


StiffnessMatrixTimeStepper::~StiffnessMatrixTimeStepper()
{

}


void StiffnessMatrixTimeStepper::registerAttributes()
{
	TimeStepper::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(MaxDt);
	//REGISTER_ATTRIBUTE(computedOnce);
}


void StiffnessMatrixTimeStepper::findTimeStepFromBody(const shared_ptr<Body>& body, MetaBody * ncb)
{
	BodyMacroParameters * sdec	= dynamic_cast<BodyMacroParameters*>(body->physicalParameters.get());
	Sphere* sphere 		= dynamic_cast<Sphere*>(body->geometricalModel.get());
	Vector3r& stiffness = (static_cast<StiffnessMatrix*>( ncb->physicalActions->find (body->getId(), stiffnessMatrixClassIndex).get()))->stiffness;

	//cerr << "Vector3r& stiffness = (static_cast<StiffnessMatrix*>( ncb" << endl;
	if(! (sphere && sdec && stiffness) )
		return; // not possible to compute!
	//cerr << "return; // not possible to compute!" << endl;
// 	Real Dab  	= sphere->radius;
// 	Real rad3 	= std::pow(Dab,2); // radius to the power of 2, from sphere
// 
// 	Real Eab 	= sdec->young;
// 	Real Vab 	= sdec->poisson;
// 	Real Dinit 	= 2*Dab; // assuming thet sphere is in contact with itself
// 	Real Sinit 	= Mathr::PI * std::pow( Dab , 2);
// 
// 	Real alpha 	= sdecContactModel->alpha;
// 	Real beta 	= sdecContactModel->beta;
// 	Real gamma 	= sdecContactModel->gamma;
// 
// 	Real Kn		= abs((Eab*Sinit/Dinit)*( (1+alpha)/(beta*(1+Vab) + gamma*(1-alpha*Dab) ) ));
// 	Real Ks		= abs(Kn*(1-alpha*Vab)/(1+Vab));

	Real dt = min (sqrt( sdec->mass  /  stiffness.x()) ,sqrt( sdec->mass  /  stiffness.y() ) );
	dt = 0.3 * min (sqrt( sdec->mass  /  stiffness.z()), dt);
	
	newDt = std::min(dt,newDt);
	computedSomething = true;
}


void StiffnessMatrixTimeStepper::findTimeStepFromInteraction(const shared_ptr<Interaction>& interaction, shared_ptr<BodyContainer>& bodies)
{
	unsigned int id1 = interaction->getId1();
	unsigned int id2 = interaction->getId2();
		
	if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask) )
		return; // skip other groups

	ElasticContactInteraction* sdecContact = dynamic_cast<ElasticContactInteraction*>(interaction->interactionPhysics.get());
	SpheresContactGeometry* interactionGeometry = dynamic_cast<SpheresContactGeometry*>(interaction->interactionGeometry.get());
	BodyMacroParameters * body1	= dynamic_cast<BodyMacroParameters*>((*bodies)[id1]->physicalParameters.get());
	BodyMacroParameters * body2	= dynamic_cast<BodyMacroParameters*>((*bodies)[id2]->physicalParameters.get());

	if(! (sdecContact && interactionGeometry && body1 && body2))
		return;
	
	Real mass 	= std::min( body1->mass                   , body2->mass               );
//	if(mass == 0) 			// FIXME - remove that comment: zero mass and zero inertia are too stupid to waste time checking that.
//		mass 	= std::max( body1->mass                   , body2->mass               );
//	if(mass == 0)
//		return; // not possible to compute
	Real inertia 	= std::min( body1->inertia[0]             , body2->inertia[0]         );
//	if( inertia == 0)
//		inertia = std::max( body1->inertia[0]             , body2->inertia[0]         );
//	if( inertia == 0)
//		return;
	Real rad3 	= std::pow(std::max(interactionGeometry->radius1 , interactionGeometry->radius2 ) , 2); // radius to the power of 2, from sphere

	Real dt = 0.1*min(
			  sqrt( mass     / abs(sdecContact->initialKn)      )
			, sqrt( inertia  / abs(sdecContact->initialKs*rad3) )
		  );

	newDt = std::min(dt,newDt);
	computedSomething = true;
}

bool StiffnessMatrixTimeStepper::isActivated()
{
	//cerr<< "StiffnessMatrixTimeStepper::isActivated() : " <<  Omega::instance().getCurrentIteration() << " active=" << active << endl;
	//bool result = (active && ((Omega::instance().getCurrentIteration() == 0) || (Omega::instance().getCurrentIteration() % timeStepUpdateInterval == 0)));
	//cerr << "result=" << result << endl;
	return (active && (((Omega::instance().getCurrentIteration() == 0) || (Omega::instance().getCurrentIteration() % timeStepUpdateInterval == 0)) || Omega::instance().getCurrentIteration() == 1)) ;
}


void StiffnessMatrixTimeStepper::computeTimeStep(Body* body)
{
//cerr << "computeTimeStep(Body* body)"  << endl;
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
// 	shared_ptr<InteractionContainer>& persistentInteractions = ncb->persistentInteractions;
// 	shared_ptr<InteractionContainer>& volatileInteractions = ncb->volatileInteractions;

	newDt = Mathr::MAX_REAL;
	Real defaultDt = 0.0003;
	
	computedSomething = false; // this flag is to avoid setting timestep to MAX_REAL :)
/*
	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
		findTimeStepFromInteraction(*ii , bodies);

	ii    = volatileInteractions->begin();
	iiEnd = volatileInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
		findTimeStepFromInteraction(*ii , bodies);*/
	
// no volatileInteractions at all? so let's try to estimate timestep by investigating bodies,
// simulating that a body in contact with itself. this happens only when there were not volatileInteractions at all.
	BodyContainer::iterator bi    = bodies->begin();
	BodyContainer::iterator biEnd = bodies->end();
	for(  ; bi!=biEnd ; ++bi )
	{
	//cerr << "for(  ; bi!=biEnd ; ++bi )" << endl;
		shared_ptr<Body> b = *bi;
		if (b->isDynamic) {
		//cerr << "if (body->isDynamic) {" << endl;
		if( b->getGroupMask() & sdecGroupMask)
		//cerr << "if( b->getGroupMask() & sdecGroupMask)" << computedSomething << endl;
			findTimeStepFromBody(b, ncb); }
	}
		
	if(computedSomething)
	{
		Omega::instance().setTimeStep(min(newDt , MaxDt));
		//computedOnce = true;		
		//cerr << "StiffnessMatrixTimeStepper, timestep chosen is:" << Omega::instance().getTimeStep() << endl;
	}	
	else Omega::instance().setTimeStep(MaxDt);
	//cerr << "MaxDt=" << MaxDt << endl;
//	cerr << "StiffnessMatrixTimeStepper, computedSomething is:" << computedSomething << endl;
	cerr << "StiffnessMatrixTimeStepper, newDt is:" << newDt << endl;
	cerr << "new timestep chosen is:" << Omega::instance().getTimeStep() << endl;
}

