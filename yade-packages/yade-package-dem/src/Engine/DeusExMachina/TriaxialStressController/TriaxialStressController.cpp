/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "TriaxialStressController.hpp"
#include "yade/yade-package-common/ParticleParameters.hpp"
#include "ElasticContactInteraction.hpp"
#include "yade/yade-package-common/Force.hpp"
#include "StiffnessMatrix.hpp"
#include <yade/yade-lib-wm3-math/Math.hpp>


#include <yade/yade-core/MetaBody.hpp>


TriaxialStressController::TriaxialStressController() : actionParameterStiffnessMatrix(new StiffnessMatrix), actionParameterForce(new Force), wall_bottom_id(wall_id[0]), wall_top_id(wall_id[1]), wall_left_id(wall_id[2]), wall_right_id(wall_id[3]), wall_front_id(wall_id[4]), wall_back_id(wall_id[5])
{
	//cerr << "constructor of TriaxialStressController" << std::endl;
	StiffnessMatrixClassIndex = actionParameterStiffnessMatrix->getClassIndex();
	ForceClassIndex = actionParameterForce->getClassIndex();
	wall_bottom = 0;
	wall_top = 1;
	wall_left = 2;
	wall_right = 3;
	wall_front = 4;
	wall_back = 5;
	
	interval =1;
	damping = 0.1;
	force = Vector3r::ZERO;
	for (int i=0; i<6; ++i)
	{
		wall_id[i] = 0;
		previoustranslation[i] = Vector3r::ZERO;
		stiffness[i] = 0;
		normal[i] = Vector3r::ZERO;
	}
	normal[wall_bottom].y()=1;
	normal[wall_top].y()=-1;
	normal[wall_left].x()=1;
	normal[wall_right].x()=-1;
	normal[wall_front].z()=-1;
	normal[wall_back].z()=1;
	
// 	wall_bottom_id = wall_id[0];
// 	wall_top_id = wall_id[1];
// 	wall_left_id = wall_id[2];
// 	wall_right_id = wall_id[3];
// 	wall_front_id = wall_id[4];
// 	wall_back_id = wall_id[5];
	
	//stiffness = Vector3r::ZERO;
	max_vel = 0.001;
	StiffnessMatrixClassIndex = actionParameterStiffnessMatrix->getClassIndex();
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
	REGISTER_ATTRIBUTE(damping);
	REGISTER_ATTRIBUTE(force);
	
	
	//REGISTER_ATTRIBUTE(stiffness);
 	REGISTER_ATTRIBUTE(max_vel);
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
	//cerr << "fin de TriaxialStressController::registerAttributes()" << std::endl;
}

void TriaxialStressController::updateStiffness (MetaBody * ncb)
{
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	Real dt = Omega::instance().getTimeStep();

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

void TriaxialStressController::controlStress(int wall, MetaBody* ncb, int id, Vector3r resultantForce, PhysicalParameters* p, Real wall_max_vel)
{
		//Update stiffness only if it has been computed by StiffnessCounter (see "interval")
		//if (Omega::instance().getCurrentIteration() % interval == 0)	stiffness =
		//(static_cast<StiffnessMatrix*>( ncb->physicalActions->find (id, StiffnessMatrixClassIndex).get()))->stiffness;		
// 		Vector3r effectiveforce =
// 		 	static_cast<Force*>( ncb->physicalActions->find(wall_id[wall],ForceClassIndex).get() )->force; 
		//Vector3r deltaf (effectiveforce - resultantForce);
		Real translation= normal[wall].dot(static_cast<Force*>( ncb->physicalActions->find(wall_id[wall],ForceClassIndex).get() )->force - resultantForce);
		if (translation!=0)
		{
			if (stiffness[wall]!=0)
			{
				translation /= stiffness[wall];
				translation = std::min( abs(translation), max_vel ) * Mathr::sign(translation);
			}
			else  translation = wall_max_vel * Mathr::sign(translation);
		}
		
		 
// 		cerr << "dint wall = " <<  wall ;
// 		cerr << " deltaf.x() = " <<  deltaf.x() ;
// 		if  (deltaf.x()!=0) translation.x() = 
// 		Mathr::sign(deltaf.x())*(stiffness.x()==0 ? wall_max_vel : std::min( abs(deltaf.x()/stiffness.x()), wall_max_vel));
// 		else translation.x() = 0;
// 		cerr << " deltaf.y() = " <<  deltaf.y() ;
// 		cerr << " deltaf.z() = " <<  deltaf.z() << endl;
// 		
// 		if  (deltaf.y()!=0) translation.y() = 
// 		Mathr::sign(deltaf.y())*(stiffness.y()==0 ? wall_max_vel : std::min( abs(deltaf.y()/stiffness.y()), wall_max_vel));
// 		else translation.y() = 0;
// 		if  (deltaf.z()!=0) translation.z() = 
// 		Mathr::sign(deltaf.z())*(stiffness.z()==0 ? wall_max_vel : std::min( abs(deltaf.z()/stiffness.z()), wall_max_vel));
// 		else translation.z() = 0;
		
// 			(stiffness.x()==0 ? Mathr::sign(deltaf.x())*wall_max_vel : Mathr::sign(deltaf.x())*std::min( abs(deltaf.x()/stiffness.x()), wall_max_vel),
// 			stiffness.y()==0 ? Mathr::sign(deltaf.y())*wall_max_vel : Mathr::sign(deltaf.y())*std::min( abs(deltaf.y()/stiffness.y()), wall_max_vel),
// 			stiffness.z()==0 ? Mathr::sign(deltaf.z())*wall_max_vel : Mathr::sign(deltaf.z())*std::min( abs(deltaf.z()/stiffness.z()), wall_max_vel) );
			
		previoustranslation[wall] = (1-damping)*translation*normal[wall] + 0.7*previoustranslation[wall];// formula for "steady-flow" evolution with fluctuations
		p->se3.position	+= previoustranslation[wall];
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
	
	{
		height = p_top->se3.position.y() - p_bottom->se3.position.y() - thickness;
		width = p_right->se3.position.x() - p_left->se3.position.x() - thickness;
		depth = p_front->se3.position.z() - p_back->se3.position.z() - thickness;
				
		
		Vector3r wallForce (0, sigma_iso*width*depth, 0);
		if (wall_bottom_activated)
		controlStress(wall_bottom, ncb, wall_bottom_id, -wallForce, p_bottom, max_vel);
		if (wall_top_activated)
		controlStress(wall_top, ncb, wall_top_id, wallForce, p_top, max_vel);
		
		wallForce = Vector3r(sigma_iso*height*depth, 0, 0);
		if (wall_left_activated)
		controlStress(wall_left, ncb, wall_left_id, -wallForce, p_left, max_vel*width/height);
		if (wall_right_activated)
		controlStress(wall_right, ncb, wall_right_id, wallForce, p_right, max_vel*width/height);
		
		wallForce = Vector3r(0, 0, sigma_iso*height*depth);
		if (wall_back_activated)
		controlStress(wall_back, ncb, wall_back_id, -wallForce, p_back, max_vel*depth/height);
		if (wall_front_activated)
		controlStress(wall_front, ncb, wall_front_id, wallForce, p_front, max_vel*depth/height);
					
	//}
	}
}


