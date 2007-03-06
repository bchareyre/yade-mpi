/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/



#include "BodyMacroParameters.hpp"
#include "SpheresContactGeometry.hpp"
#include "SDECLinkGeometry.hpp"
#include "ElasticContactInteraction.hpp"
#include "SDECLinkPhysics.hpp"
#include <yade/yade-core/Omega.hpp>
#include <yade/yade-core/MetaBody.hpp>
#include <yade/yade-package-common/Force.hpp>
#include <yade/yade-package-common/Momentum.hpp>
#include <yade/yade-core/PhysicalAction.hpp>
#include "GlobalStiffness.hpp"
#include "GlobalStiffnessCounter.hpp"

GlobalStiffnessCounter::GlobalStiffnessCounter() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum), actionStiffness(new GlobalStiffness)
{
	interval=1;//FIXME very high frequency - not required 
	sdecGroupMask=1;
	momentRotationLaw = true;
	actionForceIndex = actionForce->getClassIndex();
	actionMomentumIndex = actionMomentum->getClassIndex();
	actionStiffnessIndex = actionStiffness->getClassIndex();
}


void GlobalStiffnessCounter::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}

bool GlobalStiffnessCounter::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) );
}

void GlobalStiffnessCounter::action(Body* body)
{
        MetaBody * ncb = Dynamic_cast<MetaBody*>(body);
        shared_ptr<BodyContainer>& bodies = ncb->bodies;

        Real dt = Omega::instance().getTimeStep();

        /// Non Permanents Links												///

        InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
        InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
        //cerr << "#############################################################################################" << endl;
        for(  ; ii!=iiEnd ; ++ii ) {
                if ((*ii)->isReal) {
                        const shared_ptr<Interaction>& contact = *ii;
                        int id1 = contact->getId1();
                        int id2 = contact->getId2();

                        //if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
                        //continue; // skip other groups, BTW: this is example of a good usage of 'continue' keyword

                        SpheresContactGeometry* currentContactGeometry 	= static_cast<SpheresContactGeometry*>(contact->interactionGeometry.get());
                        ElasticContactInteraction * currentContactPhysics = static_cast<ElasticContactInteraction *> (contact->interactionPhysics.get());


                        Real fn = (static_cast<ElasticContactInteraction *> (contact->interactionPhysics.get()))->normalForce.Length();

                        if (fn!=0) {
                                //Diagonal terms of the translational stiffness matrix
                                Vector3r diag_stiffness = Vector3r( 	std::pow(currentContactGeometry->normal.X(),2),
                                                                     std::pow(currentContactGeometry->normal.Y(),2),
                                                                     std::pow(currentContactGeometry->normal.Z(),2) )
                                                          ;
                                diag_stiffness *= currentContactPhysics->kn-currentContactPhysics->ks;
                                diag_stiffness = diag_stiffness + Vector3r(1,1,1)*currentContactPhysics->ks;

                                //diagonal terms of the rotational stiffness matrix
                                Vector3r branch1 = currentContactGeometry->normal*currentContactGeometry->radius1;
                                Vector3r branch2 = currentContactGeometry->normal*currentContactGeometry->radius2;
                                Vector3r diag_Rstiffness =
                                        Vector3r( std::pow(currentContactGeometry->normal.Y(),2) + std::pow(currentContactGeometry->normal.Z(),2),
                                                  std::pow(currentContactGeometry->normal.X(),2) + std::pow(currentContactGeometry->normal.Z(),2),
                                                  std::pow(currentContactGeometry->normal.Y(),2) + std::pow(currentContactGeometry->normal.Y(),2) );
                                diag_Rstiffness *= currentContactPhysics->ks;
                                //cerr << "diag_Rstifness=" << diag_Rstiffness << endl;


                                PhysicalAction* st = ncb->physicalActions->find(id1,actionStiffness->getClassIndex()).get();
                                GlobalStiffness* s = static_cast<GlobalStiffness*>(st);
                                s->stiffness += diag_stiffness;
                                s->Rstiffness += diag_Rstiffness;
                                st = ncb->physicalActions->find(id2,actionStiffness->getClassIndex()).get();
                                s = static_cast<GlobalStiffness*>(st);
                                s->stiffness += diag_stiffness;
                                s->Rstiffness += diag_Rstiffness;
                                //____________________________
                        }
                }
        }
}



