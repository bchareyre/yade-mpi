/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "GlobalStiffnessTimeStepper.hpp"
#include "BodyMacroParameters.hpp"
#include "ElasticContactInteraction.hpp"
#include "SpheresContactGeometry.hpp"
#include "MacroMicroElasticRelationships.hpp"
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/Sphere.hpp>
#include <GlobalStiffness.hpp>


GlobalStiffnessTimeStepper::GlobalStiffnessTimeStepper() : TimeStepper() , sdecContactModel(new MacroMicroElasticRelationships), actionParameterGlobalStiffness(new GlobalStiffness)
{
cerr << "GlobalStiffnessTimeStepper()"  << endl;
	globalStiffnessClassIndex = actionParameterGlobalStiffness->getClassIndex();
	sdecGroupMask = 1;
	timestepSafetyCoefficient = 0.8;
	computedOnce = false;
	defaultDt = 1;
	
}


GlobalStiffnessTimeStepper::~GlobalStiffnessTimeStepper()
{

}


void GlobalStiffnessTimeStepper::registerAttributes()
{
	TimeStepper::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(defaultDt);
}


void GlobalStiffnessTimeStepper::findTimeStepFromBody(const shared_ptr<Body>& body, MetaBody * ncb)
{
	RigidBodyParameters * sdec	= static_cast<RigidBodyParameters*>(body->physicalParameters.get());
	
	Sphere* sphere = static_cast<Sphere*>(body->geometricalModel.get());
	
	Vector3r& stiffness = (static_cast<GlobalStiffness*>( ncb->physicalActions->find (body->getId(), globalStiffnessClassIndex).get()))->stiffness;
	Vector3r& Rstiffness = (static_cast<GlobalStiffness*>( ncb->physicalActions->find (body->getId(), globalStiffnessClassIndex).get()))->Rstiffness;


	//cerr << "Vector3r& stiffness = (static_cast<GlobalStiffness*>( ncb" << endl;
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

	
// 	Real dt = min (sqrt( sdec->mass  /  stiffness.x()) ,sqrt( sdec->mass  /  stiffness.y() ) );
// 	dt = 0.5 * min (sqrt( sdec->mass  /  stiffness.z()), dt);
	
	Real dt = max( max (stiffness.X(), stiffness.Y()), stiffness.Z() );
	dt = (dt!=0 ? sqrt(sdec->mass/dt) : Mathr::MAX_REAL);	
		
	Real dtx = (Rstiffness.X()!=0 ? sdec->inertia.X()/Rstiffness.X() : Mathr::MAX_REAL);
	Real dty = (Rstiffness.Y()!=0 ? sdec->inertia.Y()/Rstiffness.Y() : Mathr::MAX_REAL);
	Real dtz = (Rstiffness.Z()!=0 ? sdec->inertia.Z()/Rstiffness.Z() : Mathr::MAX_REAL);
	Real Rdt = sqrt( min( min (dtx, dty), dtz ) );
	
	//cerr << "Rstiffness.x()=" << Rstiffness.x() << "  " << Rstiffness.y() << "  " << Rstiffness.z() << endl;
	//cerr << "sdec->inertia=" << sdec->inertia.x() << " " << sdec->inertia.x() << " " << sdec->inertia.x() << endl;
	//cerr << "timesteps : dt=" << dt << " / Rdt=" << Rdt << endl;
	
	dt = timestepSafetyCoefficient*std::min(dt,Rdt);
	
	newDt = std::min(dt,newDt);
	computedSomething = true;
	
}


void GlobalStiffnessTimeStepper::findTimeStepFromInteraction(const shared_ptr<Interaction>& interaction, shared_ptr<BodyContainer>& bodies)
{


}

bool GlobalStiffnessTimeStepper::isActivated()
{
	return (active && (!computedOnce || (Omega::instance().getCurrentIteration() % timeStepUpdateInterval == 0)));
}


void GlobalStiffnessTimeStepper::computeTimeStep(Body* body)
{
//cerr << "computeTimeStep(Body* body)"  << endl;
cerr << "phasea1 "; 
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
// 	shared_ptr<InteractionContainer>& persistentInteractions = ncb->persistentInteractions;
// 	shared_ptr<InteractionContainer>& transientInteractions = ncb->transientInteractions;

	newDt = Mathr::MAX_REAL;
	//Real defaultDt = 0.0003;
	
	computedSomething = false; // this flag is to avoid setting timestep to MAX_REAL :)
/*
	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
		findTimeStepFromInteraction(*ii , bodies);

	ii    = transientInteractions->begin();
	iiEnd = transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
		findTimeStepFromInteraction(*ii , bodies);*/

	if(!computedSomething)
	{
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
	}	
	
	cerr << "phasea9 "; 
	
	if(computedSomething)
	{
		Omega::instance().setTimeStep(min(newDt , defaultDt));
		computedOnce = true;		
		//cerr << "GlobalStiffnessTimeStepper, timestep chosen is:" << Omega::instance().getTimeStep() << endl;
	}
//	cerr << "GlobalStiffnessTimeStepper, computedSomething is:" << computedSomething << endl;
	cerr << "GlobalGlobalStiffnessTimeStepper, newDt is:" << newDt << endl;
	cerr << "new timestep chosen is:" << Omega::instance().getTimeStep() << endl;
}

