/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre				  *
*  bruno.chareyre@hmg.inpg.fr					    *
*									*
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"TriaxialStressController.hpp"
#include<yade/pkg-common/ParticleParameters.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/pkg-common/Force.hpp>



#include<yade/core/MetaBody.hpp>
#include<yade/pkg-common/Sphere.hpp>

CREATE_LOGGER(TriaxialStressController);

TriaxialStressController::TriaxialStressController(): wall_bottom_id(wall_id[0]), wall_top_id(wall_id[1]), wall_left_id(wall_id[2]), wall_right_id(wall_id[3]), wall_front_id(wall_id[4]), wall_back_id(wall_id[5])
{
	//StiffnessMatrixClassIndex = actionParameterStiffnessMatrix->getClassIndex();
	shared_ptr<Force> tmpF(new Force);
	ForceClassIndex=tmpF->getClassIndex();

	previousStress = 0;
	previousMultiplier = 1;
	
	stiffnessUpdateInterval =10;
	radiusControlInterval =10;
	computeStressStrainInterval = 10;
	wallDamping = 0.25;
	//force = Vector3r::ZERO;
	for (int i=0; i<6; ++i)
	{
		wall_id[i] = 0;
		previousTranslation[i] = Vector3r::ZERO;
		stiffness[i] = 0;
		normal[i] = Vector3r::ZERO;
	}
	meanStress = 0;
	for (int i=0; i<3; ++i) 	strain[i] = 0;
	normal[wall_bottom].Y()=1;
	normal[wall_top].Y()=-1;
	normal[wall_left].X()=1;
	normal[wall_right].X()=-1;
	normal[wall_front].Z()=-1;
	normal[wall_back].Z()=1;
	
	//stiffness = Vector3r::ZERO;
	max_vel = 0.001;
	maxMultiplier = 1.001;
	finalMaxMultiplier = 1.00001;
	internalCompaction = true;
	
	wall_bottom_activated = true;
	wall_top_activated = true;
	wall_left_activated = true;
	wall_right_activated = true;
	wall_front_activated = true;
	wall_back_activated = true;
	
	height = 0;
	width = 0;
	depth = 0;
	height0 = 0;
	width0 = 0;
	depth0 = 0;
	thickness = 0;
	
	//UnbalancedForce = 0;
	
	sigma_iso = 0;
}

TriaxialStressController::~TriaxialStressController()
{	
}

void TriaxialStressController::registerAttributes()
{
	DeusExMachina::registerAttributes();
	REGISTER_ATTRIBUTE(stiffnessUpdateInterval);
	REGISTER_ATTRIBUTE(radiusControlInterval);
	REGISTER_ATTRIBUTE(computeStressStrainInterval);
	REGISTER_ATTRIBUTE(wallDamping);
//	REGISTER_ATTRIBUTE(force);
	
	//REGISTER_ATTRIBUTE(UnbalancedForce);
	
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
	REGISTER_ATTRIBUTE(height0);
	REGISTER_ATTRIBUTE(width0);
	REGISTER_ATTRIBUTE(depth0);
	REGISTER_ATTRIBUTE(thickness);
	
	
	REGISTER_ATTRIBUTE(sigma_iso);
	REGISTER_ATTRIBUTE(maxMultiplier);
	REGISTER_ATTRIBUTE(finalMaxMultiplier);
	REGISTER_ATTRIBUTE(max_vel);
	REGISTER_ATTRIBUTE(previousStress);
	REGISTER_ATTRIBUTE(previousMultiplier);
	REGISTER_ATTRIBUTE(internalCompaction);
}

void TriaxialStressController::updateStiffness (MetaBody * ncb)
{
	for (int i=0; i<6; ++i) stiffness[i] = 0;

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			const shared_ptr<Interaction>& contact = *ii;
			
			Real fn = (static_cast<ElasticContactInteraction*>	(contact->interactionPhysics.get()))->normalForce.Length();

			if (fn!=0)
			{
				int id1 = contact->getId1(), id2 = contact->getId2();
				
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
}

void TriaxialStressController::controlExternalStress(int wall, MetaBody* ncb, Vector3r resultantForce, PhysicalParameters* p, Real wall_max_vel)
{
	Real translation=normal[wall].Dot(static_cast<Force*>( ncb->physicalActions->find(wall_id[wall],ForceClassIndex).get() )->force - resultantForce);
	//bool log=((wall==3) && (Omega::instance().getCurrentIteration()%200==0));
	const bool log=false;
	if(log) LOG_DEBUG("wall="<<wall<<" actualForce="<<static_cast<Force*>(ncb->physicalActions->find(wall_id[wall],ForceClassIndex).get() )->force<<", resultantForce="<<resultantForce<<", translation="<<translation);
	//cerr << "wall="<<wall<<" actualForce="<<static_cast<Force*>(ncb->physicalActions->find(wall_id[wall],ForceClassIndex).get() )->force<<", resultantForce="<<resultantForce<<", deltaF="<<translation << endl;
	if (translation!=0)
	{
	//cerr << "stiffness = " << stiffness[wall];
	   if (stiffness[wall]!=0)
	   {
			translation /= stiffness[wall];
			if(log) TRVAR2(translation,wall_max_vel*Omega::instance().getTimeStep())
			translation = std::min( std::abs(translation), wall_max_vel*Omega::instance().getTimeStep() ) * Mathr::Sign(translation);
	   }
	   else
		translation = wall_max_vel * Mathr::Sign(translation)*Omega::instance().getTimeStep();
	}
	previousTranslation[wall] = (1-wallDamping)*translation*normal[wall];// + 0.7*previousTranslation[wall];// formula for "steady-flow" evolution with fluctuations
	//cerr << "translation = " << previousTranslation[wall] << endl;
	p->se3.position += previousTranslation[wall];
	if(log)TRVAR2(previousTranslation,p->se3.position);
}




void TriaxialStressController::applyCondition(MetaBody* ncb)
{
	//cerr << "TriaxialStressController::applyCondition" << endl;

	//Update stiffness only if it has been computed by StiffnessCounter (see "stiffnessUpdateInterval")
	if (Omega::instance().getCurrentIteration() % stiffnessUpdateInterval == 0 || Omega::instance().getCurrentIteration()<1000)
		updateStiffness(ncb);
		
	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	PhysicalParameters* p_bottom = static_cast<PhysicalParameters*>((*bodies)[wall_bottom_id]->physicalParameters.get());
	PhysicalParameters* p_top   =	 static_cast<PhysicalParameters*>((*bodies)[wall_top_id]->physicalParameters.get());
	PhysicalParameters* p_left  = static_cast<PhysicalParameters*>((*bodies)[wall_left_id]->physicalParameters.get());
	PhysicalParameters* p_right = static_cast<PhysicalParameters*>((*bodies)[wall_right_id]->physicalParameters.get());
	PhysicalParameters* p_front = static_cast<PhysicalParameters*>((*bodies)[wall_front_id]->physicalParameters.get());
	PhysicalParameters* p_back  = static_cast<PhysicalParameters*>((*bodies)[wall_back_id]->physicalParameters.get());
		
	height = p_top->se3.position.Y() - p_bottom->se3.position.Y() - thickness;
	width = p_right->se3.position.X() - p_left->se3.position.X() - thickness;
	depth = p_front->se3.position.Z() - p_back->se3.position.Z() - thickness;
 
		bool isARadiusControlIteration = (Omega::instance().getCurrentIteration() % radiusControlInterval == 0);
	if (Omega::instance().getCurrentIteration() % computeStressStrainInterval == 0 ||
		 (internalCompaction && isARadiusControlIteration) )
		computeStressStrain(ncb);

	if (!internalCompaction) {
		Vector3r wallForce (0, sigma_iso*width*depth, 0);
		if (wall_bottom_activated) controlExternalStress(wall_bottom, ncb, -wallForce, p_bottom, max_vel);
		if (wall_top_activated) controlExternalStress(wall_top, ncb, wallForce, p_top, max_vel);
		wallForce = Vector3r(sigma_iso*height*depth, 0, 0);
		if (wall_left_activated) controlExternalStress(wall_left, ncb, -wallForce, p_left, max_vel*width/height);
		if (wall_right_activated) controlExternalStress(wall_right, ncb, wallForce, p_right, max_vel*width/height);
		wallForce = Vector3r(0, 0, sigma_iso*height*width);
		if (wall_back_activated) controlExternalStress(wall_back, ncb, -wallForce, p_back, max_vel*depth/height);
		if (wall_front_activated) controlExternalStress(wall_front, ncb, wallForce, p_front, max_vel*depth/height);
	}
	else //if internal compaction
	{
		if (isARadiusControlIteration) {
			//Real s = computeStressStrain(ncb);
			if (sigma_iso<=meanStress) maxMultiplier = finalMaxMultiplier;
			if (meanStress==0) previousMultiplier = maxMultiplier;
			else {
				//     		previousMultiplier = 1+0.7*(sigma_iso-s)*(previousMultiplier-1.f)/(s-previousStress); // = (Dsigma/apparentModulus)*0.7
				//     		previousMultiplier = std::max(2-maxMultiplier, std::min(previousMultiplier, maxMultiplier));
				previousMultiplier = 1.+(sigma_iso-meanStress)/sigma_iso*(maxMultiplier-1.); // = (Dsigma/apparentModulus)*0.7
			}
			previousStress = meanStress;
			//Real apparentModulus = (s-previousStress)/(previousMultiplier-1.f);
			controlInternalStress(ncb, previousMultiplier);
		}
	}
}

Real TriaxialStressController::computeStressStrain(MetaBody* ncb)
{
	
// 	height = p_top->se3.position.Y() - p_bottom->se3.position.Y() - thickness;
// 	width = p_right->se3.position.X() - p_left->se3.position.X() - thickness;
// 	depth = p_front->se3.position.Z() - p_back->se3.position.Z() - thickness;
	
	meanStress = 0;
	if (height0 == 0) height0 = height;
	if (width0 == 0) width0 = width;
	if (depth0 == 0) depth0 = depth;
	strain[0] = Mathr::Log(width0/width);
	strain[1] = Mathr::Log(height0/height);
	strain[2] = Mathr::Log(depth0/depth);
	
	Real invXSurface = 1.f/(height*depth);
	Real invYSurface = 1.f/(width*depth);
	Real invZSurface = 1.f/(width*height);
		
	stress[wall_bottom] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_bottom],ForceClassIndex).get() )->force ) * invYSurface;
	stress[wall_top] = static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_top],ForceClassIndex).get() )->force * invYSurface;
	stress[wall_left] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_left],ForceClassIndex).get() )->force ) * invXSurface;
	stress[wall_right] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_right],ForceClassIndex).get() )->force ) * invXSurface;
	stress[wall_front] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_front],ForceClassIndex).get() )->force ) * invZSurface;
	stress[wall_back] = ( static_cast<Force*>( ncb->physicalActions->find(wall_id[wall_back],ForceClassIndex).get() )->force ) * invZSurface;	

 	//cerr << "stresses : " <<  stress[wall_bottom] << " " <<
 //stress[wall_top]<< " " << stress[wall_left]<< " " << stress[wall_right]<< " "
 //<< stress[wall_front] << " " << stress[wall_back] << endl;

	for (int i=0; i<6; i++) meanStress-=stress[i].Dot(normal[i]);
	meanStress/=6.;
	// FIXME: meanStress is both returned as function value and stored in meanStress member. Confusing, change prototype to void, since return value isn't used anywhere.
	return meanStress;
}

void TriaxialStressController::controlInternalStress(MetaBody* ncb, Real multiplier)
{
   BodyContainer::iterator bi    = ncb->bodies->begin();
   BodyContainer::iterator biEnd = ncb->bodies->end();
   //cerr << "meanstress = "radius = " << endl;
   //cerr << "bouclesurBodies" << endl;
   for (  ; bi!=biEnd ; ++bi )
   {
		if ((*bi)->isDynamic)
		{
			(static_cast<InteractingSphere*> ((*bi)->interactingGeometry.get()))->radius *= multiplier;
			(static_cast<Sphere*>((*bi)->geometricalModel.get()))->radius *= multiplier;
		}
	}
	// << "bouclesurInteraction" << endl;
	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for (  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal)
		{
			SpheresContactGeometry* contact = static_cast<SpheresContactGeometry*> ((*ii)->interactionGeometry.get());
			//	     if ((*(ncb->bodies))[(*ii)->getId1()]->isDynamic)
			//		 contact->radius1 *= multiplier;
			//	     if ((*(ncb->bodies))[(*ii)->getId2()]->isDynamic)
			//		 contact->radius2 *= multiplier;
			if ((*(ncb->bodies))[(*ii)->getId1()]->isDynamic)
				contact->radius1 = static_cast<InteractingSphere*> ((*(ncb->bodies))[(*ii)->getId1()]->interactingGeometry.get())->radius;
			if ((*(ncb->bodies))[(*ii)->getId2()]->isDynamic)
				contact->radius2 = static_cast<InteractingSphere*> ((*(ncb->bodies))[(*ii)->getId2()]->interactingGeometry.get())->radius;
		}
	}
}

/*!
    \fn TriaxialStressController::ComputeUnbalancedForce()
 */
Real TriaxialStressController::ComputeUnbalancedForce(MetaBody * ncb, bool maxUnbalanced)
{
	//compute the mean contact force
	Real MeanForce = 0.f;
	long nForce = 0;

	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	InteractionContainer::iterator ii    = ncb->transientInteractions->begin();
	InteractionContainer::iterator iiEnd = ncb->transientInteractions->end();
	for(  ; ii!=iiEnd ; ++ii ) {
		if ((*ii)->isReal) {
			const shared_ptr<Interaction>& contact = *ii;
			Real fn = (static_cast<ElasticContactInteraction*> ((contact->interactionPhysics.get()))->normalForce+static_cast<ElasticContactInteraction*>(contact->interactionPhysics.get())->shearForce).SquaredLength();
			if (fn!=0)
			{
			MeanForce += Mathr::Sqrt(fn);
			++nForce;
			}
		}
	}
	if (nForce!=0) MeanForce /= nForce;

//	int actionForceIndex = actionForce->getClassIndex();

	if (!maxUnbalanced) {
		//compute mean Unbalanced Force
		Real MeanUnbalanced=0;
		long nBodies = 0;
		BodyContainer::iterator bi    = bodies->begin();
		BodyContainer::iterator biEnd = bodies->end();
		Real f;
		for(  ; bi!=biEnd ; ++bi ) {
			if ((*bi)->isDynamic) {
				f= (static_cast<Force*>   ( ncb->physicalActions->find( (*bi)->getId() , ForceClassIndex).get() )->force).Length();
				MeanUnbalanced += f;
				if (f!=0) ++nBodies;
			}
		}
		if (nBodies != 0 && MeanForce != 0) MeanUnbalanced = MeanUnbalanced/nBodies/MeanForce;
		return  MeanUnbalanced;
	} else {
		//compute max Unbalanced Force
		Real MaxUnbalanced=0;
		BodyContainer::iterator bi    = bodies->begin();
		BodyContainer::iterator biEnd = bodies->end();
		for(  ; bi!=biEnd ; ++bi ) {
			if ((*bi)->isDynamic) {
				MaxUnbalanced = std::max((static_cast<Force*>   ( ncb->physicalActions->find( (*bi)->getId() , ForceClassIndex).get() )->force).Length(), MaxUnbalanced);
			}
		}
		if (MeanForce != 0) MaxUnbalanced = MaxUnbalanced/MeanForce;
		return MaxUnbalanced/MeanForce;
	}
}

YADE_PLUGIN();
