/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre                                   *
* bruno.chareyre@hmg.inpg.fr                                                   *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TriaxialStressController.hpp"
#include "yade/yade-package-common/ParticleParameters.hpp"
#include "yade/yade-package-common/InteractingSphere.hpp"
#include "SpheresContactGeometry.hpp"
#include "ElasticContactInteraction.hpp"
#include "yade/yade-package-common/Force.hpp"
//#include "GlobalStiffness.hpp"
#include <Wm3Math.h>
#include <yade/yade-lib-base/yadeWm3.hpp>
#include <yade/yade-lib-base/yadeWm3Extra.hpp>

#include <yade/yade-core/MetaBody.hpp>
#include "yade/yade-package-common/Sphere.hpp"



TriaxialStressController::TriaxialStressController() : actionParameterForce(new Force), wall_bottom_id(wall_id[0]), wall_top_id(wall_id[1]), wall_left_id(wall_id[2]), wall_right_id(wall_id[3]), wall_front_id(wall_id[4]), wall_back_id(wall_id[5])
{
	//cerr << "constructor of TriaxialStressController" << std::endl;
	ForceClassIndex = actionParameterForce->getClassIndex();
	previousStress = 0;
	previousMultiplier = 1;
	wall_bottom = 0;
	wall_top = 1;
	wall_left = 2;
	wall_right = 3;
	wall_front = 4;
	wall_back = 5;
	
	
	interval =10;
	wallDamping = 0.25;
	force = Vector3r::ZERO;
	for (int i=0; i<6; ++i)
	{
		wall_id[i] = 0;
		previousTranslation[i] = Vector3r::ZERO;
		stiffness[i] = 0;
		normal[i] = Vector3r::ZERO;
	}
	normal[wall_bottom].Y()=1;
	normal[wall_top].Y()=-1;
	normal[wall_left].X()=1;
	normal[wall_right].X()=-1;
	normal[wall_front].Z()=-1;
	normal[wall_back].Z()=1;
	
// 	wall_bottom_id = wall_id[0];
// 	wall_top_id = wall_id[1];
// 	wall_left_id = wall_id[2];
// 	wall_right_id = wall_id[3];
// 	wall_front_id = wall_id[4];
// 	wall_back_id = wall_id[5];
	
	//stiffness = Vector3r::ZERO;
	max_vel = 0.001;
	maxMultiplier = 1.001;
	internalCompaction = false;
	
	ForceClassIndex = actionParameterForce->getClassIndex();
			
	wall_bottom_activated = true;
	wall_top_activated = true;
	wall_left_activated = true;
	wall_right_activated = true;
	wall_front_activated = true;
	wall_back_activated = true;
	
	height = 0;
	width = 0;
	depth = 0;
	thickness = 0;
	
	sigma_iso = 0;
	//cerr << "end of TriaxialStressController constructor" << std::endl;
}

TriaxialStressController::~TriaxialStressController()
{	
}

void TriaxialStressController::registerAttributes()
{
	//cerr << "TriaxialStressController::registerAttributes()" << std::endl;
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(interval);
	REGISTER_ATTRIBUTE(wallDamping);
	REGISTER_ATTRIBUTE(force);
	
	
	
	//REGISTER_ATTRIBUTE(stiffness);
 	REGISTER_ATTRIBUTE(wall_bottom_id);
 	REGISTER_ATTRIBUTE(wall_top_id);
 	REGISTER_ATTRIBUTE(wall_left_id);
 	REGISTER_ATTRIBUTE(wall_right_id);
 	REGISTER_ATTRIBUTE(wall_front_id);
 	REGISTER_ATTRIBUTE(wall_back_id);
//	REGISTER_ATTRIBUTE(wall_id);
	
	REGISTER_ATTRIBUTE(wall_bottom_activated);
	REGISTER_ATTRIBUTE(wall_top_activated);
	REGISTER_ATTRIBUTE(wall_left_activated);
	REGISTER_ATTRIBUTE(wall_right_activated);
	REGISTER_ATTRIBUTE(wall_front_activated);
	REGISTER_ATTRIBUTE(wall_back_activated);
	
	REGISTER_ATTRIBUTE(height);
	REGISTER_ATTRIBUTE(width);
	REGISTER_ATTRIBUTE(depth);
	REGISTER_ATTRIBUTE(thickness);
	
	
	REGISTER_ATTRIBUTE(sigma_iso);
	REGISTER_ATTRIBUTE(maxMultiplier);
	REGISTER_ATTRIBUTE(max_vel);
	REGISTER_ATTRIBUTE(previousStress);
	REGISTER_ATTRIBUTE(previousMultiplier);
	REGISTER_ATTRIBUTE(internalCompaction);
	//cerr << "fin de TriaxialStressController::registerAttributes()" << std::endl;
}

void TriaxialStressController::updateStiffness (MetaBody * ncb)
{
	//shared_ptr<BodyContainer>& bodies = ncb->bodies;

	//Real dt = Omega::instance().getTimeStep();

	for (int i=0; i<6; ++i)
	{		
		stiffness[i] = 0;
	}											///

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			const shared_ptr<Interaction>& contact = *ii;
			int id1 = contact->getId1();
			int id2 = contact->getId2();
			
			for (int index=0; index<6; ++index)
			{
				if ( wall_id[index]==id1 || wall_id[index]==id2 )
				{
					ElasticContactInteraction* currentContactPhysics =
					static_cast<ElasticContactInteraction*> ( contact->interactionPhysics.get() );
					stiffness[index]  += currentContactPhysics->kn;
				
				}			
			}			
		}
	}

}

void TriaxialStressController::controlExternalStress(int wall, MetaBody* ncb, int id, Vector3r resultantForce, PhysicalParameters* p, Real wall_max_vel) //FIXME remove parameter "id"
{
		//Update stiffness only if it has been computed by StiffnessCounter (see "interval")
		//if (Omega::instance().getCurrentIteration() % interval == 0)	stiffness =
		//(static_cast<StiffnessMatrix*>( ncb->physicalActions->find (id, StiffnessMatrixClassIndex).get()))->stiffness;		
// 		Vector3r effectiveforce =
// 		 	static_cast<Force*>( ncb->physicalActions->find(wall_id[wall],ForceClassIndex).get() )->force; 
		//Vector3r deltaf (effectiveforce - resultantForce);
		Real translation= normal[wall].Dot(static_cast<Force*>( ncb->physicalActions->find(wall_id[wall],ForceClassIndex).get() )->force - resultantForce);
		if (translation!=0)
		{
			if (stiffness[wall]!=0)
			{
				translation /= stiffness[wall];
				translation = std::min( abs(translation), max_vel ) * Mathr::Sign(translation);
			}
			else  translation = wall_max_vel * Mathr::Sign(translation);
		}
		
		 

			
		previousTranslation[wall] = (1-wallDamping)*translation*normal[wall] + 0.7*previousTranslation[wall];// formula for "steady-flow" evolution with fluctuations
		p->se3.position	+= previousTranslation[wall];
		//cerr << "previoustranslation[wall]=" << previoustranslation[wall] << endl;
		//p->velocity		=  previoustranslation/dt;//FIXME : useless???	
	//}
	
}
void TriaxialStressController::applyCondition(Body* body)
{
	//cerr << "void TriaxialStressController::applyCondition(Body* body)" << std::endl;
	MetaBody * ncb = dynamic_cast<MetaBody*>(body);
	
	//Update stiffness only if it has been computed by StiffnessCounter (see "interval")
	if (Omega::instance().getCurrentIteration() % interval == 0) updateStiffness(ncb);

	
	shared_ptr<BodyContainer>& bodies = ncb->bodies;
	
	//if(PhysicalParameters* p = dynamic_cast<PhysicalParameters*>((*bodies)[*ii]->physicalParameters.get()))
	
// 	if(PhysicalParameters* p_bottom = static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters.get()) &&
// 	PhysicalParameters* p_top   =	 static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get()) &&
// 	PhysicalParameters* p_left 	= static_cast<PhysicalParameters*>((*bodies)[wall_left_id]->physicalParameters.get()) &&
// 	PhysicalParameters* p_right = static_cast<PhysicalParameters*>((*bodies)[wall_right_id]->physicalParameters.get()) &&
// 	PhysicalParameters* p_front = static_cast<PhysicalParameters*>((*bodies)[wall_front_id]->physicalParameters.get()) &&
// 	PhysicalParameters* p_back 	= static_cast<PhysicalParameters*>((*bodies)[wall_back_id]->physicalParameters.get()))
	
	
	
	PhysicalParameters* p_bottom = static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters.get());
	PhysicalParameters* p_top   =	 static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get());
	PhysicalParameters* p_left 	= static_cast<PhysicalParameters*>((*bodies)[wall_left_id]->physicalParameters.get());
	PhysicalParameters* p_right = static_cast<PhysicalParameters*>((*bodies)[wall_right_id]->physicalParameters.get());
	PhysicalParameters* p_front = static_cast<PhysicalParameters*>((*bodies)[wall_front_id]->physicalParameters.get());
	PhysicalParameters* p_back 	= static_cast<PhysicalParameters*>((*bodies)[wall_back_id]->physicalParameters.get());
	
	
	height = p_top->se3.position.Y() - p_bottom->se3.position.Y() - thickness;
	width = p_right->se3.position.X() - p_left->se3.position.X() - thickness;
	depth = p_front->se3.position.Z() - p_back->se3.position.Z() - thickness;
				
	if (!internalCompaction)
    	{	
		Vector3r wallForce (0, sigma_iso*width*depth, 0);
		if (wall_bottom_activated)
		controlExternalStress(wall_bottom, ncb, wall_bottom_id, -wallForce, p_bottom, max_vel);
		if (wall_top_activated)
		controlExternalStress(wall_top, ncb, wall_top_id, wallForce, p_top, max_vel);
		
		wallForce = Vector3r(sigma_iso*height*depth, 0, 0);
		if (wall_left_activated)
		controlExternalStress(wall_left, ncb, wall_left_id, -wallForce, p_left, max_vel*width/height);
		if (wall_right_activated)
		controlExternalStress(wall_right, ncb, wall_right_id, wallForce, p_right, max_vel*width/height);
		
		wallForce = Vector3r(0, 0, sigma_iso*height*depth);
		if (wall_back_activated)
		controlExternalStress(wall_back, ncb, wall_back_id, -wallForce, p_back, max_vel*depth/height);
		if (wall_front_activated)
		controlExternalStress(wall_front, ncb, wall_front_id, wallForce, p_front, max_vel*depth/height);
					
	//}
	}
	
    else //if internal compaction
    {	
    	if (Omega::instance().getCurrentIteration() % 10 == 0) {
    	Real s = computeStress(ncb);
    	if (s==0) previousMultiplier = maxMultiplier; 
    	else {
//     		previousMultiplier = 1+0.7*(sigma_iso-s)*(previousMultiplier-1.f)/(s-previousStress); // = (Dsigma/apparentModulus)*0.7
//     		previousMultiplier = std::max(2-maxMultiplier, std::min(previousMultiplier, maxMultiplier));
		previousMultiplier = 1+(sigma_iso-s)/sigma_iso*(maxMultiplier-1.f); // = (Dsigma/apparentModulus)*0.7
    	     }
    	previousStress = s;
    	if (Omega::instance().getCurrentIteration() % 50 == 0) cerr << "s= " << s << "; previousMultiplier = " << previousMultiplier << endl;
    	//Real apparentModulus = (s-previousStress)/(previousMultiplier-1.f);    	
    	controlInternalStress(ncb, previousMultiplier);
    	}
    }
}


Real TriaxialStressController::computeStress(MetaBody* ncb)
{
// 	height = p_top->se3.position.y() - p_bottom->se3.position.y() - thickness;
// 	width = p_right->se3.position.x() - p_left->se3.position.x() - thickness;
// 	depth = p_front->se3.position.z() - p_back->se3.position.z() - thickness;
	
	Real meanStress = 0;
	
	Real invXSurface = 1/(height*depth);
	Real invYSurface = 1/(width*depth);
	Real invZSurface = 1/(width*height);
		
	stress[wall_bottom]=( static_cast<Force*>(ncb->physicalActions->find(wall_id[wall_bottom],ForceClassIndex).get())->force ) * invYSurface;
	stress[wall_top] = static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_top],ForceClassIndex).get() )->force * invYSurface;
	stress[wall_left] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_left],ForceClassIndex).get() )->force ) * invXSurface;
	stress[wall_right] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_right],ForceClassIndex).get() )->force ) * invXSurface;
	stress[wall_front] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_front],ForceClassIndex).get() )->force ) * invZSurface;
	stress[wall_back] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_back],ForceClassIndex).get() )->force ) * invZSurface;	
		
	if (Omega::instance().getCurrentIteration() % 50 == 0) {
	cerr << "stresses : " <<  -stress[wall_bottom].Y() << " " << stress[wall_top].Y() << " " << -stress[wall_left].X() << " " << stress[wall_right].X() << " " << -stress[wall_back].Z() << " " << stress[wall_front].Z() << endl;}
	for (int i=0; i<6; i++) {
	//Real x= stress[i].dot(normal[i]);
	//cerr << i << " : " << x << endl;
	meanStress-= stress[i].Dot(normal[i]);}
	return meanStress*0.16666666666;
}

void TriaxialStressController::controlInternalStress(MetaBody* ncb, Real multiplier)
{
        BodyContainer::iterator bi    = ncb->bodies->begin();
        BodyContainer::iterator biEnd = ncb->bodies->end();
        Real f;
        for(  ; bi!=biEnd ; ++bi ) {
                if ((*bi)->isDynamic) {
                        (static_cast<InteractingSphere*> ((*bi)->interactingGeometry.get()))->radius *= multiplier;
                        (static_cast<Sphere*>((*bi)->geometricalModel.get()))->radius *= multiplier;}
        }

        InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
        InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
        for(  ; ii!=iiEnd ; ++ii ) {
                if ((*ii)->isReal) {
                        SpheresContactGeometry* contact = static_cast<SpheresContactGeometry*> ((*ii)->interactionGeometry.get());
                        if ((*(ncb->bodies))[(*ii)->getId1()]->isDynamic)
                                contact->radius1 *= multiplier;
                        if ((*(ncb->bodies))[(*ii)->getId2()]->isDynamic)
                                contact->radius2 *= multiplier;
                      
                }
        }
}
