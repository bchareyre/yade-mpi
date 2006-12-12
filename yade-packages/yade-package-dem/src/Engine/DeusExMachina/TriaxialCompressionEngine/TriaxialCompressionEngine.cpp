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


TriaxialCompressionEngine::TriaxialCompressionEngine() : actionForce(new Force)
{
	translationAxis=TriaxialStressController::normal[wall_bottom_id];
	strainRate=0;
	StabilityCriterion=0.001;
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
	cerr << "UnbalancedForce=" << UnbalancedForce << endl;
	if (autoCompressionActivation) compressionActivated = (UnbalancedForce<=StabilityCriterion);//Is the assembly compact and stable?
	if (compressionActivated)
	{
		wall_bottom_activated=false;//stop stress control on top and bottom wall
		wall_top_activated=false;
		autoCompressionActivation = false; //avoid stopping compression if UnbalancedForce increases due to compression		
	}
	
	
}


void TriaxialCompressionEngine::applyCondition(Body * body)
{
	//cerr << "TriaxialCompressionEngine::applyCondition(Body * body)" << endl;
	if (Omega::instance().getCurrentIteration() % interval == 0)//FIXME should never be executed here bu only when a data recorder requires it
                updateParameters(body);

        TriaxialStressController::applyCondition(body);

        if (compressionActivated)
        {
		cerr << "Compression started!!" << endl;
        	Real dt = Omega::instance().getTimeStep();
                MetaBody * ncb = static_cast<MetaBody*>(body);
                shared_ptr<BodyContainer>& bodies = ncb->bodies;
                PhysicalParameters* p = static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters.get());
                p->se3.position += 0.5*strainRate*height*translationAxis*dt;
                p = static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get());
                p->se3.position -= 0.5*strainRate*height*translationAxis*dt;
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
                        MeanForce += (static_cast<ElasticContactInteraction*> (contact->interactionPhysics.get()))->normalForce.Length();
                        ++nForce;
                }
        }
        MeanForce /= nForce;

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
