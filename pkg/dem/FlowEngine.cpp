/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano <catalano@grenoble-inp.fr>    *
*  Copyright (C) 2009 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2012 by Donia Marzougui <donia.marzougui@grenoble-inp.fr>*
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef YADE_CGAL

#ifdef FLOW_ENGINE
#include<yade/core/Scene.hpp>
#include<yade/lib/base/Math.hpp>
#include<yade/pkg/dem/TesselationWrapper.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Wall.hpp>
#include<yade/pkg/common/Box.hpp>
#include <sys/stat.h>
#include <sys/types.h>
#include <boost/thread.hpp>
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#ifdef LINSOLV
#include <cholmod.h>
#endif

#include "FlowEngine.hpp"

CREATE_LOGGER ( FlowEngine );
CREATE_LOGGER ( PeriodicFlowEngine );

CGT::CVector makeCgVect ( const Vector3r& yv ) {return CGT::CVector ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r ( const CGT::Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r ( const CGT::CVector& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}


FlowEngine::~FlowEngine()
{
}

const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

void FlowEngine::action()
{
        if ( !isActivated ) return;
        timingDeltas->start();
	setPositionsBuffer(true);
	timingDeltas->checkpoint ( "Position buffer" );
        if (first) {
	  if (multithread) setPositionsBuffer(false);
	  buildTriangulation(pZero,solver);
	  initializeVolumes(solver);
	  backgroundSolver=solver;
	  backgroundCompleted=true;
	}
	solver->ompThreads = ompThreads>0? ompThreads : omp_get_max_threads();

        timingDeltas->checkpoint ( "Triangulating" );
	updateVolumes ( solver );
        timingDeltas->checkpoint ( "Update_Volumes" );
	
        epsVolCumulative += epsVolMax;
	retriangulationLastIter++;
	if (!updateTriangulation) updateTriangulation = // If not already set true by another function of by the user, check conditions
		(defTolerance>0 && epsVolCumulative > defTolerance) || retriangulationLastIter>meshUpdateInterval;

        ///Compute flow and and forces here
	if (pressureForce){
		solver->gaussSeidel(scene->dt);
		timingDeltas->checkpoint ( "Gauss-Seidel (includes matrix construct and factorization in single-thread mode)" );
		solver->computeFacetForcesWithCache();}
        timingDeltas->checkpoint ( "Compute_Forces" );
        ///Application of vicscous forces
        scene->forces.sync();
	timingDeltas->checkpoint ( "forces.sync()" );
	computeLubricationContributions ( *solver );
	timingDeltas->checkpoint ( "viscous forces" );
	Vector3r force;
	Vector3r torque;
        FiniteVerticesIterator verticesEnd = solver->T[solver->currentTes].Triangulation().finite_vertices_end();
        for ( FiniteVerticesIterator vIt = solver->T[solver->currentTes].Triangulation().finite_vertices_begin(); vIt !=  verticesEnd; vIt++ ) {
		force = pressureForce ? Vector3r ( vIt->info().forces[0],vIt->info().forces[1],vIt->info().forces[2] ): Vector3r(0,0,0);
		torque = Vector3r(0,0,0);
                if (shearLubrication || viscousShear){
			force = force + solver->shearLubricationForces[vIt->info().id()];
			torque = torque + solver->shearLubricationTorques[vIt->info().id()];
			if (pumpTorque)
				torque = torque + solver->pumpLubricationTorques[vIt->info().id()];
		}
		if (twistTorque)
			torque = torque + solver->twistLubricationTorques[vIt->info().id()];
		if (normalLubrication)
			force = force + solver-> normalLubricationForce[vIt->info().id()];
		scene->forces.addForce ( vIt->info().id(), force);
		scene->forces.addTorque ( vIt->info().id(), torque);
        }
        ///End Compute flow and forces
        timingDeltas->checkpoint ( "Applying Forces" );
	int sleeping = 0;
	if (multithread && !first) {
		while (updateTriangulation && !backgroundCompleted) { /*cout<<"sleeping..."<<sleeping++<<endl;*/
		  sleeping++;
		boost::this_thread::sleep(boost::posix_time::microseconds(1000));}
		if (debug && sleeping) cerr<<"sleeping..."<<sleeping<<endl;
		if (updateTriangulation || (ellapsedIter>(0.5*meshUpdateInterval) && backgroundCompleted)) {
			if (debug) cerr<<"switch flow solver"<<endl;
			if (useSolver==0) LOG_ERROR("background calculations not available for Gauss-Seidel");
			if (fluidBulkModulus>0) solver->interpolate (solver->T[solver->currentTes], backgroundSolver->T[backgroundSolver->currentTes]);
			solver=backgroundSolver;
			backgroundSolver = shared_ptr<FlowSolver> (new FlowSolver);
			//Copy imposed pressures/flow from the old solver
			backgroundSolver->imposedP = vector<pair<CGT::Point,Real> >(solver->imposedP);
			backgroundSolver->imposedF = vector<pair<CGT::Point,Real> >(solver->imposedF);
			if (debug) cerr<<"switched"<<endl;
			setPositionsBuffer(false);//set "parallel" buffer for background calculation 
			backgroundCompleted=false;
			retriangulationLastIter=ellapsedIter;
			updateTriangulation=false;
			epsVolCumulative=0;
			ellapsedIter=0;
			boost::thread workerThread(&FlowEngine::backgroundAction,this);
			workerThread.detach();
			if (debug) cerr<<"backgrounded"<<endl;
			initializeVolumes(solver);
			computeLubricationContributions(*solver);
			if (debug) cerr<<"volumes initialized"<<endl;
		}
		else {
			if (debug && !backgroundCompleted) cerr<<"still computing solver in the background, ellapsedIter="<<ellapsedIter<<endl;
			ellapsedIter++;
		}
	} else {
	        if (updateTriangulation && !first) {
			buildTriangulation (pZero, solver);
			initializeVolumes(solver);
			computeLubricationContributions(*solver);
               		updateTriangulation = false;
			epsVolCumulative=0;
			retriangulationLastIter=0;
			ReTrg++;}
        }
        first=false;
        timingDeltas->checkpoint ( "triangulate + init volumes" );
}

void FlowEngine::backgroundAction()
{
	if (useSolver<1) {LOG_ERROR("background calculations not available for Gauss-Seidel"); return;}
        buildTriangulation ( pZero,backgroundSolver );
	//FIXME: GS is computing too much, we need only matrix factorization in fact
	backgroundSolver->gaussSeidel(scene->dt);
	//FIXME(2): and here we need only cached variables, not forces
	backgroundSolver->computeFacetForcesWithCache(/*onlyCache?*/ true);
// 	boost::this_thread::sleep(boost::posix_time::seconds(5));
 	backgroundCompleted = true;
}

template<class Solver>

void FlowEngine::boundaryConditions ( Solver& flow )
{

	for (int k=0;k<6;k++)	{
		flow->boundary (wallIds[k]).flowCondition=!bndCondIsPressure[k];
                flow->boundary (wallIds[k]).value=bndCondValue[k];
                flow->boundary (wallIds[k]).velocity = boundaryVelocity[k];//FIXME: needs correct implementation, maybe update the cached pos/vel?
	}
}

template<class Solver>
void FlowEngine::setImposedPressure ( unsigned int cond, Real p,Solver& flow )
{
        if ( cond>=flow->imposedP.size() ) LOG_ERROR ( "Setting p with cond higher than imposedP size." );
        flow->imposedP[cond].second=p;
        //force immediate update of boundary conditions
	flow->pressureChanged=true;
}

template<class Solver>
void FlowEngine::imposeFlux ( Vector3r pos, Real flux,Solver& flow ){
        flow.imposedF.push_back ( pair<CGT::Point,Real> ( CGT::Point ( pos[0],pos[1],pos[2] ), flux ) );
}

template<class Solver>
void FlowEngine::clearImposedPressure ( Solver& flow ) { flow->imposedP.clear(); flow->ipCells.clear();}
template<class Solver>
void FlowEngine::clearImposedFlux ( Solver& flow ) { flow->imposedF.clear(); flow->ifCells.clear();}

template<class Solver>
Real FlowEngine::getCellFlux ( unsigned int cond, const shared_ptr<Solver>& flow )
{
	if ( cond>=flow->imposedP.size() ) {LOG_ERROR ( "Getting flux with cond higher than imposedP size." ); return 0;}
        double flux=0;
        typename Solver::CellHandle& cell= flow->ipCells[cond];
        for ( int ngb=0;ngb<4;ngb++ ) {
                flux+= cell->info().kNorm() [ngb]* ( cell->info().p()-cell->neighbor ( ngb )->info().p() );
        }
        return flux+cell->info().dv();
}

template<class Solver>
void FlowEngine::initSolver ( Solver& flow )
{
       	flow->Vtotalissimo=0; flow->VSolidTot=0; flow->vPoral=0; flow->sSolidTot=0;
        flow->slipOnLaterals = slipBoundary;
        flow->kFactor = permeabilityFactor;
        flow->debugOut = debug;
        flow->useSolver = useSolver;
	#ifdef EIGENSPARSE_LIB
	flow->numSolveThreads = numSolveThreads;
	flow->numFactorizeThreads = numFactorizeThreads;
	#endif
	flow->meanKStat = meanKStat;
        flow->viscosity = viscosity;
        flow->tolerance=tolerance;
        flow->relax=relax;
        flow->clampKValues = clampKValues;
	flow->maxKdivKmean = maxKdivKmean;
	flow->minKdivKmean = minKdivKmean;
        flow->meanKStat = meanKStat;
        flow->permeabilityMap = permeabilityMap;
        flow->fluidBulkModulus = fluidBulkModulus;
        flow->T[flow->currentTes].Clear();
        flow->T[flow->currentTes].maxId=-1;
        flow->xMin = 1000.0, flow->xMax = -10000.0, flow->yMin = 1000.0, flow->yMax = -10000.0, flow->zMin = 1000.0, flow->zMax = -10000.0;
}

#ifdef LINSOLV
template<class Solver>
void FlowEngine::setForceMetis ( Solver& flow, bool force )
{
        if (force) {
		flow->eSolver.cholmod().nmethods=1;
		flow->eSolver.cholmod().method[0].ordering=CHOLMOD_METIS;
	} else cholmod_defaults(&(flow->eSolver.cholmod()));
}

template<class Solver>
bool FlowEngine::getForceMetis ( Solver& flow ) {return (flow->eSolver.cholmod().nmethods==1);}
#endif

template<class Solver>
void FlowEngine::buildTriangulation ( Solver& flow )
{
        buildTriangulation ( 0.f,flow );
}

template<class Solver>
void FlowEngine::buildTriangulation ( double pZero, Solver& flow )
{
        flow->resetNetwork();
	if (first) flow->currentTes=0;
        else {
                flow->currentTes=!flow->currentTes;
                if (debug) cout << "--------RETRIANGULATION-----------" << endl;
        }

	initSolver(flow);

        addBoundary ( flow );
        triangulate ( flow );
        if ( debug ) cout << endl << "Tesselating------" << endl << endl;
        flow->T[flow->currentTes].Compute();

        flow->defineFictiousCells();
	// For faster loops on cells define this vector
	flow->T[flow->currentTes].cellHandles.clear();
	flow->T[flow->currentTes].cellHandles.reserve(flow->T[flow->currentTes].Triangulation().number_of_finite_cells());
	FiniteCellsIterator cellEnd = flow->T[flow->currentTes].Triangulation().finite_cells_end();
	int k=0;
	for ( FiniteCellsIterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cellEnd; cell++ ){
		flow->T[flow->currentTes].cellHandles.push_back(cell);
		cell->info().id=k++;}//define unique numbering now, corresponds to position in cellHandles
        flow->displayStatistics ();
        flow->computePermeability();
        porosity = flow->vPoralPorosity/flow->vTotalePorosity;

        boundaryConditions ( flow );
        flow->initializePressures ( pZero );
	
        if ( !first && !multithread && (useSolver==0 || fluidBulkModulus>0)) flow->interpolate ( flow->T[!flow->currentTes], flow->T[flow->currentTes] );
        if ( waveAction ) flow->applySinusoidalPressure ( flow->T[flow->currentTes].Triangulation(), sineMagnitude, sineAverage, 30 );
        if (normalLubrication || shearLubrication || viscousShear) flow->computeEdgesSurfaces();
}

void FlowEngine::setPositionsBuffer(bool current)
{
	vector<posData>& buffer = current? positionBufferCurrent : positionBufferParallel;
	buffer.clear();
	buffer.resize(scene->bodies->size());
	shared_ptr<Sphere> sph ( new Sphere );
        const int Sph_Index = sph->getClassIndexStatic();
	FOREACH ( const shared_ptr<Body>& b, *scene->bodies ) {
                if (!b || ignoredBody==b->getId()) continue;
                posData& dat = buffer[b->getId()];
		dat.id=b->getId();
		dat.pos=b->state->pos;
		dat.isSphere= (b->shape->getClassIndex() ==  Sph_Index);
		if (dat.isSphere) dat.radius = YADE_CAST<Sphere*>(b->shape.get())->radius;
		dat.exists=true;
	}
}

template<class Solver>
void FlowEngine::addBoundary ( Solver& flow )
{
	vector<posData>& buffer = multithread ? positionBufferParallel : positionBufferCurrent;
        solver->xMin = Mathr::MAX_REAL, solver->xMax = -Mathr::MAX_REAL, solver->yMin = Mathr::MAX_REAL, solver->yMax = -Mathr::MAX_REAL, solver->zMin = Mathr::MAX_REAL, solver->zMax = -Mathr::MAX_REAL;
        FOREACH ( const posData& b, buffer ) {
                if ( !b.exists ) continue;
                if ( b.isSphere ) {
                        const Real& rad = b.radius;
                        const Real& x = b.pos[0];
                        const Real& y = b.pos[1];
                        const Real& z = b.pos[2];
                        flow->xMin = min ( flow->xMin, x-rad );
                        flow->xMax = max ( flow->xMax, x+rad );
                        flow->yMin = min ( flow->yMin, y-rad );
                        flow->yMax = max ( flow->yMax, y+rad );
                        flow->zMin = min ( flow->zMin, z-rad );
                        flow->zMax = max ( flow->zMax, z+rad );
                }
        }
	//FIXME id_offset must be set correctly, not the case here (always 0), then we need walls first or it will fail
        idOffset = flow->T[flow->currentTes].maxId+1;
        flow->idOffset = idOffset;
        flow->sectionArea = ( flow->xMax - flow->xMin ) * ( flow->zMax-flow->zMin );
        flow->vTotal = ( flow->xMax-flow->xMin ) * ( flow->yMax-flow->yMin ) * ( flow->zMax-flow->zMin );
        flow->yMinId=wallIds[ymin];
        flow->yMaxId=wallIds[ymax];
        flow->xMaxId=wallIds[xmax];
        flow->xMinId=wallIds[xmin];
        flow->zMinId=wallIds[zmin];
        flow->zMaxId=wallIds[zmax];

        //FIXME: Id's order in boundsIds is done according to the enumeration of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
        flow->boundsIds[0]= &flow->xMinId;
        flow->boundsIds[1]= &flow->xMaxId;
        flow->boundsIds[2]= &flow->yMinId;
        flow->boundsIds[3]= &flow->yMaxId;
        flow->boundsIds[4]= &flow->zMinId;
        flow->boundsIds[5]= &flow->zMaxId;

	for (int k=0;k<6;k++) flow->boundary ( *flow->boundsIds[k] ).useMaxMin = boundaryUseMaxMin[k];
        flow->cornerMin = CGT::Point ( flow->xMin, flow->yMin, flow->zMin );
        flow->cornerMax = CGT::Point ( flow->xMax, flow->yMax, flow->zMax );
 
        //assign BCs types and values
        boundaryConditions ( flow );

        double center[3];
        for ( int i=0; i<6; i++ ) {
                if ( *flow->boundsIds[i]<0 ) continue;
                CGT::CVector Normal ( normal[i].x(), normal[i].y(), normal[i].z() );
                if ( flow->boundary ( *flow->boundsIds[i] ).useMaxMin ) flow->addBoundingPlane(Normal, *flow->boundsIds[i] );
                else {
			for ( int h=0;h<3;h++ ) center[h] = buffer[*flow->boundsIds[i]].pos[h];
// 			cerr << "id="<<*flow->boundsIds[i] <<" center="<<center[0]<<","<<center[1]<<","<<center[2]<<endl;
                        flow->addBoundingPlane ( center, wallThickness, Normal,*flow->boundsIds[i] );
                }
        }
}

template<class Solver>
void FlowEngine::triangulate ( Solver& flow )
{
///Using Tesselation wrapper (faster)
// 	TesselationWrapper TW;
// 	if (TW.Tes) delete TW.Tes;
// 	TW.Tes = &(flow->T[flow->currentTes]);//point to the current Tes we have in Flowengine
// 	TW.insertSceneSpheres();//TW is now really inserting in FlowEngine, using the faster insert(begin,end)
// 	TW.Tes = NULL;//otherwise, Tes would be deleted by ~TesselationWrapper() at the end of the function.
///Using one-by-one insertion
	vector<posData>& buffer = multithread ? positionBufferParallel : positionBufferCurrent;
	FOREACH ( const posData& b, buffer ) {
                if ( !b.exists ) continue;
                if ( b.isSphere ) {
			if (b.id==ignoredBody) continue;
                        flow->T[flow->currentTes].insert ( b.pos[0], b.pos[1], b.pos[2], b.radius, b.id );}
        }
        flow->T[flow->currentTes].redirected=true;//By inserting one-by-one, we already redirected
        flow->shearLubricationForces.resize ( flow->T[flow->currentTes].maxId+1 );
	flow->shearLubricationTorques.resize ( flow->T[flow->currentTes].maxId+1 );
	flow->pumpLubricationTorques.resize ( flow->T[flow->currentTes].maxId+1 );
	flow->twistLubricationTorques.resize ( flow->T[flow->currentTes].maxId+1 );
	flow->shearLubricationBodyStress.resize ( flow->T[flow->currentTes].maxId+1 );
	flow->normalLubricationForce.resize ( flow->T[flow->currentTes].maxId+1 );
	flow->normalLubricationBodyStress.resize ( flow->T[flow->currentTes].maxId+1 );
}
template<class Solver>
void FlowEngine::initializeVolumes ( Solver& flow )
{
	typedef typename Solver::element_type Flow;
	typedef typename Flow::FiniteVerticesIterator FiniteVerticesIterator;
	typedef typename Solver::element_type Flow;
	
	FiniteVerticesIterator verticesEnd = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
	CGT::CVector Zero(0,0,0);
	for (FiniteVerticesIterator vIt = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); vIt!= verticesEnd; vIt++) vIt->info().forces=Zero;

	FOREACH(CellHandle& cell, flow->T[flow->currentTes].cellHandles)
	{
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = volumeCell ( cell ); break;
			case ( 1 ) : cell->info().volume() = volumeCellSingleFictious ( cell ); break;
			case ( 2 ) : cell->info().volume() = volumeCellDoubleFictious ( cell ); break;
			case ( 3 ) : cell->info().volume() = volumeCellTripleFictious ( cell ); break;
			default: break;
		}
		if (flow->fluidBulkModulus>0) { cell->info().invVoidVolume() = 1. / ( abs(cell->info().volume()) - flow->volumeSolidPore(cell) ); }
	}
	if (debug) cout << "Volumes initialised." << endl;
}

void FlowEngine::averageRealCellVelocity()
{
        solver->averageRelativeCellVelocity();
        Vector3r Vel ( 0,0,0 );
        //AVERAGE CELL VELOCITY
        FiniteCellsIterator cellEnd = solver->T[solver->currentTes].Triangulation().finite_cells_end();
        for ( FiniteCellsIterator cell = solver->T[solver->currentTes].Triangulation().finite_cells_begin(); cell != cellEnd; cell++ ) {
                for ( int g=0;g<4;g++ ) {
                        if ( !cell->vertex ( g )->info().isFictious ) {
                                const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
                                for ( int i=0;i<3;i++ ) Vel[i] = Vel[i] + sph->state->vel[i]/4;
                        }
                }
                RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
                CGT::Point posAvFacet;
                double volumeFacetTranslation = 0;
                CGT::CVector velAv ( Vel[0], Vel[1], Vel[2] );
                for ( int i=0; i<4; i++ ) {
                        volumeFacetTranslation = 0;
                        if ( !Tri.is_infinite ( cell->neighbor ( i ) ) ) {
                                CGT::CVector Surfk = cell->info()-cell->neighbor ( i )->info();
                                Real area = sqrt ( Surfk.squared_length() );
                                Surfk = Surfk/area;
                                CGT::CVector branch = cell->vertex ( facetVertices[i][0] )->point() - cell->info();
                                posAvFacet = ( CGT::Point ) cell->info() + ( branch*Surfk ) *Surfk;
                                volumeFacetTranslation += velAv*cell->info().facetSurfaces[i];
                                cell->info().averageVelocity() = cell->info().averageVelocity() - volumeFacetTranslation/cell->info().volume() * ( posAvFacet-CGAL::ORIGIN );
                        }
                }
        }
}
template<class Solver>
void FlowEngine::updateVolumes ( Solver& flow )
{
        if ( debug ) cout << "Updating volumes.............." << endl;
        Real invDeltaT = 1/scene->dt;
        epsVolMax=0;
        Real totVol=0; Real totDVol=0;
	#ifdef YADE_OPENMP
	const long size=flow->T[flow->currentTes].cellHandles.size();
	#pragma omp parallel for num_threads(ompThreads>0 ? ompThreads : 1)
	for(long i=0; i<size; i++){
		CellHandle& cell = flow->T[flow->currentTes].cellHandles[i];
	#else
	FOREACH(CellHandle& cell, flow->T[flow->currentTes].cellHandles){
	#endif
		double newVol, dVol;
                switch ( cell->info().fictious() ) {
                	case ( 3 ) : newVol = volumeCellTripleFictious ( cell ); break;
               		case ( 2 ) : newVol = volumeCellDoubleFictious ( cell ); break;
                	case ( 1 ) : newVol = volumeCellSingleFictious ( cell ); break;
			case ( 0 ) : newVol = volumeCell (cell ); break;
                	default: newVol = 0; break;}
                dVol=cell->info().volumeSign* ( newVol - cell->info().volume() );
		cell->info().dv() = dVol*invDeltaT;
                cell->info().volume() = newVol;
		if (defTolerance>0) { //if the criterion is not used, then we skip these updates a save a LOT of time when Nthreads > 1
			#pragma omp atomic
			totVol+=newVol;
			#pragma omp atomic
                	totDVol+=abs(dVol);}
        }
	if (defTolerance>0)  epsVolMax = totDVol/totVol;
	for (unsigned int n=0; n<flow->imposedF.size();n++) {
		flow->ifCells[n]->info().dv()+=flow->imposedF[n].second;
		flow->ifCells[n]->info().Pcondition=false;}
        if ( debug ) cout << "Updated volumes, total =" <<totVol<<", dVol="<<totDVol<<endl;
}
template<class Cellhandle>
Real FlowEngine::volumeCellSingleFictious ( Cellhandle cell )
{
        Vector3r V[3];
        int b=0;
        int w=0;
        cell->info().volumeSign=1;
        Real wallCoordinate=0;

        for ( int y=0;y<4;y++ ) {
                if ( ! ( cell->vertex ( y )->info().isFictious ) ) {
                        V[w]=positionBufferCurrent[cell->vertex ( y )->info().id()].pos;
			w++;
                } else {
                        b = cell->vertex ( y )->info().id();
                        const shared_ptr<Body>& wll = Body::byId ( b , scene );
                        if ( !solver->boundary ( b ).useMaxMin ) wallCoordinate = wll->state->pos[solver->boundary ( b ).coordinate]+ ( solver->boundary ( b ).normal[solver->boundary ( b ).coordinate] ) *wallThickness/2;
                        else wallCoordinate = solver->boundary ( b ).p[solver->boundary ( b ).coordinate];
                }
        }
        Real volume = 0.5* ( ( V[0]-V[1] ).cross ( V[0]-V[2] ) ) [solver->boundary ( b ).coordinate] * ( 0.33333333333* ( V[0][solver->boundary ( b ).coordinate]+ V[1][solver->boundary ( b ).coordinate]+ V[2][solver->boundary ( b ).coordinate] ) - wallCoordinate );
        return abs ( volume );
}
template<class Cellhandle>
Real FlowEngine::volumeCellDoubleFictious ( Cellhandle cell )
{
        Vector3r A=Vector3r::Zero(), AS=Vector3r::Zero(),B=Vector3r::Zero(), BS=Vector3r::Zero();

        cell->info().volumeSign=1;
        int b[2];
        int coord[2];
        Real wallCoordinate[2];
        int j=0;
        bool firstSph=true;

        for ( int g=0;g<4;g++ ) {
                if ( cell->vertex ( g )->info().isFictious ) {
                        b[j] = cell->vertex ( g )->info().id();
                        coord[j]=solver->boundary ( b[j] ).coordinate;
                        if ( !solver->boundary ( b[j] ).useMaxMin ) wallCoordinate[j] = positionBufferCurrent[b[j]].pos[coord[j]] + ( solver->boundary ( b[j] ).normal[coord[j]] ) *wallThickness/2;
                        else wallCoordinate[j] = solver->boundary ( b[j] ).p[coord[j]];
                        j++;
                } else if ( firstSph ) {
                        A=AS=/*AT=*/ positionBufferCurrent[cell->vertex(g)->info().id()].pos;
                        firstSph=false;
                } else {
                        B=BS=/*BT=*/ positionBufferCurrent[cell->vertex(g)->info().id()].pos;;
                }
        }
        AS[coord[0]]=BS[coord[0]] = wallCoordinate[0];

        //first pyramid with triangular base (A,B,BS)
        Real Vol1=0.5* ( ( A-BS ).cross ( B-BS ) ) [coord[1]]* ( 0.333333333* ( 2*B[coord[1]]+A[coord[1]] )-wallCoordinate[1] );
        //second pyramid with triangular base (A,AS,BS)
        Real Vol2=0.5* ( ( AS-BS ).cross ( A-BS ) ) [coord[1]]* ( 0.333333333* ( B[coord[1]]+2*A[coord[1]] )-wallCoordinate[1] );
        return abs ( Vol1+Vol2 );
}
template<class Cellhandle>
Real FlowEngine::volumeCellTripleFictious ( Cellhandle cell )
{
        Vector3r A;

        int b[3];
        int coord[3];
        Real wallCoordinate[3];
        int j=0;
        cell->info().volumeSign=1;

        for ( int g=0;g<4;g++ ) {
                if ( cell->vertex ( g )->info().isFictious ) {
                        b[j] = cell->vertex ( g )->info().id();
                        coord[j]=solver->boundary ( b[j] ).coordinate;
                        const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
                        if ( !solver->boundary ( b[j] ).useMaxMin ) wallCoordinate[j] = wll->state->pos[coord[j]] + ( solver->boundary ( b[j] ).normal[coord[j]] ) *wallThickness/2;
                        else wallCoordinate[j] = solver->boundary ( b[j] ).p[coord[j]];
                        j++;
                } else {
                        const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
                        A= ( sph->state->pos );
                }
        }
        Real volume = ( A[coord[0]]-wallCoordinate[0] ) * ( A[coord[1]]-wallCoordinate[1] ) * ( A[coord[2]]-wallCoordinate[2] );
        return abs ( volume );
}
template<class Cellhandle>
Real FlowEngine::volumeCell ( Cellhandle cell )
{
	static const Real inv6 = 1/6.;
	const Vector3r& p0 = positionBufferCurrent[cell->vertex ( 0 )->info().id()].pos;
	const Vector3r& p1 = positionBufferCurrent[cell->vertex ( 1 )->info().id()].pos;
	const Vector3r& p2 = positionBufferCurrent[cell->vertex ( 2 )->info().id()].pos;
	const Vector3r& p3 = positionBufferCurrent[cell->vertex ( 3 )->info().id()].pos;
	Real volume = inv6 * ((p0-p1).cross(p0-p2)).dot(p0-p3);
        if ( ! ( cell->info().volumeSign ) ) cell->info().volumeSign= ( volume>0 ) ?1:-1;
        return volume;
}
template<class Solver>
void FlowEngine::computeLubricationContributions ( Solver& flow )
{
	if (normalLubrication || shearLubrication || viscousShear){
		if ( debug ) cout << "Application of viscous forces" << endl;
		if ( debug ) cout << "Number of edges = " << flow.edgeIds.size() << endl;
		for ( unsigned int k=0; k<flow.shearLubricationForces.size(); k++ ) flow.shearLubricationForces[k]=Vector3r::Zero();
		for ( unsigned int k=0; k<flow.shearLubricationTorques.size(); k++ ) flow.shearLubricationTorques[k]=Vector3r::Zero();
		for ( unsigned int k=0; k<flow.pumpLubricationTorques.size(); k++ ) flow.pumpLubricationTorques[k]=Vector3r::Zero();
		for ( unsigned int k=0; k<flow.twistLubricationTorques.size(); k++ ) flow.twistLubricationTorques[k]=Vector3r::Zero();
		for ( unsigned int k=0; k<flow.shearLubricationBodyStress.size(); k++) flow.shearLubricationBodyStress[k]=Matrix3r::Zero();
		for ( unsigned int k=0; k<flow.normalLubricationForce.size(); k++ ) flow.normalLubricationForce[k]=Vector3r::Zero();
		for ( unsigned int k=0; k<flow.normalLubricationBodyStress.size(); k++) flow.normalLubricationBodyStress[k]=Matrix3r::Zero();

		typedef typename Solver::Tesselation Tesselation;
		const Tesselation& Tes = flow.T[flow.currentTes];
		flow.deltaShearVel.clear(); flow.normalV.clear(); flow.deltaNormVel.clear(); flow.surfaceDistance.clear(); flow.onlySpheresInteractions.clear(); flow.normalStressInteraction.clear(); flow.shearStressInteraction.clear();


		for ( int i=0; i< ( int ) flow.edgeIds.size(); i++ ) {
			const int& id1 = flow.edgeIds[i].first;
			const int& id2 = flow.edgeIds[i].second;
			
			int hasFictious= Tes.vertex ( id1 )->info().isFictious +  Tes.vertex ( id2 )->info().isFictious;
			if (hasFictious>0 or id1==id2) continue;
			const shared_ptr<Body>& sph1 = Body::byId ( id1, scene );
			const shared_ptr<Body>& sph2 = Body::byId ( id2, scene );
			Sphere* s1=YADE_CAST<Sphere*> ( sph1->shape.get() );
			Sphere* s2=YADE_CAST<Sphere*> ( sph2->shape.get() );
			const Real& r1 = s1->radius;
			const Real& r2 = s2->radius;
			Vector3r deltaV; Real deltaNormV; Vector3r deltaShearV;
			Vector3r O1O2Vector; Real O1O2; Vector3r normal; Real surfaceDist; Vector3r O1CVector; Vector3r O2CVector;Real meanRad ;Real Rh; Vector3r deltaAngVel; Vector3r deltaShearAngVel;
			Vector3r shearLubF; Vector3r normaLubF; Vector3r pumpT; Vector3r deltaAngNormVel; Vector3r twistT; Vector3r angVel1; Vector3r angVel2; 
		//FIXME: if periodic and velGrad!=0, then deltaV should account for velGrad, not the case currently
			if ( !hasFictious ){
				O1O2Vector = sph2->state->pos + makeVector3r(Tes.vertex(id2)->info().ghostShift()) - sph1->state->pos - makeVector3r(Tes.vertex(id1)->info().ghostShift());
				O1O2 = O1O2Vector.norm(); 
				normal= (O1O2Vector/O1O2);
				surfaceDist = O1O2 - r2 - r1;
				O1CVector = (O1O2/2. + (pow(r1,2) - pow(r2,2)) / (2.*O1O2))*normal;
				O2CVector = -(O1O2Vector - O1CVector);
				meanRad = (r2 + r1)/2.;
				Rh = (r1 < r2)? surfaceDist + 0.45 * r1 : surfaceDist + 0.45 * r2;
				deltaV = (sph2->state->vel + sph2->state->angVel.cross(-r2 * normal)) - (sph1->state->vel+ sph1->state->angVel.cross(r1 * normal));
				angVel1 = sph1->state->angVel;
				angVel2 = sph2->state->angVel;
				deltaAngVel = sph2->state->angVel - sph1->state->angVel;

			} else {
				if ( hasFictious==1 ) {//for the fictious sphere, use velocity of the boundary, not of the body
					bool v1fictious = Tes.vertex ( id1 )->info().isFictious;
					int bnd = v1fictious? id1 : id2;
					int coord = flow.boundary(bnd).coordinate;
					O1O2 = v1fictious ? abs((sph2->state->pos + makeVector3r(Tes.vertex(id2)->info().ghostShift()))[coord] - flow.boundary(bnd).p[coord]) : abs((sph1->state->pos + makeVector3r(Tes.vertex(id1)->info().ghostShift()))[coord] - flow.boundary(bnd).p[coord]);
					if(v1fictious)
						normal = makeVector3r(flow.boundary(id1).normal);
					else
						normal = -makeVector3r(flow.boundary(id2).normal);
					O1O2Vector = O1O2 * normal;
					meanRad = v1fictious ? r2:r1;
					surfaceDist = O1O2 - meanRad;
					if (v1fictious){
						O1CVector = Vector3r::Zero();
						O2CVector = - O1O2Vector;}
					else{
						O1CVector =  O1O2Vector;
						O2CVector = Vector3r::Zero();}
				
					Rh = surfaceDist + 0.45 * meanRad;
					Vector3r v1 = ( Tes.vertex ( id1 )->info().isFictious ) ? flow.boundary ( id1 ).velocity:sph1->state->vel + sph1->state->angVel.cross(r1 * normal);
					Vector3r v2 = ( Tes.vertex ( id2 )->info().isFictious ) ? flow.boundary ( id2 ).velocity:sph2->state->vel + sph2->state->angVel.cross(-r2 * (normal));
					deltaV = v2-v1;
					angVel1 = ( Tes.vertex ( id1 )->info().isFictious ) ? Vector3r::Zero() : sph1->state->angVel;
					angVel2 = ( Tes.vertex ( id2 )->info().isFictious ) ? Vector3r::Zero() : sph2->state->angVel;
					deltaAngVel = angVel2 - angVel1;
				}
			}
			deltaShearV = deltaV - ( normal.dot ( deltaV ) ) *normal;
			deltaShearAngVel = deltaAngVel - ( normal.dot ( deltaAngVel ) ) *normal;
			flow.deltaShearVel.push_back(deltaShearV);
			flow.normalV.push_back(normal);
			flow.surfaceDistance.push_back(max(surfaceDist, 0.) + eps*meanRad);

			/// Compute the  shear Lubrication force and torque on each particle
			
			if (shearLubrication)
				shearLubF = flow.computeShearLubricationForce(deltaShearV,surfaceDist,i,eps,O1O2,meanRad);
			else if (viscousShear) 
				shearLubF = flow.computeViscousShearForce ( deltaShearV, i , Rh);
				
			if (viscousShear || shearLubrication){

				flow.shearLubricationForces[id1]+=shearLubF;
				flow.shearLubricationForces[id2]+=(-shearLubF);
				flow.shearLubricationTorques[id1]+=O1CVector.cross(shearLubF);
				flow.shearLubricationTorques[id2]+=O2CVector.cross(-shearLubF);
				
				/// Compute the  pump Lubrication torque on each particle
				
				if (pumpTorque){
					pumpT = flow.computePumpTorque(deltaShearAngVel, surfaceDist, i, eps, meanRad );
					flow.pumpLubricationTorques[id1]+=(-pumpT);
					flow.pumpLubricationTorques[id2]+=pumpT;}
				
				/// Compute the  twist Lubrication torque on each particle
				
				if (twistTorque){
					deltaAngNormVel = (normal.dot(deltaAngVel))*normal ;
					twistT = flow.computeTwistTorque(deltaAngNormVel, surfaceDist, i, eps, meanRad );
					flow.twistLubricationTorques[id1]+=(-twistT);
					flow.twistLubricationTorques[id2]+=twistT;
				}
			}		
			/// Compute the viscous shear stress on each particle
			
			if (viscousShearBodyStress){
				flow.shearLubricationBodyStress[id1] += shearLubF * O1CVector.transpose()/ (4.0/3.0 *3.14* pow(r1,3));
				flow.shearLubricationBodyStress[id2] += (-shearLubF) * O2CVector.transpose()/ (4.0/3.0 *3.14* pow(r2,3));
				flow.shearStressInteraction.push_back(shearLubF * O1O2Vector.transpose()/(4.0/3.0 *3.14* pow(r1,3)));
				}

			/// Compute the normal lubrication force applied on each particle
			
			if (normalLubrication){
				deltaNormV = normal.dot(deltaV);
				flow.deltaNormVel.push_back(deltaNormV * normal);
				normaLubF = flow.computeNormalLubricationForce (deltaNormV, surfaceDist, i,eps,stiffness,scene->dt,meanRad)*normal;
				flow.normalLubricationForce[id1]+=normaLubF;
				flow.normalLubricationForce[id2]+=(-normaLubF);

				/// Compute the normal lubrication stress on each particle
				
				if (viscousNormalBodyStress){
					flow.normalLubricationBodyStress[id1] += normaLubF * O1CVector.transpose()/ (4.0/3.0 *3.14* pow(r1,3));
					flow.normalLubricationBodyStress[id2] += (-normaLubF) *O2CVector.transpose() / (4.0/3.0 *3.14* pow(r2,3));
					flow.normalStressInteraction.push_back(normaLubF * O1O2Vector.transpose()/(4.0/3.0 *3.14* pow(r1,3)));
				}
			}
			
			if (!hasFictious)
				flow.onlySpheresInteractions.push_back(i);
		}
	}
}

YADE_PLUGIN ( ( FlowEngine ) );

//______________________________________________________________

//___________________ PERIODIC VERSION _________________________
//______________________________________________________________

PeriodicFlowEngine::~PeriodicFlowEngine(){}

void PeriodicFlowEngine:: action()
{
        if ( !isActivated ) return;
	timingDeltas->start();
	preparePShifts();
	setPositionsBuffer(true);
	if (first) {
		if (multithread) setPositionsBuffer(false);
		cachedCell= Cell(*(scene->cell));
		buildTriangulation(pZero,solver);
		if (solver->errorCode>0) {LOG_INFO("triangulation error, pausing"); Omega::instance().pause(); return;}
		initializeVolumes(solver); backgroundSolver=solver; backgroundCompleted=true;}
//         if ( first ) {Build_Triangulation ( pZero ); updateTriangulation = false; initializeVolumes();}
	
	timingDeltas->checkpoint("Triangulating");
        updateVolumes (solver);
        epsVolCumulative += epsVolMax;
	retriangulationLastIter++;
	if (!updateTriangulation) updateTriangulation = // If not already set true by another function of by the user, check conditions
		(defTolerance>0 && epsVolCumulative > defTolerance) || retriangulationLastIter>meshUpdateInterval;

	timingDeltas->checkpoint("Update_Volumes");

	///Compute flow and and forces here
	if (pressureForce){
		solver->gaussSeidel(scene->dt);
		timingDeltas->checkpoint("Gauss-Seidel");
		solver->computeFacetForcesWithCache();}
	timingDeltas->checkpoint("Compute_Pressure_Forces");

        ///Compute vicscous forces
        scene->forces.sync();
        computeLubricationContributions(*solver);
	timingDeltas->checkpoint("Compute_Viscous_Forces");
	Vector3r force;
	Vector3r torque;
	const Tesselation& Tes = solver->T[solver->currentTes];
	for (int id=0; id<=Tes.maxId; id++){
		assert (Tes.vertexHandles[id] != NULL);
		const Tesselation::VertexInfo& v_info = Tes.vertexHandles[id]->info();
		force =(pressureForce) ? Vector3r ( ( v_info.forces ) [0],v_info.forces[1],v_info.forces[2] ) : Vector3r(0,0,0);
		torque = Vector3r(0,0,0);
                if (shearLubrication || viscousShear){
			force = force +solver->shearLubricationForces[v_info.id()];
			torque = torque +solver->shearLubricationTorques[v_info.id()];
			if (pumpTorque)
				torque = torque +solver->pumpLubricationTorques[v_info.id()];
			if (twistTorque)
				torque = torque +solver->twistLubricationTorques[v_info.id()];
		}
		
		if (normalLubrication)
			force = force + solver->normalLubricationForce[v_info.id()];
		scene->forces.addForce ( v_info.id(), force);
		scene->forces.addTorque ( v_info.id(), torque);
	}
        ///End Compute flow and forces
	timingDeltas->checkpoint("Applying Forces");
	if (multithread && !first) {
		while (updateTriangulation && !backgroundCompleted) { /*cout<<"sleeping..."<<sleeping++<<endl;*/ 	boost::this_thread::sleep(boost::posix_time::microseconds(1000));}
		if (updateTriangulation || ellapsedIter>(0.5*meshUpdateInterval)) {
			if (useSolver==0) LOG_ERROR("background calculations not available for Gauss-Seidel");
			if (fluidBulkModulus>0) solver->interpolate (solver->T[solver->currentTes], backgroundSolver->T[backgroundSolver->currentTes]);
			solver=backgroundSolver;
			backgroundSolver = shared_ptr<FlowSolver> (new FlowSolver);
			//Copy imposed pressures/flow from the old solver
			backgroundSolver->imposedP = vector<pair<CGT::Point,Real> >(solver->imposedP);
			backgroundSolver->imposedF = vector<pair<CGT::Point,Real> >(solver->imposedF);
			setPositionsBuffer(false);
			cachedCell= Cell(*(scene->cell));
			backgroundCompleted=false;
			retriangulationLastIter=ellapsedIter;
			ellapsedIter=0;
			epsVolCumulative=0;
			boost::thread workerThread(&PeriodicFlowEngine::backgroundAction,this);
			workerThread.detach();
			initializeVolumes(solver);
			computeLubricationContributions(*solver);
		}
		else if (debug && !first) {
			if (debug && !backgroundCompleted) cerr<<"still computing solver in the background"<<endl;
			ellapsedIter++;}
	} else {
	        if (updateTriangulation && !first) {
			cachedCell= Cell(*(scene->cell));
			buildTriangulation (pZero, solver);
			initializeVolumes(solver);
			computeLubricationContributions(*solver);
               		updateTriangulation = false;
			epsVolCumulative=0;
                	retriangulationLastIter=0;
                	ReTrg++;}
        }
        first=false;
	timingDeltas->checkpoint("Ending");
}


void PeriodicFlowEngine::backgroundAction()
{
	if (useSolver<1) {LOG_ERROR("background calculations not available for Gauss-Seidel"); return;}
        buildTriangulation (pZero,backgroundSolver);
	//FIXME: GS is computing too much, we need only matrix factorization in fact
	backgroundSolver->gaussSeidel(scene->dt);
	backgroundSolver->computeFacetForcesWithCache(/*onlyCache?*/ true);
// 	boost::this_thread::sleep(boost::posix_time::seconds(10));
	backgroundCompleted = true;
}

void PeriodicFlowEngine::triangulate( shared_ptr<FlowSolver>& flow )
{
        Tesselation& Tes = flow->T[flow->currentTes];
	vector<posData>& buffer = multithread ? positionBufferParallel : positionBufferCurrent;
	FOREACH ( const posData& b, buffer ) {
                if ( !b.exists || !b.isSphere || b.id==ignoredBody) continue;
                Vector3i period; Vector3r wpos;
		// FIXME: use "sheared" variant if the cell is sheared
		wpos=cachedCell.wrapPt ( b.pos,period );
		const Body::id_t& id = b.id;
		const Real& rad = b.radius;
                const Real& x = wpos[0];
                const Real& y = wpos[1];
                const Real& z = wpos[2];
                VertexHandle vh0=Tes.insert ( x, y, z, rad, id );
//                 VertexHandle vh0=Tes.insert ( b.pos[0], b.pos[1], b.pos[2], b.radius, b.id );
		if (vh0==NULL) {
			flow->errorCode = 2;
			LOG_ERROR("Vh NULL in PeriodicFlowEngine::triangulate(), check input data"); continue;}
		for ( int k=0;k<3;k++ ) vh0->info().period[k]=-period[k];
                const Vector3r cellSize ( cachedCell.getSize() );
		//FIXME: if hasShear, comment in
//                 wpos=scene->cell->unshearPt ( wpos );
                // traverse all periodic cells around the body, to see if any of them touches
                Vector3r halfSize= ( rad+duplicateThreshold ) *Vector3r ( 1,1,1 );
                Vector3r pmin,pmax;
                Vector3i i;
                for ( i[0]=-1; i[0]<=1; i[0]++ )
                        for ( i[1]=-1;i[1]<=1; i[1]++ )
                                for ( i[2]=-1; i[2]<=1; i[2]++ ) {
                                        if ( i[0]!=0 || i[1]!=0 || i[2]!=0 ) { // middle; already rendered above
                                        Vector3r pos2=wpos+Vector3r ( cellSize[0]*i[0],cellSize[1]*i[1],cellSize[2]*i[2] ); // shift, but without shear!
                                        pmin=pos2-halfSize;
                                        pmax=pos2+halfSize;
                                        if ( (pmin[0]<=cellSize[0]) && (pmax[0]>=0) && (pmin[1]<=cellSize[1]) && (pmax[1]>=0) && (pmin[2]<=cellSize[2]) && (pmax[2]>=0) ) {
                                                //with shear:
                                                //Vector3r pt=scene->cell->shearPt ( pos2 );
                                                //without shear:
                                                const Vector3r& pt= pos2;
                                                VertexHandle vh=Tes.insert ( pt[0],pt[1],pt[2],rad,id,false,id );
                                                for ( int k=0;k<3;k++ ) vh->info().period[k]=i[k]-period[k];}}
				}
		//re-assign the original vertex pointer since duplicates may have overwrite it
		Tes.vertexHandles[id]=vh0;
        }
        Tes.redirected=true;//By inserting one-by-one, we already redirected
        flow -> shearLubricationForces.resize ( Tes.maxId+1 );
	flow -> shearLubricationTorques.resize ( Tes.maxId+1 );
	flow -> pumpLubricationTorques.resize ( Tes.maxId+1 );
	flow -> twistLubricationTorques.resize ( Tes.maxId+1 );
	flow -> shearLubricationBodyStress.resize ( Tes.maxId+1 );
	flow -> normalLubricationForce.resize ( Tes.maxId+1 );
	flow -> normalLubricationBodyStress.resize ( Tes.maxId+1 );
}


Real PeriodicFlowEngine::volumeCell ( CellHandle cell )
{
	static const Real inv6 = 1/6.;	
	const Vector3r p0 = positionBufferCurrent[cell->vertex(0)->info().id()].pos + makeVector3r(cell->vertex(0)->info().ghostShift());
	const Vector3r p1 = positionBufferCurrent[cell->vertex(1)->info().id()].pos + makeVector3r(cell->vertex(1)->info().ghostShift());
	const Vector3r p2 = positionBufferCurrent[cell->vertex(2)->info().id()].pos + makeVector3r(cell->vertex(2)->info().ghostShift());
	const Vector3r p3 = positionBufferCurrent[cell->vertex(3)->info().id()].pos + makeVector3r(cell->vertex(3)->info().ghostShift());
	Real volume = inv6*((p0-p1).cross(p0-p2)).dot(p0-p3);
        if ( ! ( cell->info().volumeSign ) ) cell->info().volumeSign= ( volume>0 ) ?1:-1;
        return volume;
}

Real PeriodicFlowEngine::volumeCellSingleFictious ( CellHandle cell )
{
        Vector3r V[3];
        int b=0;
        int w=0;
        cell->info().volumeSign=1;
        Real wallCoordinate=0;

        for ( int y=0;y<4;y++ ) {
                if ( ! ( cell->vertex ( y )->info().isFictious ) ) {
                        const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( y )->info().id(), scene );
                        V[w]=sph->state->pos+ makeVector3r ( cell->vertex ( y )->info().ghostShift() );
                        w++;
                } else {
                        b = cell->vertex ( y )->info().id();
                        const shared_ptr<Body>& wll = Body::byId ( b,scene );
                        if ( !solver->boundary ( b ).useMaxMin ) wallCoordinate = wll->state->pos[solver->boundary ( b ).coordinate]+ ( solver->boundary ( b ).normal[solver->boundary ( b ).coordinate] ) *wallThickness/2;
                        else wallCoordinate = solver->boundary ( b ).p[solver->boundary ( b ).coordinate];
                }
        }
        Real Volume = 0.5* ( ( V[0]-V[1] ).cross ( V[0]-V[2] ) ) [solver->boundary ( b ).coordinate] * ( 0.33333333333* ( V[0][solver->boundary ( b ).coordinate]+ V[1][solver->boundary ( b ).coordinate]+ V[2][solver->boundary ( b ).coordinate] ) - wallCoordinate );
        return abs ( Volume );
}


void PeriodicFlowEngine::locateCell ( CellHandle baseCell, unsigned int& index, int& baseIndex, shared_ptr<FlowSolver>& flow, unsigned int count)
{
        if (count>10) {
		LOG_ERROR("More than 10 attempts to locate a cell, duplicateThreshold may be too small, resulting in periodicity inconsistencies.");
		flow->errorCode=1; return;
	}
	PeriFlowTesselation::CellInfo& baseInfo = baseCell->info();
        //already located, return FIXME: is inline working correctly? else move this test outside the function, just before the calls
	if ( baseInfo.index>0 || baseInfo.isGhost ) return;
	RTriangulation& Tri = flow->T[flow->currentTes].Triangulation();
	Vector3r center ( 0,0,0 );
	Vector3i period;

	if (baseCell->info().fictious()==0)
		for ( int k=0;k<4;k++ ) center+= 0.25*makeVector3r (baseCell->vertex(k)->point());
	else {
		
		Real boundPos=0; int coord=0;
		for ( int k=0;k<4;k++ ) {
			if ( !baseCell->vertex ( k )->info().isFictious ) center+= 0.3333333333*makeVector3r ( baseCell->vertex ( k )->point() );
			else {
				coord=flow->boundary ( baseCell->vertex ( k )->info().id() ).coordinate;
				boundPos=flow->boundary ( baseCell->vertex ( k )->info().id() ).p[coord];}
		}
		center[coord]=boundPos;
	}
	Vector3r wdCenter= cachedCell.wrapPt ( center,period );
	if ( period[0]!=0 || period[1]!=0 || period[2]!=0 ) {
		if ( baseCell->info().index>0 ) {
			cout<<"indexed cell is found ghost!"<<baseInfo.index <<endl;
			baseInfo.isGhost=false;
			return;
		}
		CellHandle ch= Tri.locate ( CGT::Point ( wdCenter[0],wdCenter[1],wdCenter[2] )
// 					     ,/*hint*/ v0
					     );
		baseInfo.period[0]=period[0];
		baseInfo.period[1]=period[1];
		baseInfo.period[2]=period[2];
		//call recursively, since the returned cell could be also a ghost (especially if baseCell is a non-periodic type from the external contour
		locateCell ( ch,index,baseIndex,flow,++count );
		if ( ch==baseCell ) cerr<<"WTF!!"<<endl;
		//check consistency
		bool checkC=false;
		for (int kk=0; kk<4;kk++) if ((!baseCell->vertex(kk)->info().isGhost) && ((!baseCell->vertex(kk)->info().isFictious))) checkC = true;
		if (checkC) {
			bool checkV=true;
			for (int kk=0; kk<4;kk++) {
				checkV=false;
				for (int jj=0; jj<4;jj++)
					if (baseCell->vertex(kk)->info().id() == ch->vertex(jj)->info().id()) checkV = true;
				if (!checkV) {cerr <<"periodicity is broken"<<endl;
				for (int jj=0; jj<4;jj++) cerr<<baseCell->vertex(jj)->info().id()<<" ";
				cerr<<" vs. ";
				for (int jj=0; jj<4;jj++) cerr<<ch->vertex(jj)->info().id()<<" ";
				cerr<<endl;}
			}
		} else {
// 			bool checkV=true;
// 			for (int kk=0; kk<4;kk++) {
// 				checkV=false;
// 				for (int jj=0; jj<4;jj++)
// 					if (baseCell->vertex(kk)->info().id() == ch->vertex(jj)->info().id()) checkV = true;
// 				if (!checkV) {cerr <<"periodicity is broken (that's ok probably)"<<endl;
// 				for (int jj=0; jj<4;jj++) cerr<<baseCell->vertex(jj)->info().id()<<" ";
// 				cerr<<" vs. ";
// 				for (int jj=0; jj<4;jj++) cerr<<ch->vertex(jj)->info().id()<<" ";
// 				cerr<<endl;}
// 			}
		}

		baseInfo.isGhost=true;
		baseInfo._pression=& ( ch->info().p() );
		baseInfo.index=ch->info().index;
		baseInfo.baseIndex=ch->info().baseIndex;
		baseInfo.Pcondition=ch->info().Pcondition;
	} else {
		baseInfo.isGhost=false;
		//index is 1-based, if it is zero it is not initialized, we define it here
		if (  baseInfo.baseIndex<0 ){
			baseInfo.baseIndex=++baseIndex;
			if (!baseInfo.Pcondition) baseInfo.index=++index;}
	}
}

Vector3r PeriodicFlowEngine::meanVelocity()
{
        solver->averageRelativeCellVelocity();
        Vector3r meanVel ( 0,0,0 );
        Real volume=0;
        FiniteCellsIterator cellEnd = solver->T[solver->currentTes].Triangulation().finite_cells_end();
        for ( FiniteCellsIterator cell = solver->T[solver->currentTes].Triangulation().finite_cells_begin(); cell != cellEnd; cell++ ) {
		//We could also define velocity using cell's center
//                 if ( !cell->info().isReal() ) continue;
                if ( cell->info().isGhost ) continue;
                for ( int i=0;i<3;i++ )
                        meanVel[i]=meanVel[i]+ ( ( cell->info().averageVelocity() ) [i] * abs ( cell->info().volume() ) );
                volume+=abs ( cell->info().volume() );
        }
        return ( meanVel/volume );
}

void PeriodicFlowEngine::updateVolumes (shared_ptr<FlowSolver>& flow)
{
        if ( debug ) cout << "Updating volumes.............." << endl;
        Real invDeltaT = 1/scene->dt;
        double newVol, dVol;
        epsVolMax=0;
        Real totVol=0;
        Real totDVol=0;
        Real totVol0=0;
        Real totVol1=0;

	FOREACH(CellHandle& cell, flow->T[flow->currentTes].cellHandles){
                switch ( cell->info().fictious() ) {
                case ( 1 ) :
                        newVol = volumeCellSingleFictious ( cell );
                        totVol1+=newVol;
                        break;
                case ( 0 ) :
                        newVol = volumeCell ( cell );
                        totVol0+=newVol;
                        break;
                default:
                        newVol = 0;
                        break;
                }
                totVol+=newVol;
                dVol=cell->info().volumeSign * ( newVol - cell->info().volume() );
                totDVol+=dVol;
                epsVolMax = max ( epsVolMax, abs ( dVol/newVol ) );
                cell->info().dv() = dVol * invDeltaT;
                cell->info().volume() = newVol;
        }
        if ( debug ) cout << "Updated volumes, total =" <<totVol<<", dVol="<<totDVol<<" "<< totVol0<<" "<< totVol1<<endl;
}


void PeriodicFlowEngine::initializeVolumes (shared_ptr<FlowSolver>& flow)
{
        FiniteVerticesIterator verticesEnd = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
        CGT::CVector Zero ( 0,0,0 );
        for ( FiniteVerticesIterator vIt = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); vIt!= verticesEnd; vIt++ ) vIt->info().forces=Zero;

	FOREACH(CellHandle& cell, flow->T[flow->currentTes].cellHandles){
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = volumeCell ( cell ); break;
			case ( 1 ) : cell->info().volume() = volumeCellSingleFictious ( cell ); break;
			default:  cell->info().volume() = 0; break;
		}
		//FIXME: the void volume is negative sometimes, hence crashing...
		if (flow->fluidBulkModulus>0) { cell->info().invVoidVolume() = 1. / (max(0.1*cell->info().volume(),abs(cell->info().volume()) - flow->volumeSolidPore(cell)) ); }
	}
        if ( debug ) cout << "Volumes initialised." << endl;
}

void PeriodicFlowEngine::buildTriangulation ( double pZero, shared_ptr<FlowSolver>& flow)
{
        flow->resetNetwork();
        if (first) flow->currentTes=0;
        else {
                flow->currentTes=!flow->currentTes;
                if ( debug ) cout << "--------RETRIANGULATION-----------" << endl;}
        initSolver(flow);
        addBoundary ( flow );
        if ( debug ) cout << endl << "Added boundaries------" << endl << endl;
        triangulate (flow);
        if ( debug ) cout << endl << "Tesselating------" << endl << endl;
        flow->T[flow->currentTes].Compute();
        flow->defineFictiousCells();

        //FIXME: this is already done in addBoundary(?)
        boundaryConditions ( flow );
	if ( debug ) cout << endl << "boundaryConditions------" << endl << endl;
        flow->initializePressures ( pZero );
	if ( debug ) cout << endl << "initializePressures------" << endl << endl;
        // Define the ghost cells and add indexes to the cells inside the period (the ones that will contain the pressure unknowns)
        //This must be done after boundary conditions and initialize pressure, else the indexes are not good (not accounting imposedP): FIXME
        unsigned int index=0;
	int baseIndex=-1;
        FlowSolver::Tesselation& Tes = flow->T[flow->currentTes];
	Tes.cellHandles.resize(Tes.Triangulation().number_of_finite_cells());
	const FiniteCellsIterator cellend=Tes.Triangulation().finite_cells_end();
        for ( FiniteCellsIterator cell=Tes.Triangulation().finite_cells_begin(); cell!=cellend; cell++ ){
                locateCell ( cell,index,baseIndex,flow );
		if (flow->errorCode>0) return;
		//Fill this vector than can be later used to speedup loops
		if (!cell->info().isGhost) Tes.cellHandles[cell->info().baseIndex]=cell;
	}
	Tes.cellHandles.resize(baseIndex+1);

	if ( debug ) cout << endl << "locateCell------" << endl << endl;
        flow->computePermeability ( );
        porosity = flow->vPoralPorosity/flow->vTotalePorosity;
        flow->tolerance=tolerance;flow->relax=relax;
	
        flow->displayStatistics ();
        //FIXME: check interpolate() for the periodic case, at least use the mean pressure from previous step.
	if ( !first && !multithread && (useSolver==0 || fluidBulkModulus>0)) flow->interpolate ( flow->T[!flow->currentTes], Tes );
// 	if ( !first && (useSolver==0 || fluidBulkModulus>0)) flow->interpolate ( flow->T[!flow->currentTes], flow->T[flow->currentTes] );
	
        if ( waveAction ) flow->applySinusoidalPressure ( Tes.Triangulation(), sineMagnitude, sineAverage, 30 );

        if (normalLubrication || shearLubrication || viscousShear) flow->computeEdgesSurfaces();
	if ( debug ) cout << endl << "end buildTri------" << endl << endl;
}

void PeriodicFlowEngine::preparePShifts()
{
	CGT::PeriodicCellInfo::gradP = makeCgVect ( gradP );
        CGT::PeriodicCellInfo::hSize[0] = makeCgVect ( scene->cell->hSize.col ( 0 ) );
        CGT::PeriodicCellInfo::hSize[1] = makeCgVect ( scene->cell->hSize.col ( 1 ) );
        CGT::PeriodicCellInfo::hSize[2] = makeCgVect ( scene->cell->hSize.col ( 2 ) );
        CGT::PeriodicCellInfo::deltaP=CGT::CVector (
                                              CGT::PeriodicCellInfo::hSize[0]*CGT::PeriodicCellInfo::gradP,
                                              CGT::PeriodicCellInfo::hSize[1]*CGT::PeriodicCellInfo::gradP,
                                              CGT::PeriodicCellInfo::hSize[2]*CGT::PeriodicCellInfo::gradP );
}


YADE_PLUGIN((PeriodicFlowEngine));

#endif //FLOW_ENGINE

#endif /* YADE_CGAL */

