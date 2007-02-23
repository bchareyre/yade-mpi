/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TriaxialCompressionEngine.hpp"
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-package-common/Force.hpp>
#include "ElasticContactInteraction.hpp"
#include <yade/yade-lib-base/yadeWm3Extra.hpp>
#include <boost/lexical_cast.hpp>


TriaxialCompressionEngine::TriaxialCompressionEngine() : actionForce(new Force)
{
	translationAxis=TriaxialStressController::normal[wall_bottom_id];
	strainRate=0;
	currentStrainRate=0;
	StabilityCriterion=0.001;
	Phase1=false;
	Phase1End = "Phase1End";
	FinalIterationPhase1 = 0;
// 	Phase2End = "Phase2End";
	compressionActivated=false;
	autoCompressionActivation=true;
	for (int i=0; i<3; ++i) strain[i]=0;
	
	
}

TriaxialCompressionEngine::~TriaxialCompressionEngine()
{	
}


void TriaxialCompressionEngine::postProcessAttributes(bool deserializing)
{
	if(deserializing)
        translationAxis.Normalize();
}


void TriaxialCompressionEngine::registerAttributes()
{
	TriaxialStressController::registerAttributes();
	REGISTER_ATTRIBUTE(strainRate);
	REGISTER_ATTRIBUTE(currentStrainRate);
	REGISTER_ATTRIBUTE(strain);
	REGISTER_ATTRIBUTE(UnbalancedForce);
	REGISTER_ATTRIBUTE(StabilityCriterion);
	REGISTER_ATTRIBUTE(translationAxis);
	REGISTER_ATTRIBUTE(compressionActivated);
	REGISTER_ATTRIBUTE(autoCompressionActivation);
}



void TriaxialCompressionEngine::updateParameters(Body * body)
{
	
	UnbalancedForce=ComputeUnbalancedForce(body);
	if (Omega::instance().getCurrentIteration() % 100 == 0) cerr << "UnbalancedForce=" << UnbalancedForce << endl;
	
	// new test
	//cerr << "1" << endl;
	if (!Phase1 && autoCompressionActivation &&
	UnbalancedForce<=StabilityCriterion)		//Start 
	{	
// 		// saving snapshot.xml
// 	string fileName = "../data/" + Phase1End + "_" + 
// 	lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
// 	cerr << "saving snapshot: " << fileName << " ...";
// 	Omega::instance().saveSimulation(fileName);
		
		internalCompaction = false;
		Phase1 = true;
		FinalIterationPhase1 =
		Omega::instance().getCurrentIteration();
	}
	
	if (autoCompressionActivation && Phase1
	&& ((Omega::instance().getCurrentIteration()) >=
	(FinalIterationPhase1+1000)))
	
	{
		if (UnbalancedForce<=StabilityCriterion)
		{
		// saving snapshot.xml
	string fileName = "../data/" + Phase1End + "_" + 
	lexical_cast<string>(Omega::instance().getCurrentIteration()) + ".xml";
	cerr << "saving snapshot: " << fileName << " ...";
	Omega::instance().saveSimulation(fileName);
	
	compressionActivated = true;
		}
	
	}

	if (compressionActivated)
	{
		wall_bottom_activated=false;
		wall_top_activated=false;
		autoCompressionActivation = false;
	}
		
}


void TriaxialCompressionEngine::applyCondition(Body * body)
{
	//cerr << "TriaxialCompressionEngine::applyCondition(Body * body)" << endl;
	if (Omega::instance().getCurrentIteration() % interval == 0)//FIXME should never be executed here bu only when a data recorder requires it
                updateParameters(body);

        TriaxialStressController::applyCondition(body);
        MetaBody * ncb2 = static_cast<MetaBody*>(body);
        //cerr << computeStress(ncb2)  << endl;

        if (compressionActivated)
        {
		if (Omega::instance().getCurrentIteration() % 50 == 0) 
 		cerr << "Compression started!!" << endl;
        	Real dt = Omega::instance().getTimeStep();
                  MetaBody * ncb = static_cast<MetaBody*>(body);
                  shared_ptr<BodyContainer>& bodies = ncb->bodies;
                  
                  if (currentStrainRate < strainRate) currentStrainRate
			+= strainRate*0.0003;	// !!! si décharge
                
                  PhysicalParameters* p =
		static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->
		physicalParameters. get());
                  p->se3.position += 0.5*strainRate*height*translationAxis*dt;
//                 cerr << "deplacmt = " <<
// 		0.5*strainRate*height*translationAxis*dt << endl;
//                 cerr << "wall_bottom : p->se3.position = " << p->se3.position
// 		<< endl;
		
		  p =
		static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->	
		physicalParameters.get( ));
                  p->se3.position -= 0.5*strainRate*height*translationAxis*dt;
//                 cerr << "wall_top : p->se3.position = " << p->se3.position
// 		<< endl;

        }
}


/*!
    \fn TriaxialCompressionEngine::ComputeUnbalancedForce()
 */
Real TriaxialCompressionEngine::ComputeUnbalancedForce(Body * body, bool maxUnbalanced)
{
        //compute the mean contact force
        Real MeanForce=0;
        long nForce = 0;

        MetaBody * ncb = static_cast<MetaBody*>(body);
        shared_ptr<BodyContainer>& bodies = ncb->bodies;

        InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
        InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
        for(  ; ii!=iiEnd ; ++ii ) {
                if ((*ii)->isReal) {
                        const shared_ptr<Interaction>& contact = *ii;
                        Real fn = (static_cast<ElasticContactInteraction*> (contact->interactionPhysics.get()))->normalForce.Length();
                        if (fn!=0)
                        {
                        MeanForce += (static_cast<ElasticContactInteraction*> (contact->interactionPhysics.get()))->normalForce.Length();
                        ++nForce;
                        }
                }
        }
        if (MeanForce!=0) MeanForce /= nForce;

        int actionForceIndex = actionForce->getClassIndex();

        if (!maxUnbalanced) {
                //compute mean Unbalanced Force
                Real MeanUnbalanced=0;
                long nBodies = 0;
                BodyContainer::iterator bi    = bodies->begin();
                BodyContainer::iterator biEnd = bodies->end();
                Real f;
                for(  ; bi!=biEnd ; ++bi ) {
                        if ((*bi)->isDynamic) {
                                f= (static_cast<Force*>   ( ncb->physicalActions->find( (*bi)->getId() , actionForceIndex).get() )->force).Length();
                                MeanUnbalanced += f;
                                if (f!=0) ++nBodies;
                        }
                }
                return MeanUnbalanced/nBodies/MeanForce;
        } else {
                //compute max Unbalanced Force
                Real MaxUnbalanced=0;
                BodyContainer::iterator bi    = bodies->begin();
                BodyContainer::iterator biEnd = bodies->end();
                for(  ; bi!=biEnd ; ++bi ) {
                        if ((*bi)->isDynamic) {
                                MaxUnbalanced = std::max((static_cast<Force*>   ( ncb->physicalActions->find( (*bi)->getId() , actionForceIndex).get() )->force).Length(), MaxUnbalanced);
                        }
                }
                return MaxUnbalanced/MeanForce;
        }
}
