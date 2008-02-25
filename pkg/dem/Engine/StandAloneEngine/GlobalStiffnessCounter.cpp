/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/



#include<yade/pkg-dem/BodyMacroParameters.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/SDECLinkGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-dem/SDECLinkPhysics.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>
#include<yade/pkg-dem/GlobalStiffness.hpp>
#include "GlobalStiffnessCounter.hpp"

//#include<yade/extra/Brefcom.hpp>

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
	return ((Omega::instance().getCurrentIteration() % interval == 0) || (Omega::instance().getCurrentIteration() < (long int) 2));
}


bool GlobalStiffnessCounter::getSphericalElasticInteractionParameters(const shared_ptr<Interaction>& contact, Vector3r& normal, Real& kn, Real& ks, Real& radius1, Real& radius2){
	shared_ptr<SpheresContactGeometry> currentContactGeometry=YADE_PTR_CAST<SpheresContactGeometry>(contact->interactionGeometry);
	shared_ptr<ElasticContactInteraction> currentContactPhysics=YADE_PTR_CAST<ElasticContactInteraction>(contact->interactionPhysics);

	Real fn=currentContactPhysics->normalForce.SquaredLength();
	if(fn==0) return false;//This test means : is something happening at this contact : no question about numerical error
	normal=currentContactGeometry->normal;
	radius1=currentContactGeometry->radius1; radius2=currentContactGeometry->radius2;
	kn=currentContactPhysics->kn; ks=currentContactPhysics->ks;
	return true;
}

bool GlobalStiffnessCounter::getInteractionParameters(const shared_ptr<Interaction>& contact, Vector3r& normal, Real& kn, Real& ks, Real& radius1, Real& radius2){

	shared_ptr<SpheresContactGeometry> geom1=YADE_PTR_CAST<SpheresContactGeometry>(contact->interactionGeometry);
	shared_ptr<ElasticContactInteraction> phys1=YADE_PTR_CAST<ElasticContactInteraction>(contact->interactionPhysics);
	if(geom1 && phys1){
		Real fn=phys1->normalForce.Length();
		if(fn==0) return false;
		normal=geom1->normal;
		radius1=geom1->radius1; radius2=geom1->radius2;
		kn=phys1->kn; ks=phys1->ks;
		return true;
	}

// 	shared_ptr<SDECLinkGeometry> geom3=dynamic_pointer_cast<SDECLinkGeometry>(contact->interactionGeometry);
// 	shared_ptr<SDECLinkPhysics> phys3=dynamic_pointer_cast<SDECLinkPhysics>(contact->interactionPhysics);
// 	if(geom3 && phys3){
// 		Real fn=phys3->normalForce.Length();
// 		if(fn==0) return false;
// 		normal=geom3->normal;
// 		radius1=geom3->radius1; radius2=geom3->radius2;
// 		kn=phys3->kn; ks=phys3->ks;
// 		return true;
// 	}
// 
// 	shared_ptr<SpheresContactGeometry> geom2=dynamic_pointer_cast<SpheresContactGeometry>(contact->interactionGeometry);
// 	shared_ptr<BrefcomContact> phys2=dynamic_pointer_cast<BrefcomContact>(contact->interactionPhysics);
// 	if(geom2 && phys2){
// 		return false; // FIXME, adapt to refactored Brefcom
// 	}

	return false;
}

void GlobalStiffnessCounter::traverseInteractions(MetaBody* ncb, const shared_ptr<InteractionContainer>& interactions, bool spheresOnly){
	for(InteractionContainer::iterator I=interactions->begin(); I!=interactions->end(); ++I){
		const shared_ptr<Interaction>& contact = *I;
		if(!contact->isReal) continue;

		body_id_t id1=contact->getId1(), id2=contact->getId2();

		// all we need for getting stiffness
		Vector3r normal; Real kn, ks, radius1, radius2;

		/* This is to overcome class scatter:
		 * 	SpheresContactGeometry & ElasticContactInteraction
		 * 	or
		 * 	SDECLinkGeometry & SDECLinkPhysics
		 * 	or
		 * 	SpheresContactGeometry & BrefcomContact
		 */
		if(spheresOnly){ // go fast, may CRASH for non-spherical stuff however (!)
			if(!getSphericalElasticInteractionParameters(contact, normal, kn, ks, radius1, radius2)) continue;
		} else {
			if(!getInteractionParameters(contact,normal,kn,ks,radius1,radius2)) continue;
		}

		//Diagonal terms of the translational stiffness matrix
		Vector3r diag_stiffness = Vector3r(std::pow(normal.X(),2),std::pow(normal.Y(),2),std::pow(normal.Z(),2));
		diag_stiffness *= kn-ks;
		diag_stiffness = diag_stiffness + Vector3r(1,1,1)*ks;

		//diagonal terms of the rotational stiffness matrix
		// Vector3r branch1 = currentContactGeometry->normal*currentContactGeometry->radius1;
		// Vector3r branch2 = currentContactGeometry->normal*currentContactGeometry->radius2;
		Vector3r diag_Rstiffness =
			Vector3r(std::pow(normal.Y(),2)+std::pow(normal.Z(),2),
				std::pow(normal.X(),2)+std::pow(normal.Z(),2),
				std::pow(normal.X(),2)+std::pow(normal.Y(),2));
		diag_Rstiffness *= ks;
		//cerr << "diag_Rstifness=" << diag_Rstiffness << endl;

		PhysicalAction* st = ncb->physicalActions->find(id1,actionStiffness->getClassIndex()).get();
		GlobalStiffness* s = static_cast<GlobalStiffness*>(st);
		s->stiffness += diag_stiffness;
		s->Rstiffness += diag_Rstiffness*pow(radius1,2);	
		st = ncb->physicalActions->find(id2,actionStiffness->getClassIndex()).get();
		s = static_cast<GlobalStiffness*>(st);
		s->stiffness += diag_stiffness;
		s->Rstiffness += diag_Rstiffness*pow(radius2,2);
	}
}

void GlobalStiffnessCounter::action(Body* body)
{
	MetaBody * ncb = YADE_CAST<MetaBody*>(body);
	// shared_ptr<BodyContainer>& bodies = ncb->bodies;
	//	Real dt = Omega::instance().getTimeStep();

	/// transient Links
	traverseInteractions(ncb,ncb->transientInteractions, /*spheresOnly? */ true);

	/* ignore pesistent links, unused */
	// traverseInteractions(ncb,ncb->persistentInteractions);
#if 0
        InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
        InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
        //cerr << "#############################################################################################" << endl;
        for(  ; ii!=iiEnd ; ++ii ) {
                if ((*ii)->isReal) {
                        const shared_ptr<Interaction>& contact = *ii;
                        int id1 = contact->getId1();
                        int id2 = contact->getId2();

			//if body_getId1.isClumpMember() Id1 = ball from clump Idc
			// Id1 = Body::clumpId

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
				s->Rstiffness += diag_Rstiffness*pow(currentContactGeometry->radius1,2);	
				st = ncb->physicalActions->find(id2,actionStiffness->getClassIndex()).get();
				s = static_cast<GlobalStiffness*>(st);
				s->stiffness += diag_stiffness;
				s->Rstiffness += diag_Rstiffness*pow(currentContactGeometry->radius2,2);
                                //____________________________
                        }
                }
        }
#endif
}



YADE_PLUGIN();
