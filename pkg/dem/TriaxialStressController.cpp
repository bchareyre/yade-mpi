/*************************************************************************
*  Copyright (C) 2006 by Bruno Chareyre				 	 *
*  bruno.chareyre@hmg.inpg.fr					  	 *
*									 *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"TriaxialStressController.hpp"
#include<pkg/common/Sphere.hpp>
#include<pkg/common/Box.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/dem/FrictPhys.hpp>
#include<core/State.hpp>
#include<assert.h>
#include<core/Scene.hpp>
#include<pkg/dem/Shop.hpp>
#include<core/Clump.hpp>

#ifdef FLOW_ENGINE
//#include<pkg/pfv/FlowEngine.hpp>
#include "FlowEngine_FlowEngineT.hpp"
#endif

CREATE_LOGGER(TriaxialStressController);
YADE_PLUGIN((TriaxialStressController));

TriaxialStressController::~TriaxialStressController(){}

Vector3r TriaxialStressController::getStress(int boundId) {assert (boundId>=0 && boundId<=5); return stress[boundId];}

Vector3r TriaxialStressController::getStrainRate() {
	return Vector3r (
		(Body::byId(wall_right_id,scene)->state->vel[0]-Body::byId(wall_left_id,scene)->state->vel[0])/width,
		(Body::byId(wall_top_id,scene)->state->vel[1]-Body::byId(wall_bottom_id,scene)->state->vel[1])/height,
		(Body::byId(wall_front_id,scene)->state->vel[2]-Body::byId(wall_back_id,scene)->state->vel[2])/depth
	);
}

void TriaxialStressController::updateStiffness() {
	Real fluidStiffness = 0.;
	#ifdef FLOW_ENGINE
	FOREACH(const shared_ptr<Engine> e, Omega::instance().getScene()->engines) {
		if (e->getClassName() == "FlowEngine") {
			TemplateFlowEngine_FlowEngineT<FlowCellInfo_FlowEngineT,FlowVertexInfo_FlowEngineT>* flow = 
			dynamic_cast<TemplateFlowEngine_FlowEngineT<FlowCellInfo_FlowEngineT,FlowVertexInfo_FlowEngineT>*>(e.get());
			if ( (flow->fluidBulkModulus > 0) && (!(flow->dead)) ) fluidStiffness = flow->fluidBulkModulus/porosity;
		}
	}
	#endif
	for (int i=0; i<6; ++i) stiffness[i] = 0;
	InteractionContainer::iterator ii    = scene->interactions->begin();
	InteractionContainer::iterator iiEnd = scene->interactions->end();
	for(  ; ii!=iiEnd ; ++ii ) if ((*ii)->isReal())
	{
		const shared_ptr<Interaction>& contact = *ii;
		Real fn = (static_cast<FrictPhys*>	(contact->phys.get()))->normalForce.norm();
		if (fn!=0)
		{
			int id1 = contact->getId1(), id2 = contact->getId2();
			for (int index=0; index<6; ++index) if ( wall_id[index]==id1 || wall_id[index]==id2 )
			{
				FrictPhys* currentContactPhysics = static_cast<FrictPhys*> ( contact->phys.get() );
				stiffness[index] += currentContactPhysics->kn;
			}
		}
	}
	if (fluidStiffness > 0) {
		stiffness[0] += fluidStiffness*width*depth/height;
		stiffness[1] += fluidStiffness*width*depth/height;
		stiffness[2] += fluidStiffness*height*depth/width;
		stiffness[3] += fluidStiffness*height*depth/width;
		stiffness[4] += fluidStiffness*width*height/depth;
		stiffness[5] += fluidStiffness*width*height/depth;
	}
}

void TriaxialStressController::controlExternalStress(int wall, Vector3r resultantForce, State* p, Real wall_max_vel) // controls walls such that Sum Forces from Sample on Wall = resultantForce
{
	scene->forces.sync();
	Real translation=normal[wall].dot(getForce(scene,wall_id[wall])-resultantForce);
	const bool log=false;
	if(log) LOG_DEBUG("wall="<<wall<<" actualForce="<<getForce(scene,wall_id[wall])<<", resultantForce="<<resultantForce<<", translation="<<translation);
	if (translation!=0)
	{
	   if (stiffness[wall]!=0)
	   {
			translation /= stiffness[wall];
			if(log) TRVAR2(translation,wall_max_vel*scene->dt)
			translation = std::min( std::abs(translation), wall_max_vel*scene->dt ) * Mathr::Sign(translation);
	   }
	   else translation = wall_max_vel * Mathr::Sign(translation)*scene->dt;
	}
	previousTranslation[wall] = (1-stressDamping)*translation*normal[wall] + 0.8*previousTranslation[wall];// formula for "steady-flow" evolution with fluctuations
	//Don't update position since Newton is doing that starting from bzr2612
// 	p->se3.position += previousTranslation[wall];
	externalWork += previousTranslation[wall].dot(getForce(scene,wall_id[wall]));
	// this is important is using VelocityBins. Otherwise the motion is never detected. Related to https://bugs.launchpad.net/yade/+bug/398089
	p->vel=previousTranslation[wall]/scene->dt;
	//if(log)TRVAR2(previousTranslation,p->se3.position);
}

void TriaxialStressController::action()
{
	// sync thread storage of ForceContainer
	scene->forces.sync();
	if (first) {// sync boundaries ids in the table
		wall_id[wall_bottom] = wall_bottom_id;
 		wall_id[wall_top] = wall_top_id;
 		wall_id[wall_left] = wall_left_id;
 		wall_id[wall_right] = wall_right_id;
 		wall_id[wall_front] = wall_front_id;
 		wall_id[wall_back] = wall_back_id;}

	if(thickness<0) thickness=2.0*YADE_PTR_CAST<Box>(Body::byId(wall_bottom_id,scene)->shape)->extents.y();
	State* p_bottom=Body::byId(wall_bottom_id,scene)->state.get();
	State* p_top=Body::byId(wall_top_id,scene)->state.get();
	State* p_left=Body::byId(wall_left_id,scene)->state.get();
	State* p_right=Body::byId(wall_right_id,scene)->state.get();
	State* p_front=Body::byId(wall_front_id,scene)->state.get();
	State* p_back=Body::byId(wall_back_id,scene)->state.get();
	height = p_top->se3.position.y() - p_bottom->se3.position.y() - thickness;
	width = p_right->se3.position.x() - p_left->se3.position.x() - thickness;
	depth = p_front->se3.position.z() - p_back->se3.position.z() - thickness;

	boxVolume = height * width * depth;
	if ( (first) || (updatePorosity) ) {
		BodyContainer::iterator bi = scene->bodies->begin();
		BodyContainer::iterator biEnd = scene->bodies->end();

		particlesVolume = 0;
		for ( ; bi!=biEnd; ++bi ) {
			const shared_ptr<Body>& b = *bi;
			if (b->isClump()) {
				const shared_ptr<Clump>& clump = YADE_PTR_CAST<Clump>(b->shape);
				const shared_ptr<Body>& member = Body::byId(clump->members.begin()->first,scene);
				particlesVolume += b->state->mass / member->material->density;
			}
			else if (b->isDynamic() && !b->isClumpMember()) {
				const shared_ptr<Sphere>& sphere = YADE_PTR_CAST<Sphere> ( b->shape );
				particlesVolume += 1.3333333*Mathr::PI*pow ( sphere->radius, 3 );
			}
		}
		first = false;
		updatePorosity = false;
	}
	max_vel1=3 * width /(height+width+depth)*max_vel;
	max_vel2=3 * height /(height+width+depth)*max_vel;
	max_vel3 =3 * depth /(height+width+depth)*max_vel;

	porosity = ( boxVolume - particlesVolume ) /boxVolume;
	position_top = p_top->se3.position.y();
	position_bottom = p_bottom->se3.position.y();
	position_right = p_right->se3.position.x();
	position_left = p_left->se3.position.x();
	position_front = p_front->se3.position.z();
	position_back = p_back->se3.position.z();

	// must be done _after_ height, width, depth have been calculated
	//Update stiffness only if it has been computed by StiffnessCounter (see "stiffnessUpdateInterval")
	if (scene->iter % stiffnessUpdateInterval == 0 || scene->iter<100) updateStiffness();
	bool isARadiusControlIteration = (scene->iter % radiusControlInterval == 0);

	if (scene->iter % computeStressStrainInterval == 0 ||
		 (internalCompaction && isARadiusControlIteration) )
		computeStressStrain();

	if (!internalCompaction) {
		Vector3r wallForce (0, goal2*width*depth, 0);
		if (wall_bottom_activated) {
			if (stressMask & 2)  controlExternalStress(wall_bottom, wallForce, p_bottom, max_vel2);
			else {
				p_bottom->vel[1] += (-normal[wall_bottom][1]*0.5*goal2*height -p_bottom->vel[1])*(1-strainDamping);
				externalWork += p_bottom->vel.dot(getForce(scene,wall_bottom_id))*scene->dt;}
		} else p_bottom->vel=Vector3r::Zero();
		if (wall_top_activated) {
			if (stressMask & 2)  controlExternalStress(wall_top, -wallForce, p_top, max_vel2);
			else {
				p_top->vel[1] += (-normal[wall_top][1]*0.5*goal2*height -p_top->vel[1])*(1-strainDamping);
				externalWork += p_top->vel.dot(getForce(scene,wall_top_id))*scene->dt;}
		} else p_top->vel=Vector3r::Zero();
		
		wallForce = Vector3r(goal1*height*depth, 0, 0);
		if (wall_left_activated) {
			if (stressMask & 1) controlExternalStress(wall_left, wallForce, p_left, max_vel1);
			else {
				p_left->vel[0] += (-normal[wall_left][0]*0.5*goal1*width -p_left->vel[0])*(1-strainDamping);
				externalWork += p_left->vel.dot(getForce(scene,wall_left_id))*scene->dt;}
		} else p_left->vel=Vector3r::Zero();
		if (wall_right_activated) {
			if (stressMask & 1) controlExternalStress(wall_right, -wallForce, p_right, max_vel1);
			else {
				p_right->vel[0] += (-normal[wall_right][0]*0.5*goal1*width -p_right->vel[0])*(1-strainDamping);
				externalWork += p_right->vel.dot(getForce(scene,wall_right_id))*scene->dt;}
		} else p_right->vel=Vector3r::Zero();

		wallForce = Vector3r(0, 0, goal3*height*width);
		if (wall_back_activated) {
			if (stressMask & 4) controlExternalStress(wall_back, wallForce, p_back, max_vel3);
			else {
				p_back->vel[2] += (-normal[wall_back][2]*0.5*goal3*depth -p_back->vel[2])*(1-strainDamping);
				externalWork += p_back->vel.dot(getForce(scene,wall_back_id))*scene->dt;}
		} else p_back->vel=Vector3r::Zero();
		if (wall_front_activated) {
			if (stressMask & 4) controlExternalStress(wall_front, -wallForce, p_front, max_vel3);
			else {
				p_front->vel[2] += (-normal[wall_front][2]*0.5*goal3*depth -p_front->vel[2])*(1-strainDamping);
				externalWork += p_front->vel.dot(getForce(scene,wall_front_id))*scene->dt;}
		} else p_front->vel=Vector3r::Zero();
	}
	else //if internal compaction
	{
		p_bottom->vel=Vector3r::Zero(); p_top->vel=Vector3r::Zero(); p_left->vel=Vector3r::Zero(); p_right->vel=Vector3r::Zero(); p_back->vel=Vector3r::Zero(); p_front->vel=Vector3r::Zero();
		if (isARadiusControlIteration) {
			Real sigma_iso_ = bool(stressMask & 1)*goal1 +  bool(stressMask & 2)*goal2 +  bool(stressMask & 4)*goal3;
			sigma_iso_ /=  bool(stressMask & 1) +  bool(stressMask & 2) +  bool(stressMask & 4);
			if (std::abs(sigma_iso_)<=std::abs(meanStress)) maxMultiplier = finalMaxMultiplier;
			if (meanStress==0) previousMultiplier = maxMultiplier;
			else {
				//     		previousMultiplier = 1+0.7*(sigma_iso-s)*(previousMultiplier-1.f)/(s-previousStress); // = (Dsigma/apparentModulus)*0.7
				//     		previousMultiplier = std::max(2-maxMultiplier, std::min(previousMultiplier, maxMultiplier));
			  if (sigma_iso_ < 0) // compressive case: we have to increase radii if meanStress > sigma_iso_, considering that sigma_iso_ < 0. We end with the same expression as before sign change
			    previousMultiplier = 1.+(sigma_iso_-meanStress)/sigma_iso_*(maxMultiplier-1.); // = (Dsigma/apparentModulus)*0.7
			  else // tensile case: we have to increase radii if meanStress > sigma_iso_ too. But here sigma_iso_ > 0 => another expression
			    previousMultiplier = 1.+(meanStress-sigma_iso_)/sigma_iso_*(maxMultiplier-1.); // = (Dsigma/apparentModulus)*0.7
			}
			previousStress = meanStress;
			//Real apparentModulus = (s-previousStress)/(previousMultiplier-1.f);
			controlInternalStress(previousMultiplier);
		}
	}
}

void TriaxialStressController::computeStressStrain()
{
	scene->forces.sync();
	State* p_bottom=Body::byId(wall_bottom_id,scene)->state.get();
	State* p_top=Body::byId(wall_top_id,scene)->state.get();
	State* p_left=Body::byId(wall_left_id,scene)->state.get();
	State* p_right=Body::byId(wall_right_id,scene)->state.get();
	State* p_front=Body::byId(wall_front_id,scene)->state.get();
	State* p_back=Body::byId(wall_back_id,scene)->state.get();

 	height = p_top->se3.position.y() - p_bottom->se3.position.y() - thickness;
 	width = p_right->se3.position.x() - p_left->se3.position.x() - thickness;
 	depth = p_front->se3.position.z() - p_back->se3.position.z() - thickness;

	meanStress = 0;
	if (height0 == 0) height0 = height;
	if (width0 == 0) width0 = width;
	if (depth0 == 0) depth0 = depth;
	strain[0] = log(width/width0); // all strain values are positiv for extension
	strain[1] = log(height/height0);
	strain[2] = log(depth/depth0);
	volumetricStrain=strain[0]+strain[1]+strain[2];

	Real invXSurface = 1.f/(height*depth);
	Real invYSurface = 1.f/(width*depth);
	Real invZSurface = 1.f/(width*height);

 	force[wall_bottom]=getForce(scene,wall_id[wall_bottom]); stress[wall_bottom]=force[wall_bottom]*invYSurface; // all stress values are positiv for tension
	force[wall_top]=   getForce(scene,wall_id[wall_top]);    stress[wall_top]=-force[wall_top]*invYSurface;
	force[wall_left]=  getForce(scene,wall_id[wall_left]);   stress[wall_left]=force[wall_left]*invXSurface;
	force[wall_right]= getForce(scene,wall_id[wall_right]);  stress[wall_right]= -force[wall_right]*invXSurface;
	force[wall_front]= getForce(scene,wall_id[wall_front]);  stress[wall_front]=-force[wall_front]*invZSurface;
        force[wall_back]=  getForce(scene,wall_id[wall_back]);   stress[wall_back]= force[wall_back]*invZSurface;

	for (int i=0; i<6; i++) meanStress+=stress[i].dot(pow(-1.0,i)*normal[i]); // normal[i] is always inwards
	meanStress/=6.; // ( sXX(xLeft) + sXX(xRight) + sYY(yBottom) + sYY(yTop) + sZZ(zBack) + sZZ(zFront) ) / 6
}

void TriaxialStressController::controlInternalStress ( Real multiplier )
{
	particlesVolume *= pow ( multiplier,3 );
	Shop::growParticles(multiplier,true,true);
}

/*!
    \fn TriaxialStressController::ComputeUnbalancedForce( bool maxUnbalanced)
 */
Real TriaxialStressController::ComputeUnbalancedForce( bool maxUnbalanced) {return Shop::unbalancedForce(maxUnbalanced,scene);}


