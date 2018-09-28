/*************************************************************************
*  Copyright (C) 2018 by Robert Caulk <rob.caulk@gmail.com>  		 *
*  Copyright (C) 2018 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*									 *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
/* This engine is under active development. Experimental only */
//#define THERMAL
#ifdef THERMAL
#include<pkg/pfv/Thermal.hpp>
#include<core/Scene.hpp>
#include<core/Omega.hpp>
#include<pkg/common/Sphere.hpp>
#include<pkg/dem/Shop.hpp>
 
YADE_PLUGIN((ThermalState));
ThermalState::~ThermalState(){};

CREATE_LOGGER(ThermalEngine);
YADE_PLUGIN((ThermalEngine));

ThermalEngine::~ThermalEngine(){}

void ThermalEngine::action()
{
	scene = Omega::instance().getScene().get();
	FOREACH(const shared_ptr<Engine> e, Omega::instance().getScene()->engines) {
		if (e->getClassName() == "FlowEngine") {
			flow = dynamic_cast<FlowEngineT*>(e.get());
		}
	}
	if (!conduction) thermoMech = false; //don't allow thermoMech if conduction is not activated
	if (advection) {
		flow->solver->initializeInternalEnergy(); // internal energy of cells
		flow->solver->augmentConductivityMatrix(scene->dt);
	}
	if (conduction) {
		if (setInternalEnergy) initializeInternalEnergy();  // internal energy of particles
		computeSolidSolidFluxes(); 
		if (advection) computeSolidFluidFluxes(flow);
	}
	computeNewTemperatures(flow); // new temps for particles and pores
	if (thermoMech) thermalExpansion();
}

void ThermalEngine::makeThermalState() {
	// loop through bodies and copy information over to new state
}

void ThermalEngine::initializeInternalEnergy() {
	const shared_ptr<BodyContainer>& bodies=scene->bodies;
	const long size=bodies->size();
//	#pragma omp parallel for
	for (long i=0; i<size; i++){
		const shared_ptr<Body>& b=(*bodies)[i];
		if (b->shape->getClassIndex()!=Sphere::getClassIndexStatic() || !b) continue;
		ThermalState* thState = YADE_CAST<ThermalState*>(b->state.get());
		thState->U = thState->Cp*thState->mass*thState->temp;
	}
	setInternalEnergy = false;
}

void ThermalEngine::computeSolidFluidFluxes(FlowEngineT* flow) {		
	fluidK = flow->fluidK;
	if (!flow->solver->sphericalVertexAreaCalculated) computeVertexSphericalArea(flow);
	shared_ptr<BodyContainer>& bodies = scene->bodies;
	Tesselation& Tes = flow->solver->T[flow->solver->currentTes];
//	#ifdef YADE_OPENMP
	const long size = Tes.cellHandles.size();
//	#pragma omp parallel for num_threads(ompThreads>0 ? ompThreads : 1)
    	for (long i=0; i<size; i++){
		CellHandle& cell = Tes.cellHandles[i];
//	#else	
		if (cell->info().isGhost) continue; // Do we need special cases for fictious cells?
		for (int v=0;v<4;v++){
			//if (cell->vertex(v)->info().isFictious) continue;
			const long int id = cell->vertex(v)->info().id();
			const shared_ptr<Body>& b =(*bodies)[id];
			if (b->shape->getClassIndex()!=Sphere::getClassIndexStatic() || !b) continue;
			const double surfaceArea = cell->info().sphericalVertexSurface[v];
			computeFlux(cell,b,surfaceArea);
		}
	}
}


void ThermalEngine::computeVertexSphericalArea(FlowEngineT* flow){
	Tesselation& Tes = flow->solver->T[flow->solver->currentTes];
//	#ifdef YADE_OPENMP
	const long size = Tes.cellHandles.size();
//	#pragma omp parallel for num_threads(ompThreads>0 ? ompThreads : 1)
    	for (long i=0; i<size; i++){
		CellHandle& cell = Tes.cellHandles[i];
//	#else		
		if (cell->info().isFictious) continue;
		VertexHandle W[4];
		for (int k=0;k<4;k++) W[k] = cell->vertex(k);
			cell->info().sphericalVertexSurface[0]=flow->solver->fastSphericalTriangleArea(W[0]->point(),W[1]->point().point(),W[2]->point().point(),W[3]->point().point());
			cell->info().sphericalVertexSurface[1]=flow->solver->fastSphericalTriangleArea(W[1]->point(),W[0]->point().point(),W[2]->point().point(),W[3]->point().point());
			cell->info().sphericalVertexSurface[2]=flow->solver->fastSphericalTriangleArea(W[2]->point(),W[1]->point().point(),W[0]->point().point(),W[3]->point().point());
			cell->info().sphericalVertexSurface[3]=flow->solver->fastSphericalTriangleArea(W[3]->point(),W[1]->point().point(),W[2]->point().point(),W[0]->point().point());
		}
	flow->solver->sphericalVertexAreaCalculated=true;
}


void ThermalEngine::computeFlux(CellHandle& cell,const shared_ptr<Body>& b, const double surfaceArea) {
	const Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());
	ThermalState* thState = YADE_CAST<ThermalState*>(b->state.get()); 
	const double h = 2. * fluidK / (2.*sphere->radius); // heat transfer coeff assuming Re<<1 (stokes flow)
	//const double areaRatio = surfaceArea/(4.*M_PI*sphere->radius*sphere->radius);
	const double flux = h*surfaceArea*(cell->info().temp() - thState->temp); // Total surface accounted for through pores, but we also account for conduction area? Unphysical? 
	if (!cell->info().Tcondition) cell->info().internalEnergy -= flux*scene->dt;
	thState->U += flux*scene->dt;
}


void ThermalEngine::computeSolidSolidFluxes() {
//	#ifdef YADE_OPENMP
	const shared_ptr<InteractionContainer>& interactions=scene->interactions;
	const long size=interactions->size();
//	#pragma omp parallel for
	for (long i=0; i<size; i++){
		const shared_ptr<Interaction>& I=(*interactions)[i];
//	#else
//	for (const auto & I : *scene->interactions){
//	#endif
		const ScGeom* geom;
		if (!I || !I->geom.get() || !I->phys.get()) continue;
		if (I->geom.get()){
			geom = YADE_CAST<ScGeom*> (I->geom.get());
			if (!geom) continue;
			const double pd = geom->penetrationDepth;

		const shared_ptr<Body>& b1_=Body::byId(I->getId1(),scene);
		const shared_ptr<Body>& b2_=Body::byId(I->getId2(),scene);
		if (b1_->shape->getClassIndex()!=Sphere::getClassIndexStatic() || b2_->shape->getClassIndex()!=Sphere::getClassIndexStatic() || !b1_ || !b2_) continue;
		ThermalState* thState1 = YADE_CAST<ThermalState*>(b1_->state.get()); 
		ThermalState* thState2 = YADE_CAST<ThermalState*>(b2_->state.get()); 
		Sphere* sphere1 = dynamic_cast<Sphere*>(b1_->shape.get());
		Sphere* sphere2 = dynamic_cast<Sphere*>(b2_->shape.get());
		
		double& U1 = thState1->U;
		double& U2 = thState2->U;	
		const double k1 = thState1->k;
		const double k2 = thState2->k;
		const double r1 = sphere1->radius;
		const double r2 = sphere2->radius;
		const double T1 = thState1->temp;
		const double T2 = thState2->temp;
		const double d = r1 + r2 - pd;
		if (d==0) continue;
		double R = 0;
		double r = 0;
		// for equation: 
		if (r1 >= r2) {R = r1;r = r2;}
		else if (r1 < r2) {R = r2;r = r1;} 
		// The radius of the intersection found by: Kern, W. F. and Bland, J. R. Solid Mensuration with Proofs, 2nd ed. New York: Wiley, p. 97, 1948.	http://mathworld.wolfram.com/Sphere-SphereIntersection.html	
		const double numerator = pow((-d+r-R)*(-d-r+R)*(-d+r+R)*(d+r+R),0.5);	
		const double rc = numerator / (2.*d);
		const double dt = scene->dt;		
		const double fluxij = 4.*rc*(T1-T2) / (1./k1 + 1./k2);
		
		U1 -= fluxij*dt;
		U2 += fluxij*dt;
		}
	}
}

void ThermalEngine::computeNewTemperatures(FlowEngineT* flow) {
	if (conduction){
		const shared_ptr<BodyContainer>& bodies=scene->bodies;
		const long size=bodies->size();
//		#pragma omp parallel for
		for (long i=0; i<size; i++){
			const shared_ptr<Body>& b=(*bodies)[i];
			if (b->shape->getClassIndex()!=Sphere::getClassIndexStatic() || !b) continue;
			ThermalState* thState = YADE_CAST<ThermalState*>(b->state.get());
			thState->oldTemp = thState->temp; 
			thState->temp = thState->U/(thState->Cp*thState->mass); // + thState->temp;
		}
	}
	if (advection) flow->solver->setNewCellTemps();
}


void ThermalEngine::thermalExpansion() {
	const shared_ptr<BodyContainer>& bodies=scene->bodies;
	const long size=bodies->size();

	for (long i=0; i<size; i++){
		const shared_ptr<Body> b=(*bodies)[i];
		if (b->shape->getClassIndex()!=Sphere::getClassIndexStatic() || !b) continue;
		Sphere* sphere = dynamic_cast<Sphere*>(b->shape.get());		
		ThermalState* thState = YADE_CAST<ThermalState*>(b->state.get());
		sphere->radius +=  thState->alpha * sphere->radius * (thState->temp - thState->oldTemp);
	}
}









#endif//THERMAL
