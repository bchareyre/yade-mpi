/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
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
#include "StiffnessMatrix.hpp"
#include "StiffnessCounter.hpp"

StiffnessCounter::StiffnessCounter() : InteractionSolver() , actionForce(new Force) , actionMomentum(new Momentum), actionStiffness(new StiffnessMatrix)
{
	interval=1;//FIXME very high frequency - not required 
	sdecGroupMask=1;
	momentRotationLaw = true;
}


void StiffnessCounter::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(sdecGroupMask);
	REGISTER_ATTRIBUTE(momentRotationLaw);
}

bool StiffnessCounter::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) );
}

void StiffnessCounter::action(Body* body)
{
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

/// Non Permanents Links												///

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();
			
			//if( !( (*bodies)[id1]->getGroupMask() & (*bodies)[id2]->getGroupMask() & sdecGroupMask)  )
				//continue; // skip other groups, BTW: this is example of a good usage of 'continue' keyword
	
			SpheresContactGeometry* currentContactGeometry 	= static_cast<SpheresContactGeometry*>(contact->interactionGeometry.get());
			ElasticContactInteraction* currentContactPhysics = static_cast<ElasticContactInteraction*> (contact->interactionPhysics.get());
			
		/*	//old style__________________						
			Matrix3r matrix1, matrix2;	
 			matrix1.makeTensorProduct(currentContactGeometry->normal,currentContactGeometry->normal);
 			matrix2.makeIdentity ();
 			matrix2 = matrix2*currentContactPhysics->ks + matrix1*(currentContactPhysics->kn-currentContactPhysics->ks);
			PhysicalAction* st = ncb->physicalActions->find(id1,actionStiffness->getClassIndex()).get();
			StiffnessMatrix* s = static_cast<StiffnessMatrix*>(st);
			s->stiffness += matrix2; 	
			st = ncb->physicalActions->find(id2,actionStiffness->getClassIndex()).get();
			s = static_cast<StiffnessMatrix*>(st);
			s->stiffness += matrix2;
			//_____________________________	*/		
			
			//optimized style____________		
			Vector3r diag_stiffness = Vector3r( 	std::pow(currentContactGeometry->normal.X(),2),
								std::pow(currentContactGeometry->normal.Y(),2),
								std::pow(currentContactGeometry->normal.Z(),2) );
			diag_stiffness *= currentContactPhysics->kn-currentContactPhysics->ks;
			diag_stiffness = diag_stiffness + Vector3r(1,1,1)*currentContactPhysics->ks;	//diagonal terms of stiffness matrix
			
			PhysicalAction* st = ncb->physicalActions->find(id1,actionStiffness->getClassIndex()).get();
			StiffnessMatrix* s = static_cast<StiffnessMatrix*>(st);
			s->stiffness += diag_stiffness; 	
			st = ncb->physicalActions->find(id2,actionStiffness->getClassIndex()).get();
			s = static_cast<StiffnessMatrix*>(st);
			s->stiffness += diag_stiffness;
			//____________________________
			
			
		}
	}

}


