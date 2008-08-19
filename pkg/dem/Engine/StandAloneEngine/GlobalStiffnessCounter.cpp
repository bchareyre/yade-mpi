/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                  *
*  bruno.chareyre@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/NormalShearInteractions.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/core/Omega.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Force.hpp>
#include<yade/pkg-common/Momentum.hpp>
#include<yade/core/PhysicalAction.hpp>
#include<yade/pkg-dem/GlobalStiffness.hpp>
#include"GlobalStiffnessCounter.hpp"

GlobalStiffnessCounter::GlobalStiffnessCounter() : InteractionSolver()
{
	interval=100;
	//sdecGroupMask=1;
	actionForceIndex=shared_ptr<Force>(new Force)->getClassIndex();
	actionMomentumIndex=shared_ptr<Momentum>(new Momentum)->getClassIndex();
	actionStiffnessIndex=shared_ptr<GlobalStiffness>(new GlobalStiffness)->getClassIndex();
}


void GlobalStiffnessCounter::registerAttributes()
{
	InteractionSolver::registerAttributes();
	REGISTER_ATTRIBUTE(interval);
	//REGISTER_ATTRIBUTE(sdecGroupMask);
}

bool GlobalStiffnessCounter::isActivated()
{
	return ((Omega::instance().getCurrentIteration() % interval == 0) || (Omega::instance().getCurrentIteration() < (long int) 2));
}

#if 0
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
#endif

void GlobalStiffnessCounter::traverseInteractions(MetaBody* ncb, const shared_ptr<InteractionContainer>& interactions){
	FOREACH(const shared_ptr<Interaction>& contact, *interactions){
		if(!contact->isReal) continue;

		SpheresContactGeometry* geom=YADE_CAST<SpheresContactGeometry*>(contact->interactionGeometry.get()); assert(geom);
		// FIXME: for some reason, the following cast is not successful. Please help!
		//cerr<<__FILE__<<":"<<__LINE__<<" Interaction physics of type "<<contact->interactionPhysics->getClassName()<<" ("<<typeid(*contact->interactionPhysics).name()<<")"<<endl;
		//NormalShearInteraction* phys=YADE_CAST<NormalShearInteraction*>(contact->interactionPhysics.get()); assert(phys);
		ElasticContactInteraction* phys=YADE_CAST<ElasticContactInteraction*>(contact->interactionPhysics.get()); assert(phys);
		// all we need for getting stiffness
		Vector3r& normal=geom->normal; Real& kn=phys->kn; Real& ks=phys->ks; Real& radius1=geom->radius1; Real& radius2=geom->radius2;
		// FIXME? NormalShearInteraction knows nothing about whether the contact is "active" (force!=0) or not;
		// former code: if(force==0) continue; /* disregard this interaction, it is not active */.
		// It seems though that in such case either the interaction is accidentally at perfect equilibrium (unlikely)
		// or it should have been deleted already. Right?

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

		PhysicalAction* st = ncb->physicalActions->find(contact->getId1(),actionStiffnessIndex).get();
		GlobalStiffness* s = static_cast<GlobalStiffness*>(st);
		s->stiffness += diag_stiffness;
		s->Rstiffness += diag_Rstiffness*pow(radius1,2);	
		st = ncb->physicalActions->find(contact->getId2(),actionStiffnessIndex).get();
		s = static_cast<GlobalStiffness*>(st);
		s->stiffness += diag_stiffness;
		s->Rstiffness += diag_Rstiffness*pow(radius2,2);
	}
}

void GlobalStiffnessCounter::action(MetaBody* ncb)
{
	// transient Links
	traverseInteractions(ncb,ncb->transientInteractions);

	// ignore pesistent links, unused
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


                               PhysicalAction* st = ncb->physicalActions->find(id1,actionStiffnessIndex).get();
				GlobalStiffness* s = static_cast<GlobalStiffness*>(st);
				s->stiffness += diag_stiffness;
				s->Rstiffness += diag_Rstiffness*pow(currentContactGeometry->radius1,2);	
				st = ncb->physicalActions->find(id2,actionStiffnessIndex).get();
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
