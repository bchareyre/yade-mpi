/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"GlobalStiffnessTimeStepper.hpp"
#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/MacroMicroElasticRelationships.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-dem/GlobalStiffness.hpp>

CREATE_LOGGER(GlobalStiffnessTimeStepper);

GlobalStiffnessTimeStepper::GlobalStiffnessTimeStepper() : TimeStepper() , sdecContactModel(new MacroMicroElasticRelationships), actionParameterGlobalStiffness(new GlobalStiffness)
{
//cerr << "GlobalStiffnessTimeStepper()"  << endl;
	globalStiffnessClassIndex = actionParameterGlobalStiffness->getClassIndex();
	sdecGroupMask = 1;
	timestepSafetyCoefficient = 0.8;
	computedOnce = false;
	defaultDt = 1;
	previousDt = defaultDt;
	
}


GlobalStiffnessTimeStepper::~GlobalStiffnessTimeStepper()
{

}


void GlobalStiffnessTimeStepper::registerAttributes()
{
	TimeStepper::registerAttributes();
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(defaultDt);
	REGISTER_ATTRIBUTE(previousDt);
	REGISTER_ATTRIBUTE(timestepSafetyCoefficient);
	REGISTER_ATTRIBUTE(computedOnce);
}


void GlobalStiffnessTimeStepper::findTimeStepFromBody(const shared_ptr<Body>& body, MetaBody * ncb)
{
	RigidBodyParameters * sdec	= static_cast<RigidBodyParameters*>(body->physicalParameters.get());
	
	// Sphere* sphere = static_cast<Sphere*>(body->geometricalModel.get());
	
	Vector3r& stiffness = (static_cast<GlobalStiffness*>( ncb->physicalActions->find (body->getId(), globalStiffnessClassIndex).get()))->stiffness;
	Vector3r& Rstiffness = (static_cast<GlobalStiffness*>( ncb->physicalActions->find (body->getId(), globalStiffnessClassIndex).get()))->Rstiffness;


	//cerr << "Vector3r& stiffness = (static_cast<GlobalStiffness*>( ncb" << endl;
	if(! ( /* sphere && */ sdec && stiffness) )
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
	Real dtx, dty, dtz;	

	Real dt = max( max (stiffness.X(), stiffness.Y()), stiffness.Z() );
	if (dt!=0) {
		dt = sdec->mass/dt;  computedSomething = true;}//dt = squared eigenperiod of translational motion 
	else dt = Mathr::MAX_REAL;
	
	if (Rstiffness.X()!=0) {
		dtx = sdec->inertia.X()/Rstiffness.X();  computedSomething = true;}//dtx = squared eigenperiod of rotational motion around x
	else dtx = Mathr::MAX_REAL;

	if (Rstiffness.Y()!=0) {
		dty = sdec->inertia.Y()/Rstiffness.Y();  computedSomething = true;}
	else dty = Mathr::MAX_REAL;

	if (Rstiffness.Z()!=0) {
		dtz = sdec->inertia.Z()/Rstiffness.Z();  computedSomething = true;}
	else dtz = Mathr::MAX_REAL;


	//if (Rstiffness.X()!=0) 
	//Real dtx = (Rstiffness.X()!=0 ? sdec->inertia.X()/Rstiffness.X() : Mathr::MAX_REAL);
	//Real dty = (Rstiffness.Y()!=0 ? sdec->inertia.Y()/Rstiffness.Y() : Mathr::MAX_REAL);
	//Real dtz = (Rstiffness.Z()!=0 ? sdec->inertia.Z()/Rstiffness.Z() : Mathr::MAX_REAL);
	Real Rdt =  std::min( std::min (dtx, dty), dtz );//Rdt = smallest squared eigenperiod for rotational motions
	
	//cerr << "Rstiffness.x()=" << Rstiffness.x() << "  " << Rstiffness.y() << "  " << Rstiffness.z() << endl;
	//cerr << "sdec->inertia=" << sdec->inertia.x() << " " << sdec->inertia.x() << " " << sdec->inertia.x() << endl;
	//cerr << "timesteps : dt=" << dt << " / Rdt=" << Rdt << endl;
	
	dt = 1.41044*timestepSafetyCoefficient*std::sqrt(std::min(dt,Rdt));//1.41044 = sqrt(2)
	
	newDt = std::min(dt,newDt);
	//computedSomething = true;
	
}


void GlobalStiffnessTimeStepper::findTimeStepFromInteraction(const shared_ptr<Interaction>& interaction, shared_ptr<BodyContainer>& bodies)
{


}

bool GlobalStiffnessTimeStepper::isActivated()
{
	return (active && ((!computedOnce) || (Omega::instance().getCurrentIteration() % timeStepUpdateInterval == 0) || (Omega::instance().getCurrentIteration() < (long int) 2) ));
}


void GlobalStiffnessTimeStepper::computeTimeStep(MetaBody* ncb)
{

	shared_ptr<BodyContainer>& bodies = ncb->bodies;
// 	shared_ptr<InteractionContainer>& persistentInteractions = ncb->persistentInteractions;
// 	shared_ptr<InteractionContainer>& transientInteractions = ncb->transientInteractions;

	newDt = Mathr::MAX_REAL;
	//Real defaultDt = 0.0003;
	
	//computedSomething = false; // this flag is to avoid setting timestep to MAX_REAL :)
/*
	InteractionContainer::iterator ii    = persistentInteractions->begin();
	InteractionContainer::iterator iiEnd = persistentInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
		findTimeStepFromInteraction(*ii , bodies);

	ii    = transientInteractions->begin();
	iiEnd = transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
		findTimeStepFromInteraction(*ii , bodies);*/

	//if(!computedSomething)
	//{
		BodyContainer::iterator bi    = bodies->begin();
		BodyContainer::iterator biEnd = bodies->end();
		for(  ; bi!=biEnd ; ++bi )
		{
		//cerr << "for(  ; bi!=biEnd ; ++bi )" << endl;
			shared_ptr<Body> b = *bi;
			if (b->isDynamic) {
			//cerr << "if (body->isDynamic) {" << endl;
			if((b->getGroupMask() & sdecGroupMask)||sdecGroupMask==0)
			//cerr << "if( b->getGroupMask() & sdecGroupMask)" << computedSomething << endl;
				findTimeStepFromBody(b, ncb); }
		}
	//}	
		
	if(computedSomething)
	{
		previousDt = min ( min(newDt , defaultDt), 1.5*previousDt );// at maximum, dt will be multiplied by 1.5 in one iterration, this is to prevent brutal switches from 0.000... to 1 in some computations 
		Omega::instance().setTimeStep(previousDt);
		//previousDt = Omega::instance().getTimeStep();
		//Omega::instance().setTimeStep(newDt);
		computedOnce = true;	
		//cerr << "computedOnce=" << computedOnce << endl;	
		//cerr << "computed timestep is:" << newDt;
	}
	else if (!computedOnce) Omega::instance().setTimeStep(defaultDt);
	//cerr << " new timestep is:" << Omega::instance().getTimeStep() << endl;

	LOG_INFO("computed timestep " << newDt <<
			(Omega::instance().getTimeStep()==newDt ? string(", appplied") :
			string(", BUT timestep is ")+lexical_cast<string>(Omega::instance().getTimeStep()))<<".");
}

YADE_PLUGIN();
