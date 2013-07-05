/*************************************************************************
*  Copyright (C) 2010 by Klaus Thoeni                                    *
*  klaus.thoeni@newcastle.edu.au                                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"WirePM.hpp"
#include<yade/core/Scene.hpp>
#include<yade/pkg/dem/ScGeom.hpp>
#include<yade/core/Omega.hpp>
#include "../../lib/base/Math.hpp"

#include <boost/random/linear_congruential.hpp>
#include <boost/random/triangle_distribution.hpp>
#include <boost/random/variate_generator.hpp>

#include<yade/core/Timing.hpp>

YADE_PLUGIN((WireMat)(WireState)(WirePhys)(Ip2_WireMat_WireMat_WirePhys)(Law2_ScGeom_WirePhys_WirePM));


/********************** WireMat ****************************/
CREATE_LOGGER(WireMat);

void WireMat::postLoad(WireMat&){

	//BUG: ????? postLoad is called twice,
	LOG_TRACE( "WireMat::postLoad - update material parameters" );
	
	// compute cross-section area for single wire
	as = pow(diameter*0.5,2)*Mathr::PI;

	// check for stress strain curve for single wire
	if(strainStressValues.empty()) return; // uninitialized object, don't do nothing at all
	if(strainStressValues.size() < 2)
		throw std::invalid_argument("WireMat.strainStressValues: at least two points must be given.");
	if(strainStressValues[0](0) == 0. && strainStressValues[0](1) == 0.)
		throw std::invalid_argument("WireMat.strainStressValues: Definition must start with values greater than zero (strain>0,stress>0)");
	
	switch(type) {
		case 0:
			LOG_DEBUG("WireMat - Bertrand's approach");
			if(!strainStressValuesDT.empty())
				throw std::invalid_argument("Use of WireMat.strainStressValuesDT has no effect!");
			break;
		case 1:
			// check stress strain curve four double twist if type=1
			LOG_DEBUG("WireMat - New approach with two curves");
			if(isDoubleTwist) {
				if(strainStressValuesDT.empty())
					throw runtime_error("WireMat.strainStressValuesDT not defined");
				if(strainStressValuesDT.size() < 2)
					throw std::invalid_argument("WireMat.strainStressValuesDT: at least two points must be given.");
				if(strainStressValuesDT[0](0) == 0. && strainStressValuesDT[0](1))
					throw std::invalid_argument("WireMat.strainStressValuesDT: Definition must start with values greater than zero (strain>0,stress>0)");
			}
			break;
		case 2:
			// check stress strain curve four double twist if type=2
			LOG_DEBUG("WireMat - New approach with two curves and initial shift");
			if(isDoubleTwist) {
				if(strainStressValuesDT.empty())
					throw runtime_error("WireMat.strainStressValuesDT not defined");
				if(strainStressValuesDT.size() < 2)
					throw std::invalid_argument("WireMat.strainStressValuesDT: at least two points must be given.");
				if(strainStressValuesDT[0](0) == 0. && strainStressValuesDT[0](1))
					throw std::invalid_argument("WireMat.strainStressValuesDT: Definition must start with values greater than zero (strain>0,stress>0)");
			}
			break;
		default:
			throw std::invalid_argument("WireMat.type: Type must be 0, 1 or 2.");
			break;
	}

}


/********************** Law2_ScGeom_WirePhys_WirePM ****************************/
CREATE_LOGGER(Law2_ScGeom_WirePhys_WirePM);

void Law2_ScGeom_WirePhys_WirePM::go(shared_ptr<IGeom>& ig, shared_ptr<IPhys>& ip, Interaction* contact){

	LOG_TRACE( "Law2_ScGeom_WirePhys_WirePM::go - contact law" );

	ScGeom* geom = static_cast<ScGeom*>(ig.get()); 
	WirePhys* phys = static_cast<WirePhys*>(ip.get());
	const int &id1 = contact->getId1();
	const int &id2 = contact->getId2();
	Body* b1 = Body::byId(id1,scene).get();
	Body* b2 = Body::byId(id2,scene).get();
	
	Real displN = geom->penetrationDepth; // NOTE: ScGeom -> penetrationDepth>0 when spheres interpenetrate, and therefore, for wire always negative

	/* get reference to values since values are updated/changed in order to take unloading into account */
	vector<Vector2r> &DFValues = phys->displForceValues;
	vector<Real> &kValues = phys->stiffnessValues;
	Real kn = phys->kn;

	Real D = displN - phys->initD; // interparticular distance is computed depending on the equilibrium distance

	/* check whether the particles are linked or not */
	if ( !phys->isLinked ) { // destroy the interaction before calculation
		scene->interactions->requestErase(contact);
		return;
	}
	if ( (phys->isLinked) && (D < DFValues.back()(0)) ) { // spheres are linked but failure because of reaching maximal admissible displacement 
		phys->isLinked=false; 
		// update body state with the number of broken links
		WireState* st1=dynamic_cast<WireState*>(b1->state.get());
		WireState* st2=dynamic_cast<WireState*>(b2->state.get());
		st1->numBrokenLinks+=1;
		st2->numBrokenLinks+=1;
		scene->interactions->requestErase(contact);
		return;
	}
	
	/* compute normal force Fn */
	Real Fn = 0.;
	if ( D > DFValues[0](0) ) { // unloading
		LOG_TRACE("WirePM: Unloading");
		Fn = kn * (D-phys->plastD);
	}
	else { // loading
		LOG_TRACE("WirePM: Loading");
		for (unsigned int i=1; i<DFValues.size(); i++) { 
			if ( D > DFValues[i](0) ) {
				Fn = DFValues[i-1](1) + (D-DFValues[i-1](0))*kValues[i-1];
				phys->plastD = D - Fn/kn;
				// update values for unloading
				DFValues[0](0) = D;
				DFValues[0](1) = Fn;
				break;
			}
		}
	}
	
	/* compression forces cannot be applied to wires */
	if (Fn > 0.) Fn = 0.;
	
	TRVAR3( displN, D, Fn );
	
	phys->normalForce = Fn*geom->normal; // NOTE: normal is position2-position1 - It is directed from particle1 to particle2

	/* compute a limit value to check how far the interaction is from failing */
	Real limitFactor = 0.;
	if (Fn < 0.) limitFactor = fabs(D/(DFValues.back()(0)));
	phys->limitFactor = limitFactor;

	State* st1 = Body::byId(id1,scene)->state.get();
	State* st2 = Body::byId(id2,scene)->state.get();
	
	/* apply forces */
	Vector3r f = phys->normalForce;
	// these lines to adapt to periodic boundary conditions
	if ( !scene->isPeriodic )  
		applyForceAtContactPoint(f , geom->contactPoint , id2, st2->se3.position, id1, st1->se3.position);
	else { // in scg we do not wrap particles positions, hence "applyForceAtContactPoint" cannot be used when scene is periodic
		scene->forces.addForce(id1,-f);
		scene->forces.addForce(id2,f);
	}
	
	/* set shear force to zero */
	phys->shearForce = Vector3r::Zero();

}

/********************** Ip2_WireMat_WireMat_WirePhys ****************************/
CREATE_LOGGER(Ip2_WireMat_WireMat_WirePhys);

void Ip2_WireMat_WireMat_WirePhys::go(const shared_ptr<Material>& b1, const shared_ptr<Material>& b2, const shared_ptr<Interaction>& interaction){
	
	/* avoid any updates if interactions which already exist */
	if(interaction->phys) return; 
	//TODO: make boolean to make sure physics are never updated, optimisation of contact detection mesh (no contact detection after link is created) 
	
	LOG_TRACE( "Ip2_WireMat_WireMat_WirePhys::go - create interaction physics" );
	
	ScGeom* geom=dynamic_cast<ScGeom*>(interaction->geom.get());
	assert(geom);

	/* set equilibrium distance, e.g. initial distance between particle (stress free state) */
	shared_ptr<WirePhys> contactPhysics(new WirePhys()); 
	Real initD = geom->penetrationDepth;
	contactPhysics->normalForce = Vector3r::Zero();

	/* get values from material */
	const shared_ptr<WireMat>& mat1 = YADE_PTR_CAST<WireMat>(b1);
	const shared_ptr<WireMat>& mat2 = YADE_PTR_CAST<WireMat>(b2);

	Real crossSection;
	vector<Vector2r> SSValues;
	
	/* check properties of interaction */
	if ( mat1->id == mat2->id ) { // interaction of two bodies of the same material
		crossSection = mat1->as;
		SSValues = mat1->strainStressValues;
		if ( (mat1->isDoubleTwist) && (abs(interaction->getId1()-interaction->getId2())==1) ) {// bodies which id differs by 1 are double twisted
			contactPhysics->isDoubleTwist = true;
			if ( mat1->type==1 || mat1->type==2 ) {
				SSValues = mat1->strainStressValuesDT;
				crossSection *= 2.;
			}
		}
		else {
			contactPhysics->isDoubleTwist = false;
		}
	}
	else { // interaction of two bodies of two different materials, take weaker material and no double-twist
		contactPhysics->isDoubleTwist = false;
		if ( mat1->diameter <= mat2->diameter){
			crossSection = mat1->as;
			SSValues = mat1->strainStressValues;
		}
		else {
			crossSection = mat2->as;
			SSValues = mat2->strainStressValues;
		}
	}
	
	Real R1 = geom->radius1;
	Real R2 = geom->radius2;
	
	Real l0 = R1 + R2 - initD; // initial length of the wire (can be single or double twisted)

	/* compute displacement-force values */
	vector<Vector2r> DFValues;
	vector<Real> kValues;
	Real dl = 0.;
	bool isShifted = false;
	
	/* account for random distortion if type=2 */
	if ( mat1->type==2 ) {
		isShifted = true;
		if (mat1->seed==-1)
			dl = l0*mat1->lambdau;
		else {
			// initialize random number generator
			static boost::minstd_rand randGen(mat1->seed!=0?mat1->seed:(int)TimingInfo::getNow(true));
			static boost::variate_generator<boost::minstd_rand&, boost::triangle_distribution<Real> > rnd(randGen, boost::triangle_distribution<Real>(0,0.5,1));
			Real rndu = rnd();
			TRVAR1( rndu );
			dl = l0*mat1->lambdau*rndu;
			isShifted = true;
		}
	}
	else if ( mat2->type==2 ) {
		isShifted = true;
		if (mat2->seed==-1)
			dl = l0*mat2->lambdau;
		else {
			// initialize random number generator
			static boost::minstd_rand randGen(mat2->seed!=0?mat2->seed:(int)TimingInfo::getNow(true));
			static boost::variate_generator<boost::minstd_rand&, boost::triangle_distribution<Real> > rnd(randGen, boost::triangle_distribution<Real>(0,0.5,1));
			Real rndu = rnd();
			TRVAR1( rndu );
			dl = l0*mat2->lambdau*rndu;
		}
	}
	contactPhysics->dL=dl;
	contactPhysics->isShifted=isShifted;
	
	// update geometry values
	l0 += dl;
	contactPhysics->initD = initD;

	/* compute threshold displacement-force values (tension negative since ScGem is used!) */
	for ( vector<Vector2r>::iterator it = SSValues.begin(); it != SSValues.end(); it++ ) {
		Vector2r values = Vector2r::Zero();
// 		values(0) = -(*it)(0)*l0;
		values(0) = -(*it)(0)*l0-dl;
		values(1) = -(*it)(1)*crossSection;
		DFValues.push_back(values);
	}

	/* compute elastic stiffness for unloading*/
	Real k = DFValues[0](1) / (DFValues[0](0)+dl);

	/* update values if the interaction is a double twist and type=0 */
	if ( contactPhysics->isDoubleTwist && mat1->type==0 ) {
		// type=0 (force displacement values are computed by manipulating the values of the single wire by using the parameters lambdak and lambdaEps)
		Real alpha = atan( l0 / (3.*Mathr::PI*mat1->diameter) );
		Real kh = k * ( l0*mat1->diameter/crossSection ) / ( 48.*cos(alpha) * ( 41./9.*(1.+mat1->poisson) + 17./4.*pow(tan(alpha),2) ) );
		k = 2. * ( mat1->lambdak*kh + (1-mat1->lambdak)*k );
		Real F = k * DFValues[0](0);
		Real mappingF = F/DFValues[0](1);
		DFValues[0](1) = F;
		for (unsigned int i = 1; i<DFValues.size(); i++) {
			DFValues[i](0) *= mat1->lambdaEps;
			DFValues[i](1) *= mappingF;
		}
	}
	else {
	// type=1 and type=2 (force displacement values have already been computed by given stress-strain curve)
	} 
	
	/* store elastic/unloading stiffness as kn in physics */
	contactPhysics->kn = k;
	contactPhysics->ks = 0.;
	TRVAR1( k );

	/* consider an additional point for the initial shift if type==2 */
	if ( mat1->type==2 ) {
		Vector2r values = Vector2r::Zero();
		values(0) = -dl+mat1->lambdaF*(DFValues[0](0)+dl);
		values(1) = DFValues[0](1)*mat1->lambdaF;
		k = values(1) / values(0);
		if ( mat1->lambdaF<1. )
			DFValues.insert( DFValues.begin(), values );
	}
	else if ( mat2->type==2 ) {
		Vector2r values = Vector2r::Zero();
		values(0) = -dl+mat2->lambdaF*(DFValues[0](0)+dl);
		values(1) = DFValues[0](1)*mat2->lambdaF;
		k = values(1) / values(0);
		if ( mat2->lambdaF<1. )
			DFValues.insert( DFValues.begin(), values );
	}

	/* compute stiffness-values of wire */
	kValues.push_back(k);
	for( unsigned int i = 1 ; i < DFValues.size(); i++ ) {
		Real deltau = -DFValues[i](0) + DFValues[i-1](0);
		Real deltaF = -DFValues[i](1) + DFValues[i-1](1);
		k = deltaF/deltau;
		kValues.push_back(k);
	}
	
	/* add zero values for first point */
	DFValues.insert( DFValues.begin(), Vector2r::Zero() );

	/* store values in physics */
	contactPhysics->displForceValues = DFValues;
	contactPhysics->stiffnessValues = kValues;

	/* set particles as linked */
	if ( (scene->iter < linkThresholdIteration))
		contactPhysics->isLinked=true;
	else
		contactPhysics->isLinked=false;

	interaction->phys = contactPhysics;

}

WirePhys::~WirePhys(){}

