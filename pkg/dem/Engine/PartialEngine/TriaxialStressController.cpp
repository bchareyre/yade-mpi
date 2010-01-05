/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre				 	 *
*  bruno.chareyre@hmg.inpg.fr					  	 *
*									 *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
 
#include"TriaxialStressController.hpp"
#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Box.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-dem/ElasticContactInteraction.hpp>
#include<yade/core/State.hpp>



#include<yade/core/Scene.hpp>
#ifdef YADE_GEOMETRICALMODEL
	#include<yade/pkg-common/SphereModel.hpp>
#endif

CREATE_LOGGER(TriaxialStressController);

TriaxialStressController::TriaxialStressController(): wall_bottom_id(wall_id[0]), wall_top_id(wall_id[1]), wall_left_id(wall_id[2]), wall_right_id(wall_id[3]), wall_front_id(wall_id[4]), wall_back_id(wall_id[5])
{
	firstRun = true;
	
	previousStress = 0;
	previousMultiplier = 1;
		
	stiffnessUpdateInterval =10;
	radiusControlInterval =10;
	computeStressStrainInterval = 10;
	wallDamping = 0.25;
	//force = Vector3r::ZERO;
	stiffness.resize(6);
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
	max_vel1 = 0.001;
	max_vel2 = 0.001;
	max_vel3 = 0.001;


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
	spheresVolume=0;
	boxVolume=0;
	porosity=1;
	height0 = 0;
	width0 = 0;
	depth0 = 0;
	thickness = -1;
	
	//UnbalancedForce = 0;
	
	sigma_iso = 0;
	sigma1 = 0;
	sigma2 = 0;
	sigma3 = 0;
	isAxisymetric = true;
}

TriaxialStressController::~TriaxialStressController()
{	
}


void TriaxialStressController::updateStiffness (Scene * ncb)
{
	for (int i=0; i<6; ++i) stiffness[i] = 0;

	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end();
	for(  ; ii!=iiEnd ; ++ii )
	{
		if ((*ii)->isReal())
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

void TriaxialStressController::controlExternalStress(int wall, Scene* ncb, Vector3r resultantForce, State* p, Real wall_max_vel)
{
	Real translation=normal[wall].Dot( getForce(ncb,wall_id[wall]) - resultantForce); 
	//bool log=((wall==3) && (Omega::instance().getCurrentIteration()%200==0));
	const bool log=false;
	if(log) LOG_DEBUG("wall="<<wall<<" actualForce="<<getForce(ncb,wall_id[wall])<<", resultantForce="<<resultantForce<<", translation="<<translation);

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
	//previousTranslation[wall] = (1-wallDamping)*translation*normal[wall];// + 0.7*previousTranslation[wall];// formula for "steady-flow" evolution with fluctuations
	previousTranslation[wall] = (1-wallDamping)*translation*normal[wall] + 0.8*previousTranslation[wall];// formula for "steady-flow" evolution with fluctuations
	//cerr << "translation = " << previousTranslation[wall] << endl;
	p->se3.position += previousTranslation[wall];
	// this is important is using VelocityBins. Otherwise the motion is never detected. Related to https://bugs.launchpad.net/yade/+bug/398089
	p->vel=previousTranslation[wall]/ncb->dt;
	if(log)TRVAR2(previousTranslation,p->se3.position);
}




void TriaxialStressController::applyCondition(Scene* ncb)
{
	//cerr << "TriaxialStressController::applyCondition" << endl;

	// sync thread storage of ForceContainer
	ncb->forces.sync(); 
	
	if(thickness<0) thickness=YADE_PTR_CAST<Box>(Body::byId(wall_bottom_id,ncb)->shape)->extents.Y();

	State* p_bottom=Body::byId(wall_bottom_id,ncb)->state.get();
	State* p_top=Body::byId(wall_top_id,ncb)->state.get();
	State* p_left=Body::byId(wall_left_id,ncb)->state.get();
	State* p_right=Body::byId(wall_right_id,ncb)->state.get();
	State* p_front=Body::byId(wall_front_id,ncb)->state.get();
	State* p_back=Body::byId(wall_back_id,ncb)->state.get();
		
	height = p_top->se3.position.Y() - p_bottom->se3.position.Y() - thickness;
	width = p_right->se3.position.X() - p_left->se3.position.X() - thickness;
	depth = p_front->se3.position.Z() - p_back->se3.position.Z() - thickness;
	
	boxVolume = height * width * depth;
	
	if (firstRun) {
		BodyContainer::iterator bi = ncb->bodies->begin();
		BodyContainer::iterator biEnd = ncb->bodies->end();
		spheresVolume = 0;
		for ( ; bi!=biEnd; ++bi )
		{
			const shared_ptr<Body>& b = *bi;
			if ( b->isDynamic )
			{
				const shared_ptr<Sphere>& sphere =
						YADE_PTR_CAST<Sphere> ( b->shape );
				spheresVolume += 1.3333333*Mathr::PI*pow ( sphere->radius, 3 );
			}
		}
		max_vel1=3 * width /(depth+width+depth)*max_vel;				
		max_vel2=3 * height /(depth+width+depth)*max_vel;
		max_vel3 =3 * depth /(depth+width+depth)*max_vel;
		firstRun = false;
	}

	// NOT JUST at the first run, since sigma_iso may be changed
	// if the TriaxialCompressionEngine is used, sigma_iso is attributed to sigma1, sigma2 and sigma3
	if (isAxisymetric){
		sigma1=sigma2=sigma3=sigma_iso;		
	}


	porosity = ( boxVolume - spheresVolume ) /boxVolume;

	position_top = p_top->se3.position.Y();
	position_bottom = p_bottom->se3.position.Y();
	position_right = p_right->se3.position.X();
	position_left = p_left->se3.position.X();
	position_front = p_front->se3.position.Z();
	position_back = p_back->se3.position.Z();



	// must be done _after_ height, width, depth have been calculated
	//Update stiffness only if it has been computed by StiffnessCounter (see "stiffnessUpdateInterval")
	if (Omega::instance().getCurrentIteration() % stiffnessUpdateInterval == 0 || Omega::instance().getCurrentIteration()<100) updateStiffness(ncb);

 
		bool isARadiusControlIteration = (Omega::instance().getCurrentIteration() % radiusControlInterval == 0);
	if (Omega::instance().getCurrentIteration() % computeStressStrainInterval == 0 ||
		 (internalCompaction && isARadiusControlIteration) )
		computeStressStrain(ncb);

	if (!internalCompaction) {
		Vector3r wallForce (0, sigma2*width*depth, 0);
		if (wall_bottom_activated) controlExternalStress(wall_bottom, ncb, -wallForce, p_bottom, max_vel2);
		if (wall_top_activated) controlExternalStress(wall_top, ncb, wallForce, p_top, max_vel2);
		
		wallForce = Vector3r(sigma1*height*depth, 0, 0);
		if (wall_left_activated) controlExternalStress(wall_left, ncb, -wallForce, p_left, max_vel1);
		if (wall_right_activated) controlExternalStress(wall_right, ncb, wallForce, p_right, max_vel1);
		
		wallForce = Vector3r(0, 0, sigma3*height*width);
		if (wall_back_activated) controlExternalStress(wall_back, ncb, -wallForce, p_back, max_vel3);
		if (wall_front_activated) controlExternalStress(wall_front, ncb, wallForce, p_front, max_vel3);
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

/* Compute:
 *
 * # strain[3] along x,y,z axes, 
 * # volumetricStrain (ε_v=ε_x+ε_y+ε_z)
 * # stress[6] on all walls
 * # meanStress
 *
 * (This function used to return meanStress, but since the return value was not used anywhere, it now returns void)
 */
void TriaxialStressController::computeStressStrain(Scene* ncb)
{
	State* p_bottom=Body::byId(wall_bottom_id,ncb)->state.get();
	State* p_top=Body::byId(wall_top_id,ncb)->state.get();
	State* p_left=Body::byId(wall_left_id,ncb)->state.get();
	State* p_right=Body::byId(wall_right_id,ncb)->state.get();
	State* p_front=Body::byId(wall_front_id,ncb)->state.get();
	State* p_back=Body::byId(wall_back_id,ncb)->state.get();
	
 	height = p_top->se3.position.Y() - p_bottom->se3.position.Y() - thickness;
 	width = p_right->se3.position.X() - p_left->se3.position.X() - thickness;
 	depth = p_front->se3.position.Z() - p_back->se3.position.Z() - thickness;
	//assert(height>0); assert(width>0); assert(depth>0);
	
	meanStress = 0;
	if (height0 == 0) height0 = height;
	if (width0 == 0) width0 = width;
	if (depth0 == 0) depth0 = depth;
	strain[0] = Mathr::Log(width0/width);
	strain[1] = Mathr::Log(height0/height);
	strain[2] = Mathr::Log(depth0/depth);

	volumetricStrain=strain[0]+strain[1]+strain[2];
	
	Real invXSurface = 1.f/(height*depth);
	Real invYSurface = 1.f/(width*depth);
	Real invZSurface = 1.f/(width*height);

 	force[wall_bottom]=getForce(ncb,wall_id[wall_bottom]); stress[wall_bottom]=force[wall_bottom]*invYSurface;
	force[wall_top]=   getForce(ncb,wall_id[wall_top]);    stress[wall_top]=force[wall_top]*invYSurface;
	force[wall_left]=  getForce(ncb,wall_id[wall_left]);   stress[wall_left]=force[wall_left]*invXSurface;
	force[wall_right]= getForce(ncb,wall_id[wall_right]);  stress[wall_right]= force[wall_right]*invXSurface;
	force[wall_front]= getForce(ncb,wall_id[wall_front]);  stress[wall_front]=force[wall_front]*invZSurface;
        force[wall_back]=  getForce(ncb,wall_id[wall_back]);   stress[wall_back]= force[wall_back]*invZSurface;


 	//cerr << "stresses : " <<  stress[wall_bottom] << " " <<
 //stress[wall_top]<< " " << stress[wall_left]<< " " << stress[wall_right]<< " "
 //<< stress[wall_front] << " " << stress[wall_back] << endl;

	for (int i=0; i<6; i++) meanStress-=stress[i].Dot(normal[i]);
	meanStress/=6.;
}

void TriaxialStressController::controlInternalStress ( Scene* ncb, Real multiplier )
{
	spheresVolume *= pow ( multiplier,3 );
	BodyContainer::iterator bi    = ncb->bodies->begin();
	BodyContainer::iterator biEnd = ncb->bodies->end();
	//cerr << "meanstress = "radius = " << endl;
	//cerr << "bouclesurBodies" << endl;
	for ( ; bi!=biEnd ; ++bi )
	{
		if ( ( *bi )->isDynamic )
		{
			( static_cast<Sphere*> ( ( *bi )->shape.get() ) )->radius *= multiplier;
			//( static_cast<SphereModel*> ( ( *bi )->geometricalModel.get() ) )->radius *= multiplier;
			#ifdef YADE_GEOMETRICALMODEL
				SphereModel* s = dynamic_cast<SphereModel*> ( ( *bi )->geometricalModel.get() ); if(s) s->radius *= multiplier;
			#endif
				(*bi)->state->mass*=pow(multiplier,3);
				(*bi)->state->inertia*=pow(multiplier,5);

		}
	}
	// << "bouclesurInteraction" << endl;
	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end();
	for ( ; ii!=iiEnd ; ++ii )
	{
		if ( ( *ii )->isReal() )
		{
			ScGeom* contact = static_cast<ScGeom*> ( ( *ii )->interactionGeometry.get() );
			//      if ((*(ncb->bodies))[(*ii)->getId1()]->isDynamic)
			//   contact->radius1 *= multiplier;
			//      if ((*(ncb->bodies))[(*ii)->getId2()]->isDynamic)
			//   contact->radius2 *= multiplier;
			if ( ( * ( ncb->bodies ) ) [ ( *ii )->getId1() ]->isDynamic )
				contact->radius1 = static_cast<Sphere*> ( ( * ( ncb->bodies ) ) [ ( *ii )->getId1() ]->shape.get() )->radius;
			if ( ( * ( ncb->bodies ) ) [ ( *ii )->getId2() ]->isDynamic )
				contact->radius2 = static_cast<Sphere*> ( ( * ( ncb->bodies ) ) [ ( *ii )->getId2() ]->shape.get() )->radius;
		}
	}
}

/*!
    \fn TriaxialStressController::ComputeUnbalancedForce(Scene * ncb, bool maxUnbalanced)
 */
Real TriaxialStressController::ComputeUnbalancedForce(Scene * ncb, bool maxUnbalanced)
{
	ncb->forces.sync();
	//compute the mean contact force
	Real MeanForce = 0.f;
	long nForce = 0;

	shared_ptr<BodyContainer>& bodies = ncb->bodies;

	InteractionContainer::iterator ii    = ncb->interactions->begin();
	InteractionContainer::iterator iiEnd = ncb->interactions->end();
	for(  ; ii!=iiEnd ; ++ii ) {
		if ((*ii)->isReal()) {
			const shared_ptr<Interaction>& contact = *ii;
			Real f = (static_cast<ElasticContactInteraction*> ((contact->interactionPhysics.get()))->normalForce+static_cast<ElasticContactInteraction*>(contact->interactionPhysics.get())->shearForce).SquaredLength();
			if (f!=0)
			{
			MeanForce += Mathr::Sqrt(f);
			++nForce;
			}
		}
	}
	if (nForce!=0) MeanForce /= nForce;


	if (!maxUnbalanced) {
		//compute mean Unbalanced Force
		Real MeanUnbalanced=0;
		long nBodies = 0;
		BodyContainer::iterator bi    = bodies->begin();
		BodyContainer::iterator biEnd = bodies->end();
		Real f;
		for(  ; bi!=biEnd ; ++bi ) {
			if ((*bi)->isDynamic) {
				f=getForce(ncb,(*bi)->getId()).Length();
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
				MaxUnbalanced = std::max(getForce(ncb,(*bi)->getId()).Length(),MaxUnbalanced);

			}
		}
		if (MeanForce != 0) MaxUnbalanced = MaxUnbalanced/MeanForce;
		return MaxUnbalanced;
	}
}

YADE_PLUGIN((TriaxialStressController));
