/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano                               *
*  emanuele.catalano@hmg.inpg.fr                                         *
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

#include "FlowEngine.hpp"

CREATE_LOGGER ( FlowEngine );
CREATE_LOGGER ( PeriodicFlowEngine );

CGT::Vecteur makeCgVect ( const Vector3r& yv ) {return CGT::Vecteur ( yv[0],yv[1],yv[2] );}
CGT::Point makeCgPoint ( const Vector3r& yv ) {return CGT::Point ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r ( const CGT::Point& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}
Vector3r makeVector3r ( const CGT::Vecteur& yv ) {return Vector3r ( yv[0],yv[1],yv[2] );}


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
        if (first) {Build_Triangulation(P_zero,solver); Initialize_volumes(solver); backgroundSolver=solver;}

        timingDeltas->checkpoint ( "Triangulating" );
	UpdateVolumes ( solver );
        timingDeltas->checkpoint ( "Update_Volumes" );
	
        Eps_Vol_Cumulative += eps_vol_max;
        if ( (EpsVolPercent_RTRG>0 && Eps_Vol_Cumulative > EpsVolPercent_RTRG) || retriangulationLastIter>PermuteInterval) {
                Update_Triangulation = true;
                Eps_Vol_Cumulative=0;
                retriangulationLastIter=0;
                ReTrg++;
        } else  {
		Update_Triangulation = false;
		retriangulationLastIter++;}


        ///Compute flow and and forces here

	solver->GaussSeidel(scene->dt);
        timingDeltas->checkpoint ( "Gauss-Seidel (includes matrix construct and factorization in single-thread mode)" );
        if ( save_mgpost ) solver->MGPost();
        if ( !CachedForces ) solver->ComputeFacetForces();
        else solver->ComputeFacetForcesWithCache();
        timingDeltas->checkpoint ( "Compute_Forces" );
        ///Application of vicscous forces
        scene->forces.sync();
	timingDeltas->checkpoint ( "forces.sync()" );
	if ( viscousShear ) ApplyViscousForces ( *solver );
	timingDeltas->checkpoint ( "viscous forces" );
	Vector3r force;
        Finite_vertices_iterator vertices_end = solver->T[solver->currentTes].Triangulation().finite_vertices_end();
        for ( Finite_vertices_iterator V_it = solver->T[solver->currentTes].Triangulation().finite_vertices_begin(); V_it !=  vertices_end; V_it++ ) {
		force = Vector3r ( ( V_it->info().forces ) [0],V_it->info().forces[1],V_it->info().forces[2] );
                if (viscousShear){
			force = force + solver->viscousShearForces[V_it->info().id()];
			scene->forces.addTorque ( V_it->info().id(), solver->viscousShearTorques[V_it->info().id()]);
		}
		if (normalLubrication)
			force = force + solver-> normLubForce[V_it->info().id()];
		scene->forces.addForce ( V_it->info().id(), force);
        }
        ///End Compute flow and forces
        timingDeltas->checkpoint ( "Applying Forces" );
	int sleeping = 0;
	if (multithread && !first) {
		while (Update_Triangulation && !backgroundCompleted) { /*cout<<"sleeping..."<<sleeping++<<endl;*/ 	boost::this_thread::sleep(boost::posix_time::microseconds(1000));}
		if (Update_Triangulation || ellapsedIter>(0.5*PermuteInterval)) {
			if (Debug) cerr<<"switch flow solver"<<endl;
			if (useSolver==0) LOG_ERROR("background calculations not available for Gauss-Seidel");
			if (fluidBulkModulus>0) solver->Interpolate (solver->T[solver->currentTes], backgroundSolver->T[backgroundSolver->currentTes]);
			solver=backgroundSolver;
			backgroundSolver = shared_ptr<FlowSolver> (new FlowSolver);
			//Copy imposed pressures/flow from the old solver
			backgroundSolver->imposedP = vector<pair<CGT::Point,Real> >(solver->imposedP);
			backgroundSolver->imposedF = vector<pair<CGT::Point,Real> >(solver->imposedF);
			if (Debug) cerr<<"switched"<<endl;
			setPositionsBuffer(false);//set "parallel" buffer for background calculation 
			backgroundCompleted=false;
			retriangulationLastIter=ellapsedIter;
			Update_Triangulation=false;
			ellapsedIter=0;
			boost::thread workerThread(&FlowEngine::backgroundAction,this);
			workerThread.detach();
			if (Debug) cerr<<"backgrounded"<<endl;
			Initialize_volumes(solver);
			if (Debug) cerr<<"volumes initialized"<<endl;
		}
		else {
			if (Debug) cerr<<"still computing solver in the background"<<endl;
			ellapsedIter++;
		}
	} else {
	        if (Update_Triangulation && !first) {
			Build_Triangulation (P_zero, solver);
			Initialize_volumes(solver); 
               		Update_Triangulation = false;}
        }
        if ( velocity_profile ) /*flow->FluidVelocityProfile();*/solver->Average_Fluid_Velocity();
        first=false;
        timingDeltas->checkpoint ( "Triangulate + init volumes" );
}

void FlowEngine::backgroundAction()
{
	if (useSolver<1) {LOG_ERROR("background calculations not available for Gauss-Seidel"); return;}
        Build_Triangulation ( P_zero,backgroundSolver );
	//FIXME: GS is computing too much, we need only matrix factorization in fact
	backgroundSolver->GaussSeidel(scene->dt);
	//FIXME(2): and here we need only cached variables, not forces
	backgroundSolver->ComputeFacetForcesWithCache(/*onlyCache?*/ true);
// 	boost::this_thread::sleep(boost::posix_time::seconds(5));
 	backgroundCompleted = true;
}

template<class Solver>

void FlowEngine::BoundaryConditions ( Solver& flow )
{
        if ( flow->y_min_id>=0 ) {
                flow->boundary ( flow->y_min_id ).flowCondition=Flow_imposed_BOTTOM_Boundary;
                flow->boundary ( flow->y_min_id ).value=Pressure_BOTTOM_Boundary;
                flow->boundary ( flow->y_min_id ).velocity = Vector3r::Zero();
        }
        if ( flow->y_max_id>=0 ) {
                flow->boundary ( flow->y_max_id ).flowCondition=Flow_imposed_TOP_Boundary;
                flow->boundary ( flow->y_max_id ).value=Pressure_TOP_Boundary;
                flow->boundary ( flow->y_max_id ).velocity = topBoundaryVelocity;
        }
        if ( flow->x_max_id>=0 ) {
                flow->boundary ( flow->x_max_id ).flowCondition=Flow_imposed_RIGHT_Boundary;
                flow->boundary ( flow->x_max_id ).value=Pressure_RIGHT_Boundary;
                flow->boundary ( flow->x_max_id ).velocity = Vector3r::Zero();
        }
        if ( flow->x_min_id>=0 ) {
                flow->boundary ( flow->x_min_id ).flowCondition=Flow_imposed_LEFT_Boundary;
                flow->boundary ( flow->x_min_id ).value=Pressure_LEFT_Boundary;
                flow->boundary ( flow->x_min_id ).velocity = Vector3r::Zero();
        }
        if ( flow->z_max_id>=0 ) {
                flow->boundary ( flow->z_max_id ).flowCondition=Flow_imposed_FRONT_Boundary;
                flow->boundary ( flow->z_max_id ).value=Pressure_FRONT_Boundary;
                flow->boundary ( flow->z_max_id ).velocity = Vector3r::Zero();
        }
        if ( flow->z_min_id>=0 ) {
                flow->boundary ( flow->z_min_id ).flowCondition=Flow_imposed_BACK_Boundary;
                flow->boundary ( flow->z_min_id ).value=Pressure_BACK_Boundary;
                flow->boundary ( flow->z_min_id ).velocity = Vector3r::Zero();
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
void FlowEngine::clearImposedPressure ( Solver& flow ) { flow->imposedP.clear(); flow->IPCells.clear();}
template<class Solver>
void FlowEngine::clearImposedFlux ( Solver& flow ) { flow->imposedF.clear(); flow->IFCells.clear();}

template<class Solver>
Real FlowEngine::getCellFlux ( unsigned int cond, const shared_ptr<Solver>& flow )
{
	if ( cond>=flow->imposedP.size() ) {LOG_ERROR ( "Getting flux with cond higher than imposedP size." ); return 0;}
        double flux=0;
        typename Solver::Cell_handle& cell= flow->IPCells[cond];
        for ( int ngb=0;ngb<4;ngb++ ) {
                /*if (!cell->neighbor(ngb)->info().Pcondition)*/
                flux+= cell->info().k_norm() [ngb]* ( cell->info().p()-cell->neighbor ( ngb )->info().p() );
        }
        return flux+cell->info().dv();
}

template<class Solver>
void FlowEngine::initSolver ( Solver& flow )
{
       	flow->Vtotalissimo=0; flow->Vsolid_tot=0; flow->Vporale=0; flow->Ssolid_tot=0;
        flow->SLIP_ON_LATERALS=slip_boundary;
        flow->k_factor = permeability_factor;
        flow->DEBUG_OUT = Debug;
        flow->useSolver = useSolver;
	#ifdef EIGENSPARSE_LIB
	flow->numSolveThreads = numSolveThreads;
	flow->numFactorizeThreads = numFactorizeThreads;
	#endif
	flow->meanKStat = meanKStat;
        flow->VISCOSITY = viscosity;
        flow->areaR2Permeability=areaR2Permeability;
        flow->TOLERANCE=Tolerance;
        flow->RELAX=Relax;
        flow->clampKValues = clampKValues;
	flow->maxKdivKmean = maxKdivKmean;
	flow->minKdivKmean = minKdivKmean;
        flow->meanKStat = meanKStat;
        flow->permeability_map = permeability_map;
        flow->fluidBulkModulus = fluidBulkModulus;
        flow->T[flow->currentTes].Clear();
        flow->T[flow->currentTes].max_id=-1;
        flow->x_min = 1000.0, flow->x_max = -10000.0, flow->y_min = 1000.0, flow->y_max = -10000.0, flow->z_min = 1000.0, flow->z_max = -10000.0;
}

template<class Solver>
void FlowEngine::Build_Triangulation ( Solver& flow )
{
        Build_Triangulation ( 0.f,flow );
}

template<class Solver>
void FlowEngine::Build_Triangulation ( double P_zero, Solver& flow )
{
        flow->ResetNetwork();
	if (first) flow->currentTes=0;
        else {
                flow->currentTes=!flow->currentTes;
                if (Debug) cout << "--------RETRIANGULATION-----------" << endl;
        }

	initSolver(flow);

        AddBoundary ( flow );
        Triangulate ( flow );
        if ( Debug ) cout << endl << "Tesselating------" << endl << endl;
        flow->T[flow->currentTes].Compute();

        flow->Define_fictious_cells();
	// For faster loops on cells define this vector
	flow->T[flow->currentTes].cellHandles.clear();
	Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
	for ( Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
		flow->T[flow->currentTes].cellHandles.push_back(cell);
        flow->DisplayStatistics ();
        flow->Compute_Permeability ( );

        porosity = flow->V_porale_porosity/flow->V_totale_porosity;

        if ( compute_K ) {BoundaryConditions ( flow ); K = flow->Sample_Permeability ( flow->x_min, flow->x_max, flow->y_min, flow->y_max, flow->z_min, flow->z_max );}

        BoundaryConditions ( flow );
        flow->Initialize_pressures ( P_zero );
	
        if ( !first && !multithread && (useSolver==0 || fluidBulkModulus>0)) flow->Interpolate ( flow->T[!flow->currentTes], flow->T[flow->currentTes] );
        if ( WaveAction ) flow->ApplySinusoidalPressure ( flow->T[flow->currentTes].Triangulation(), Sinus_Amplitude, Sinus_Average, 30 );

        if ( viscousShear ) flow->ComputeEdgesSurfaces();
	if ( normalLubrication ) flow->ComputeEdgesSurfaces();
}

void FlowEngine::setPositionsBuffer(bool current)
{
	vector<posData>& buffer = current? positionBufferCurrent : positionBufferParallel;
	buffer.clear();
	buffer.resize(scene->bodies->size());
	shared_ptr<Sphere> sph ( new Sphere );
        const int Sph_Index = sph->getClassIndexStatic();
	FOREACH ( const shared_ptr<Body>& b, *scene->bodies ) {
                if (!b || (unlikely(ignoredBody>=0) && ignoredBody==b->getId())) continue;
                posData& dat = buffer[b->getId()];
		dat.id=b->getId();
		dat.pos=b->state->pos;
		dat.isSphere= (b->shape->getClassIndex() ==  Sph_Index);
		if (dat.isSphere) dat.radius = YADE_CAST<Sphere*>(b->shape.get())->radius;
		dat.exists=true;
	}
}

template<class Solver>
void FlowEngine::AddBoundary ( Solver& flow )
{
	vector<posData>& buffer = multithread ? positionBufferParallel : positionBufferCurrent;
        solver->x_min = Mathr::MAX_REAL, solver->x_max = -Mathr::MAX_REAL, solver->y_min = Mathr::MAX_REAL, solver->y_max = -Mathr::MAX_REAL, solver->z_min = Mathr::MAX_REAL, solver->z_max = -Mathr::MAX_REAL;
        FOREACH ( const posData& b, buffer ) {
                if ( !b.exists ) continue;
                if ( b.isSphere ) {
                        const Real& rad = b.radius;
                        const Real& x = b.pos[0];
                        const Real& y = b.pos[1];
                        const Real& z = b.pos[2];
                        flow->x_min = min ( flow->x_min, x-rad );
                        flow->x_max = max ( flow->x_max, x+rad );
                        flow->y_min = min ( flow->y_min, y-rad );
                        flow->y_max = max ( flow->y_max, y+rad );
                        flow->z_min = min ( flow->z_min, z-rad );
                        flow->z_max = max ( flow->z_max, z+rad );
                }
        }
	//FIXME id_offset must be set correctly, not the case here (always 0), then we need walls first or it will fail
        id_offset = flow->T[flow->currentTes].max_id+1;
        flow->id_offset = id_offset;
        flow->SectionArea = ( flow->x_max - flow->x_min ) * ( flow->z_max-flow->z_min );
        flow->Vtotale = ( flow->x_max-flow->x_min ) * ( flow->y_max-flow->y_min ) * ( flow->z_max-flow->z_min );
        flow->y_min_id=wallBottomId;
        flow->y_max_id=wallTopId;
        flow->x_max_id=wallRightId;
        flow->x_min_id=wallLeftId;
        flow->z_min_id=wallBackId;
        flow->z_max_id=wallFrontId;

        if ( flow->y_min_id>=0 ) flow->boundary ( flow->y_min_id ).useMaxMin = BOTTOM_Boundary_MaxMin;
        if ( flow->y_max_id>=0 ) flow->boundary ( flow->y_max_id ).useMaxMin = TOP_Boundary_MaxMin;
        if ( flow->x_max_id>=0 ) flow->boundary ( flow->x_max_id ).useMaxMin = RIGHT_Boundary_MaxMin;
        if ( flow->x_min_id>=0 ) flow->boundary ( flow->x_min_id ).useMaxMin = LEFT_Boundary_MaxMin;
        if ( flow->z_max_id>=0 ) flow->boundary ( flow->z_max_id ).useMaxMin = FRONT_Boundary_MaxMin;
        if ( flow->z_min_id>=0 ) flow->boundary ( flow->z_min_id ).useMaxMin = BACK_Boundary_MaxMin;

        //FIXME: Id's order in boundsIds is done according to the enumeration of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
        flow->boundsIds[0]= &flow->x_min_id;
        flow->boundsIds[1]= &flow->x_max_id;
        flow->boundsIds[2]= &flow->y_min_id;
        flow->boundsIds[3]= &flow->y_max_id;
        flow->boundsIds[4]= &flow->z_min_id;
        flow->boundsIds[5]= &flow->z_max_id;

        flow->Corner_min = CGT::Point ( flow->x_min, flow->y_min, flow->z_min );
        flow->Corner_max = CGT::Point ( flow->x_max, flow->y_max, flow->z_max );

        if ( Debug ) {
                cout << "Section area = " << flow->SectionArea << endl;
                cout << "Vtotale = " << flow->Vtotale << endl;
                cout << "x_min = " << flow->x_min << endl;
                cout << "x_max = " << flow->x_max << endl;
                cout << "y_max = " << flow->y_max << endl;
                cout << "y_min = " << flow->y_min << endl;
                cout << "z_min = " << flow->z_min << endl;
                cout << "z_max = " << flow->z_max << endl;
                cout << endl << "Adding Boundary------" << endl;
        }
        //assign BCs types and values
        BoundaryConditions ( flow );

        double center[3];
        for ( int i=0; i<6; i++ ) {
                if ( *flow->boundsIds[i]<0 ) continue;
                CGT::Vecteur Normal ( normal[i].x(), normal[i].y(), normal[i].z() );
                if ( flow->boundary ( *flow->boundsIds[i] ).useMaxMin ) flow->AddBoundingPlane ( true, Normal, *flow->boundsIds[i] );
                else {
			for ( int h=0;h<3;h++ ) center[h] = buffer[*flow->boundsIds[i]].pos[h];
                        flow->AddBoundingPlane ( center, wall_thickness, Normal,*flow->boundsIds[i] );
                }
        }
}

template<class Solver>
void FlowEngine::Triangulate ( Solver& flow )
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
        flow->viscousShearForces.resize ( flow->T[flow->currentTes].max_id+1 );
	flow->viscousShearTorques.resize ( flow->T[flow->currentTes].max_id+1 );
	flow->viscousBodyStress.resize ( flow->T[flow->currentTes].max_id+1 );
	flow->normLubForce.resize ( flow->T[flow->currentTes].max_id+1 );
	flow->lubBodyStress.resize ( flow->T[flow->currentTes].max_id+1 );
}
template<class Solver>
void FlowEngine::Initialize_volumes ( Solver& flow )
{
	typedef typename Solver::element_type Flow;
	typedef typename Flow::Finite_vertices_iterator Finite_vertices_iterator;
	typedef typename Solver::element_type Flow;
	typedef typename Flow::Finite_cells_iterator Finite_cells_iterator;
	
	Finite_vertices_iterator vertices_end = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
	CGT::Vecteur Zero(0,0,0);
	for (Finite_vertices_iterator V_it = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); V_it!= vertices_end; V_it++) V_it->info().forces=Zero;

	FOREACH(Cell_handle& cell, flow->T[flow->currentTes].cellHandles)
	{
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = Volume_cell ( cell ); break;
			case ( 1 ) : cell->info().volume() = Volume_cell_single_fictious ( cell ); break;
			case ( 2 ) : cell->info().volume() = Volume_cell_double_fictious ( cell ); break;
			case ( 3 ) : cell->info().volume() = Volume_cell_triple_fictious ( cell ); break;
			default: break; 
		}

		if (flow->fluidBulkModulus>0) { cell->info().invVoidVolume() = 1 / ( abs(cell->info().volume()) - flow->volumeSolidPore(cell) ); }
	}
	if (Debug) cout << "Volumes initialised." << endl;
}

void FlowEngine::Average_real_cell_velocity()
{
        solver->Average_Relative_Cell_Velocity();
        Vector3r Vel ( 0,0,0 );
        //AVERAGE CELL VELOCITY
        Finite_cells_iterator cell_end = solver->T[solver->currentTes].Triangulation().finite_cells_end();
        for ( Finite_cells_iterator cell = solver->T[solver->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
                for ( int g=0;g<4;g++ ) {
                        if ( !cell->vertex ( g )->info().isFictious ) {
                                const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
                                for ( int i=0;i<3;i++ ) Vel[i] = Vel[i] + sph->state->vel[i]/4;
                        }
                }
                RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
                CGT::Point pos_av_facet;
                double volume_facet_translation = 0;
                CGT::Vecteur Vel_av ( Vel[0], Vel[1], Vel[2] );
                for ( int i=0; i<4; i++ ) {
                        volume_facet_translation = 0;
                        if ( !Tri.is_infinite ( cell->neighbor ( i ) ) ) {
                                CGT::Vecteur Surfk = cell->info()-cell->neighbor ( i )->info();
                                Real area = sqrt ( Surfk.squared_length() );
                                Surfk = Surfk/area;
                                CGT::Vecteur branch = cell->vertex ( facetVertices[i][0] )->point() - cell->info();
                                pos_av_facet = ( CGT::Point ) cell->info() + ( branch*Surfk ) *Surfk;
                                volume_facet_translation += Vel_av*cell->info().facetSurfaces[i];
                                cell->info().av_vel() = cell->info().av_vel() - volume_facet_translation/cell->info().volume() * ( pos_av_facet-CGAL::ORIGIN );
                        }
                }
        }
}
template<class Solver>
void FlowEngine::UpdateVolumes ( Solver& flow )
{
        if ( Debug ) cout << "Updating volumes.............." << endl;
        Real invDeltaT = 1/scene->dt;

        double newVol, dVol;
        eps_vol_max=0;
        Real totVol=0; Real totDVol=0; Real totVol0=0; Real totVol1=0; Real totVol2=0; Real totVol3=0;
	FOREACH(Cell_handle& cell, flow->T[flow->currentTes].cellHandles){
                switch ( cell->info().fictious() ) {
                case ( 3 ) : newVol = Volume_cell_triple_fictious ( cell ); totVol3+=newVol; break;
                case ( 2 ) : newVol = Volume_cell_double_fictious ( cell ); totVol2+=newVol; break;
                case ( 1 ) : newVol = Volume_cell_single_fictious ( cell ); totVol1+=newVol; break;
		case ( 0 ) : newVol = Volume_cell (cell ); totVol0+=newVol; break;
                default: newVol = 0; break;
                }
                totVol+=newVol;
                dVol=cell->info().volumeSign* ( newVol - cell->info().volume() );
                totDVol+=dVol;
                eps_vol_max = max ( eps_vol_max, abs ( dVol/newVol ) );
                cell->info().dv() = dVol*invDeltaT;
                cell->info().volume() = newVol;
        }
        for (unsigned int n=0; n<flow->imposedF.size();n++) {
		flow->IFCells[n]->info().dv()+=flow->imposedF[n].second;
		flow->IFCells[n]->info().Pcondition=false;}
        if ( Debug ) cout << "Updated volumes, total =" <<totVol<<", dVol="<<totDVol<<endl;
}
template<class Cellhandle>
Real FlowEngine::Volume_cell_single_fictious ( Cellhandle cell )
{
	#if 0
	//Without buffer
        Vector3r V[3];
        int b=0;
        int w=0;
        cell->info().volumeSign=1;
        Real Wall_coordinate=0;

        for ( int y=0;y<4;y++ ) {
                if ( ! ( cell->vertex ( y )->info().isFictious ) ) {
                        const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( y )->info().id(), scene );
                        V[w]=sph->state->pos;
                        w++;
                } else {
                        b = cell->vertex ( y )->info().id();
                        const shared_ptr<Body>& wll = Body::byId ( b , scene );
                        if ( !solver->boundary ( b ).useMaxMin ) Wall_coordinate = wll->state->pos[solver->boundary ( b ).coordinate]+ ( solver->boundary ( b ).normal[solver->boundary ( b ).coordinate] ) *wall_thickness/2;
                        else Wall_coordinate = solver->boundary ( b ).p[solver->boundary ( b ).coordinate];
                }
        }
        Real Volume = 0.5* ( ( V[0]-V[1] ).cross ( V[0]-V[2] ) ) [solver->boundary ( b ).coordinate] * ( 0.33333333333* ( V[0][solver->boundary ( b ).coordinate]+ V[1][solver->boundary ( b ).coordinate]+ V[2][solver->boundary ( b ).coordinate] ) - Wall_coordinate );
	#else
	//With buffer
        Vector3r V[3];
        int b=0;
        int w=0;
        cell->info().volumeSign=1;
        Real Wall_coordinate=0;

        for ( int y=0;y<4;y++ ) {
                if ( ! ( cell->vertex ( y )->info().isFictious ) ) {
//                         const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( y )->info().id(), scene );
                        V[w]=positionBufferCurrent[cell->vertex ( y )->info().id()].pos;
			w++;
                } else {
                        b = cell->vertex ( y )->info().id();
                        const shared_ptr<Body>& wll = Body::byId ( b , scene );
                        if ( !solver->boundary ( b ).useMaxMin ) Wall_coordinate = wll->state->pos[solver->boundary ( b ).coordinate]+ ( solver->boundary ( b ).normal[solver->boundary ( b ).coordinate] ) *wall_thickness/2;
                        else Wall_coordinate = solver->boundary ( b ).p[solver->boundary ( b ).coordinate];
                }
        }
        Real Volume = 0.5* ( ( V[0]-V[1] ).cross ( V[0]-V[2] ) ) [solver->boundary ( b ).coordinate] * ( 0.33333333333* ( V[0][solver->boundary ( b ).coordinate]+ V[1][solver->boundary ( b ).coordinate]+ V[2][solver->boundary ( b ).coordinate] ) - Wall_coordinate );
	#endif

        return abs ( Volume );
}
template<class Cellhandle>
Real FlowEngine::Volume_cell_double_fictious ( Cellhandle cell )
{
        Vector3r A=Vector3r::Zero(), AS=Vector3r::Zero(),B=Vector3r::Zero(), BS=Vector3r::Zero();

        cell->info().volumeSign=1;
        int b[2];
        int coord[2];
        Real Wall_coordinate[2];
        int j=0;
        bool first_sph=true;

        for ( int g=0;g<4;g++ ) {
                if ( cell->vertex ( g )->info().isFictious ) {
                        b[j] = cell->vertex ( g )->info().id();
                        coord[j]=solver->boundary ( b[j] ).coordinate;
//                         const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
                        if ( !solver->boundary ( b[j] ).useMaxMin ) Wall_coordinate[j] = positionBufferCurrent[b[j]].pos[coord[j]] + ( solver->boundary ( b[j] ).normal[coord[j]] ) *wall_thickness/2;
                        else Wall_coordinate[j] = solver->boundary ( b[j] ).p[coord[j]];
                        j++;
                } else if ( first_sph ) {
//                         const shared_ptr<Body>& sph1 = Body::byId ( cell->vertex ( g )->info().id(), scene );
                        A=AS=/*AT=*/ positionBufferCurrent[cell->vertex(g)->info().id()].pos;
                        first_sph=false;
                } else {
//                         const shared_ptr<Body>& sph2 = Body::byId ( cell->vertex ( g )->info().id(), scene );
                        B=BS=/*BT=*/ positionBufferCurrent[cell->vertex(g)->info().id()].pos;;
                }
        }
        AS[coord[0]]=BS[coord[0]] = Wall_coordinate[0];

        //first pyramid with triangular base (A,B,BS)
        Real Vol1=0.5* ( ( A-BS ).cross ( B-BS ) ) [coord[1]]* ( 0.333333333* ( 2*B[coord[1]]+A[coord[1]] )-Wall_coordinate[1] );
        //second pyramid with triangular base (A,AS,BS)
        Real Vol2=0.5* ( ( AS-BS ).cross ( A-BS ) ) [coord[1]]* ( 0.333333333* ( B[coord[1]]+2*A[coord[1]] )-Wall_coordinate[1] );
        return abs ( Vol1+Vol2 );
}
template<class Cellhandle>
Real FlowEngine::Volume_cell_triple_fictious ( Cellhandle cell )
{
        Vector3r A;

        int b[3];
        int coord[3];
        Real Wall_coordinate[3];
        int j=0;
        cell->info().volumeSign=1;

        for ( int g=0;g<4;g++ ) {
                if ( cell->vertex ( g )->info().isFictious ) {
                        b[j] = cell->vertex ( g )->info().id();
                        coord[j]=solver->boundary ( b[j] ).coordinate;
                        const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
                        if ( !solver->boundary ( b[j] ).useMaxMin ) Wall_coordinate[j] = wll->state->pos[coord[j]] + ( solver->boundary ( b[j] ).normal[coord[j]] ) *wall_thickness/2;
                        else Wall_coordinate[j] = solver->boundary ( b[j] ).p[coord[j]];
                        j++;
                } else {
                        const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
                        A= ( sph->state->pos );
                }
        }
        Real Volume = ( A[coord[0]]-Wall_coordinate[0] ) * ( A[coord[1]]-Wall_coordinate[1] ) * ( A[coord[2]]-Wall_coordinate[2] );
        return abs ( Volume );
}
template<class Cellhandle>
Real FlowEngine::Volume_cell ( Cellhandle cell )
{
	static const Real inv6 = 1/6.;
	#if 0
	//Without buffer
	const Vector3r& p0 = Body::byId ( cell->vertex ( 0 )->info().id(), scene )->state->pos;
	const Vector3r& p1 = Body::byId ( cell->vertex ( 1 )->info().id(), scene )->state->pos;
	const Vector3r& p2 = Body::byId ( cell->vertex ( 2 )->info().id(), scene )->state->pos;
	const Vector3r& p3 = Body::byId ( cell->vertex ( 3 )->info().id(), scene )->state->pos;
	#else
	const Vector3r& p0 = positionBufferCurrent[cell->vertex ( 0 )->info().id()].pos;
	const Vector3r& p1 = positionBufferCurrent[cell->vertex ( 1 )->info().id()].pos;
	const Vector3r& p2 = positionBufferCurrent[cell->vertex ( 2 )->info().id()].pos;
	const Vector3r& p3 = positionBufferCurrent[cell->vertex ( 3 )->info().id()].pos;
	#endif
	Real volume = inv6 * ((p0-p1).cross(p0-p2)).dot(p0-p3);
        if ( ! ( cell->info().volumeSign ) ) cell->info().volumeSign= ( volume>0 ) ?1:-1;
        return volume;
}
template<class Solver>
void FlowEngine::ApplyViscousForces ( Solver& flow )
{
//   flow->ComputeEdgesSurfaces(); //only done in buildTriangulation
        if ( Debug ) cout << "Application of viscous forces" << endl;
        if ( Debug ) cout << "Number of edges = " << flow.Edge_ids.size() << endl;
        for ( unsigned int k=0; k<flow.viscousShearForces.size(); k++ ) flow.viscousShearForces[k]=Vector3r::Zero();
	for ( unsigned int k=0; k<flow.viscousShearTorques.size(); k++ ) flow.viscousShearTorques[k]=Vector3r::Zero();
	for ( unsigned int k=0; k<flow.viscousBodyStress.size(); k++) flow.viscousBodyStress[k]=Matrix3r::Zero();
	for ( unsigned int k=0; k<flow.normLubForce.size(); k++ ) flow.normLubForce[k]=Vector3r::Zero();
	for ( unsigned int k=0; k<flow.lubBodyStress.size(); k++) flow.lubBodyStress[k]=Matrix3r::Zero();

	typedef typename Solver::Tesselation Tesselation;
        const Tesselation& Tes = flow.T[flow.currentTes];

        for ( int i=0; i< ( int ) flow.Edge_ids.size(); i++ ) {
                int hasFictious= Tes.vertex ( flow.Edge_ids[i].first )->info().isFictious +  Tes.vertex ( flow.Edge_ids[i].second )->info().isFictious;
                const shared_ptr<Body>& sph1 = Body::byId ( flow.Edge_ids[i].first, scene );
                const shared_ptr<Body>& sph2 = Body::byId ( flow.Edge_ids[i].second, scene );
                Sphere* s1=YADE_CAST<Sphere*> ( sph1->shape.get() );
                Sphere* s2=YADE_CAST<Sphere*> ( sph2->shape.get() );
		Vector3r deltaV; Vector3r deltaNormV; Vector3r deltaShearV;

		Vector3r visc_f; Vector3r lub_f;
                if ( !hasFictious )
                        deltaV = (sph2->state->vel + sph2->state->angVel.cross(s2->radius * flow.Edge_normal[i])) - (sph1->state->vel+ sph1->state->angVel.cross(s1->radius * flow.Edge_normal[i]));
                else {
                        if ( hasFictious==1 ) {//for the fictious sphere, use velocity of the boundary, not of the body
                                Vector3r v1 = ( Tes.vertex ( flow.Edge_ids[i].first )->info().isFictious ) ? flow.boundary ( flow.Edge_ids[i].first ).velocity:sph1->state->vel + sph1->state->angVel.cross(s1->radius * flow.Edge_normal[i]);
                                Vector3r v2 = ( Tes.vertex ( flow.Edge_ids[i].second )->info().isFictious ) ? flow.boundary ( flow.Edge_ids[i].second ).velocity:sph2->state->vel + sph2->state->angVel.cross(s2->radius * (-flow.Edge_normal[i]));
                                deltaV = v2-v1;
                        } else {//both fictious, ignore
                                deltaV = Vector3r::Zero();
                        }
                }
                deltaShearV = deltaV - ( flow.Edge_normal[i].dot ( deltaV ) ) *flow.Edge_normal[i];
		if (shearLubrication)
			visc_f = flow.ComputeShearLubricationForce (deltaShearV,i,eps);
		else
			visc_f = flow.ComputeViscousForce ( deltaShearV, i );
				
//                 if ( Debug ) cout << "la force visqueuse entre " << flow->Edge_ids[i].first << " et " << flow->Edge_ids[i].second << "est " << visc_f << endl;

///    //(1) directement sur le body Yade...
//     scene->forces.addForce(flow->Edge_ids[i].first,visc_f);
//     scene->forces.addForce(flow->Edge_ids[i].second,-visc_f);

///   //(2) ou dans CGAL? On a le choix (on pourrait même avoir info->viscousF pour faire la différence entre les deux types de forces... mais ça prend un peu plus de mémoire et de temps de calcul)
//     Tes.vertex(flow->Edge_ids[i].first)->info().forces=Tes.vertex(flow->Edge_ids[i].first)->info().forces+makeCgVect(visc_f);
//     Tes.vertex(flow->Edge_ids[i].second)->info().forces=Tes.vertex(flow->Edge_ids[i].second)->info().forces+makeCgVect(visc_f);

/// //(3) ou dans un vecteur séparé (rapide)
		flow.viscousShearForces[flow.Edge_ids[i].first]+=visc_f;
		flow.viscousShearForces[flow.Edge_ids[i].second]-=visc_f;
		flow.viscousShearTorques[flow.Edge_ids[i].first]+=flow.Edge_force_point[i].cross(visc_f);
		flow.viscousShearTorques[flow.Edge_ids[i].second]-=(flow.Edge_centerDistVect[i]-flow.Edge_force_point[i]).cross(visc_f);
		
/// Compute the viscous shear stress on each particle
		flow.viscousBodyStress[flow.Edge_ids[i].first] += visc_f * flow.Edge_force_point[i].transpose();
		flow.viscousBodyStress[flow.Edge_ids[i].second] -= visc_f * (flow.Edge_centerDistVect[i]-flow.Edge_force_point[i]).transpose();		
		
/// Compute the normal lubrication force applied on each particle
		if (normalLubrication){
			deltaNormV = (flow.Edge_normal[i].dot (deltaV)) * flow.Edge_normal[i];
			lub_f = flow.ComputeNormalLubricationForce (deltaNormV, i,eps);
			flow.normLubForce[flow.Edge_ids[i].first]+=lub_f;
			flow.normLubForce[flow.Edge_ids[i].second]-=lub_f;
		
/// Compute the normal lubrication stress on each particle
			flow.lubBodyStress[flow.Edge_ids[i].first] += lub_f * flow.Edge_force_point[i].transpose();
			flow.lubBodyStress[flow.Edge_ids[i].second] -= lub_f *(flow.Edge_centerDistVect[i]-flow.Edge_force_point[i]).transpose();
		}
	}
        if(Debug) cout<<"number of viscousShearForce"<<flow.viscousShearForces.size()<<endl;
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
		cachedCell= Cell(*(scene->cell));
		Build_Triangulation(P_zero,solver); Initialize_volumes(solver); backgroundSolver=solver;}
//         if ( first ) {Build_Triangulation ( P_zero ); Update_Triangulation = false; Initialize_volumes();}
	timingDeltas->checkpoint("Triangulating");
        UpdateVolumes (solver);
        Eps_Vol_Cumulative += eps_vol_max;
        if ( Eps_Vol_Cumulative > EpsVolPercent_RTRG || retriangulationLastIter>PermuteInterval ) {
                Update_Triangulation = true;
                Eps_Vol_Cumulative=0;
                retriangulationLastIter=0;
                ReTrg++;
         } else  {
		Update_Triangulation = false;
		retriangulationLastIter++;}
	timingDeltas->checkpoint("Update_Volumes");

	///Compute flow and and forces here

	solver->GaussSeidel(scene->dt);
	timingDeltas->checkpoint("Gauss-Seidel");
	solver->ComputeFacetForcesWithCache();
	timingDeltas->checkpoint("Compute_Pressure_Forces");

        ///Compute vicscous forces
        scene->forces.sync();
        if ( viscousShear ) ApplyViscousForces(*solver);
	timingDeltas->checkpoint("Compute_Viscous_Forces");
	Vector3r force;
	const Tesselation& Tes = solver->T[solver->currentTes];
	for (int id=0; id<=Tes.max_id; id++){
		assert (Tes.vertexHandles[id] != NULL);
		const Tesselation::Vertex_Info& v_info = Tes.vertexHandles[id]->info();
		force = Vector3r ( ( v_info.forces ) [0],v_info.forces[1],v_info.forces[2] );
                if (viscousShear){
			force = force +solver->viscousShearForces[v_info.id()];
			scene->forces.addTorque ( v_info.id(), solver->viscousShearTorques[v_info.id()]);
		}
		if (normalLubrication)
			force = force + solver->normLubForce[v_info.id()];
		scene->forces.addForce ( v_info.id(), force);
	}
        ///End Compute flow and forces
	timingDeltas->checkpoint("Applying Forces");
	if (multithread && !first) {
		while (Update_Triangulation && !backgroundCompleted) { /*cout<<"sleeping..."<<sleeping++<<endl;*/ 	boost::this_thread::sleep(boost::posix_time::microseconds(1000));}
		if (Update_Triangulation || ellapsedIter>(0.5*PermuteInterval)) {
			if (useSolver==0) LOG_ERROR("background calculations not available for Gauss-Seidel");
			if (fluidBulkModulus>0) solver->Interpolate (solver->T[solver->currentTes], backgroundSolver->T[backgroundSolver->currentTes]);
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
			boost::thread workerThread(&PeriodicFlowEngine::backgroundAction,this);
			workerThread.detach();
			Initialize_volumes(solver);
		}
		else if (Debug && !first) {
			cerr<<"still computing solver in the background"<<endl;
			ellapsedIter++;}
	} else {
	        if (Update_Triangulation && !first) {
			cachedCell= Cell(*(scene->cell));
			Build_Triangulation (P_zero, solver);
			Initialize_volumes(solver);
               		Update_Triangulation = false;}
        }
// 	if (velocity_profile) /*flow->FluidVelocityProfile();*/solver->Average_Fluid_Velocity();
        first=false;
	timingDeltas->checkpoint("Ending");
}


void PeriodicFlowEngine::backgroundAction()
{
	if (useSolver<1) {LOG_ERROR("background calculations not available for Gauss-Seidel"); return;}
        Build_Triangulation (P_zero,backgroundSolver);
	//FIXME: GS is computing too much, we need only matrix factorization in fact
	backgroundSolver->GaussSeidel(scene->dt);
	backgroundSolver->ComputeFacetForcesWithCache(/*onlyCache?*/ true);
// 	boost::this_thread::sleep(boost::posix_time::seconds(10));
	backgroundCompleted = true;
}

void PeriodicFlowEngine::Triangulate( shared_ptr<FlowSolver>& flow )
{
//         shared_ptr<Sphere> sph ( new Sphere );
//         int Sph_Index = sph->getClassIndexStatic();
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
                Vertex_handle vh0=Tes.insert ( x, y, z, rad, id );
//                 Vertex_handle vh0=Tes.insert ( b.pos[0], b.pos[1], b.pos[2], b.radius, b.id );
		if (vh0==NULL) LOG_ERROR("Vh NULL in PeriodicFlowEngine::Triangulate(), check input data");
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
                                        if ( i[0]==0 && i[1]==0 && i[2]==0 ) continue; // middle; already rendered above
                                        Vector3r pos2=wpos+Vector3r ( cellSize[0]*i[0],cellSize[1]*i[1],cellSize[2]*i[2] ); // shift, but without shear!
                                        pmin=pos2-halfSize;
                                        pmax=pos2+halfSize;
                                        if ( pmin[0]<=cellSize[0] && pmax[0]>=0 && pmin[1]<=cellSize[1] && pmax[1]>=0 && pmin[2]<=cellSize[2] && pmax[2]>=0 ) {
                                                //with shear:
                                                //Vector3r pt=scene->cell->shearPt ( pos2 );
                                                //without shear:
                                                const Vector3r& pt= pos2;
                                                Vertex_handle vh=Tes.insert ( pt[0],pt[1],pt[2],rad,id,false,id );
                                                for ( int k=0;k<3;k++ ) vh->info().period[k]=i[k]-period[k];}}
		//re-assign the original vertex pointer since duplicates may have overwrite it
		Tes.vertexHandles[id]=vh0;
        }
        Tes.redirected=true;//By inserting one-by-one, we already redirected
        flow -> viscousShearForces.resize ( Tes.max_id+1 );
	flow -> viscousShearTorques.resize ( Tes.max_id+1 );
	flow -> viscousBodyStress.resize ( Tes.max_id+1 );
	flow -> normLubForce.resize ( Tes.max_id+1 );
	flow -> lubBodyStress.resize ( Tes.max_id+1 );
}


Real PeriodicFlowEngine::Volume_cell ( Cell_handle cell )
{
	static const Real inv6 = 1/6.;
	const Vector3r p0 = Body::byId (cell->vertex(0)->info().id(),scene)->state->pos + makeVector3r(cell->vertex(0)->info().ghostShift());
	const Vector3r p1 = Body::byId (cell->vertex(1)->info().id(),scene)->state->pos + makeVector3r(cell->vertex(1)->info().ghostShift());
	const Vector3r p2 = Body::byId (cell->vertex(2)->info().id(),scene)->state->pos + makeVector3r(cell->vertex(2)->info().ghostShift());
	const Vector3r p3 = Body::byId (cell->vertex(3)->info().id(),scene)->state->pos + makeVector3r(cell->vertex(3)->info().ghostShift());
	Real volume = inv6*((p0-p1).cross(p0-p2)).dot(p0-p3);
        if ( ! ( cell->info().volumeSign ) ) cell->info().volumeSign= ( volume>0 ) ?1:-1;
        return volume;
}

Real PeriodicFlowEngine::Volume_cell_single_fictious ( Cell_handle cell )
{
        Vector3r V[3];
        int b=0;
        int w=0;
        cell->info().volumeSign=1;
        Real Wall_coordinate=0;

        for ( int y=0;y<4;y++ ) {
                if ( ! ( cell->vertex ( y )->info().isFictious ) ) {
                        const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( y )->info().id(), scene );
                        V[w]=sph->state->pos+ makeVector3r ( cell->vertex ( y )->info().ghostShift() );
                        w++;
                } else {
                        b = cell->vertex ( y )->info().id();
                        const shared_ptr<Body>& wll = Body::byId ( b,scene );
                        if ( !solver->boundary ( b ).useMaxMin ) Wall_coordinate = wll->state->pos[solver->boundary ( b ).coordinate]+ ( solver->boundary ( b ).normal[solver->boundary ( b ).coordinate] ) *wall_thickness/2;
                        else Wall_coordinate = solver->boundary ( b ).p[solver->boundary ( b ).coordinate];
                }
        }
        Real Volume = 0.5* ( ( V[0]-V[1] ).cross ( V[0]-V[2] ) ) [solver->boundary ( b ).coordinate] * ( 0.33333333333* ( V[0][solver->boundary ( b ).coordinate]+ V[1][solver->boundary ( b ).coordinate]+ V[2][solver->boundary ( b ).coordinate] ) - Wall_coordinate );
        return abs ( Volume );
}


void PeriodicFlowEngine::locateCell ( Cell_handle baseCell, unsigned int& index, int& baseIndex, shared_ptr<FlowSolver>& flow, unsigned int count)
{
        if (count>10) LOG_ERROR("More than 10 attempts to locate a cell, duplicateThreshold may be too small, resulting in periodicity inconsistencies.");
	PeriFlowTesselation::Cell_Info& base_info = baseCell->info();
        //already located, return FIXME: is inline working correctly? else move this test outside the function, just before the calls
	if ( base_info.index>0 || base_info.isGhost ) return;
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
			cout<<"indexed cell is found ghost!"<<base_info.index <<endl;
			base_info.isGhost=false;
			return;
		}
// 		const Vertex_handle& v0 = flow->T[flow->currentTes].vertexHandles[baseCell->vertex(0)->info().id()];
		Cell_handle ch= Tri.locate ( CGT::Point ( wdCenter[0],wdCenter[1],wdCenter[2] )
// 					     ,/*hint*/ v0
					     );
		base_info.period[0]=period[0];
		base_info.period[1]=period[1];
		base_info.period[2]=period[2];
		//call recursively, since the returned cell could be also a ghost (especially if baseCell is a non-periodic type from the external contour
		locateCell ( ch,index,baseIndex,flow,++count );
		if ( ch==baseCell ) cerr<<"WTF!!"<<endl;
		base_info.isGhost=true;
		base_info._pression=& ( ch->info().p() );
		base_info.index=ch->info().index;
		base_info.baseIndex=ch->info().baseIndex;
		base_info.Pcondition=ch->info().Pcondition;
	} else {
		base_info.isGhost=false;
		//index is 1-based, if it is zero it is not initialized, we define it here
		if (  base_info.baseIndex<0 ){
			base_info.baseIndex=++baseIndex;
			if (!base_info.Pcondition) base_info.index=++index;}
	}
}

Vector3r PeriodicFlowEngine::meanVelocity()
{
        solver->Average_Relative_Cell_Velocity();
        Vector3r meanVel ( 0,0,0 );
        Real volume=0;
        Finite_cells_iterator cell_end = solver->T[solver->currentTes].Triangulation().finite_cells_end();
        for ( Finite_cells_iterator cell = solver->T[solver->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
		//We could also define velocity using cell's center
//                 if ( !cell->info().isReal() ) continue;
                if ( cell->info().isGhost ) continue;
                for ( int i=0;i<3;i++ )
                        meanVel[i]=meanVel[i]+ ( ( cell->info().av_vel() ) [i] * abs ( cell->info().volume() ) );
                volume+=abs ( cell->info().volume() );
        }
        return ( meanVel/volume );
}

void PeriodicFlowEngine::UpdateVolumes (shared_ptr<FlowSolver>& flow)
{
        if ( Debug ) cout << "Updating volumes.............." << endl;
        Real invDeltaT = 1/scene->dt;
        double newVol, dVol;
        eps_vol_max=0;
        Real totVol=0;
        Real totDVol=0;
        Real totVol0=0;
        Real totVol1=0;

	FOREACH(Cell_handle& cell, flow->T[flow->currentTes].cellHandles){
                switch ( cell->info().fictious() ) {
                case ( 1 ) :
                        newVol = Volume_cell_single_fictious ( cell );
                        totVol1+=newVol;
                        break;
                case ( 0 ) :
                        newVol = Volume_cell ( cell );
                        totVol0+=newVol;
                        break;
                default:
                        newVol = 0;
                        break;
                }
                totVol+=newVol;
                dVol=cell->info().volumeSign * ( newVol - cell->info().volume() );
                totDVol+=dVol;
                eps_vol_max = max ( eps_vol_max, abs ( dVol/newVol ) );
                cell->info().dv() = dVol * invDeltaT;
                cell->info().volume() = newVol;
        }
        if ( Debug ) cout << "Updated volumes, total =" <<totVol<<", dVol="<<totDVol<<" "<< totVol0<<" "<< totVol1<<endl;
}


void PeriodicFlowEngine::Initialize_volumes (shared_ptr<FlowSolver>& flow)
{
        Finite_vertices_iterator vertices_end = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
        CGT::Vecteur Zero ( 0,0,0 );
        for ( Finite_vertices_iterator V_it = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); V_it!= vertices_end; V_it++ ) V_it->info().forces=Zero;

	FOREACH(Cell_handle& cell, flow->T[flow->currentTes].cellHandles){
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = Volume_cell ( cell ); break;
			case ( 1 ) : cell->info().volume() = Volume_cell_single_fictious ( cell ); break;
// 			case ( 2 ) : cell->info().volume() = Volume_cell_double_fictious ( cell ); break;
// 			case ( 3 ) : cell->info().volume() = Volume_cell_triple_fictious ( cell ); break;
			default:  cell->info().volume() = 0; break;
		}
		//FIXME: the void volume is negative sometimes, hence crashing...
		if (flow->fluidBulkModulus>0) { cell->info().invVoidVolume() = 1. / (max(0.1*cell->info().volume(),abs(cell->info().volume()) - flow->volumeSolidPore(cell)) ); }
	}
        if ( Debug ) cout << "Volumes initialised." << endl;
}

void PeriodicFlowEngine::Build_Triangulation ( double P_zero, shared_ptr<FlowSolver>& flow)
{
        flow->ResetNetwork();
        if (first) flow->currentTes=0;
        else {
                flow->currentTes=!flow->currentTes;
                if ( Debug ) cout << "--------RETRIANGULATION-----------" << endl;}
        initSolver(flow);
        AddBoundary ( flow );
        if ( Debug ) cout << endl << "Added boundaries------" << endl << endl;
        Triangulate (flow);
        if ( Debug ) cout << endl << "Tesselating------" << endl << endl;
        flow->T[flow->currentTes].Compute();
        flow->Define_fictious_cells();

        //FIXME: this is already done in addBoundary(?)
        BoundaryConditions ( flow );
	if ( Debug ) cout << endl << "BoundaryConditions------" << endl << endl;
        flow->Initialize_pressures ( P_zero );
	if ( Debug ) cout << endl << "Initialize_pressures------" << endl << endl;
        // Define the ghost cells and add indexes to the cells inside the period (the ones that will contain the pressure unknowns)
        //This must be done after boundary conditions and initialize pressure, else the indexes are not good (not accounting imposedP): FIXME
        unsigned int index=0;
	int baseIndex=-1;
        FlowSolver::Tesselation& Tes = flow->T[flow->currentTes];
	Tes.cellHandles.resize(Tes.Triangulation().number_of_finite_cells());
	const Finite_cells_iterator cellend=Tes.Triangulation().finite_cells_end();
        for ( Finite_cells_iterator cell=Tes.Triangulation().finite_cells_begin(); cell!=cellend; cell++ ){
                locateCell ( cell,index,baseIndex,flow );
		//Fill this vector than can be later used to speedup loops
		if (!cell->info().isGhost) Tes.cellHandles[cell->info().baseIndex]=cell;
	}
	Tes.cellHandles.resize(baseIndex+1);

	if ( Debug ) cout << endl << "locateCell------" << endl << endl;
        flow->Compute_Permeability ( );
        porosity = flow->V_porale_porosity/flow->V_totale_porosity;
        flow->TOLERANCE=Tolerance;flow->RELAX=Relax;
	
        flow->DisplayStatistics ();
        //FIXME: check interpolate() for the periodic case, at least use the mean pressure from previous step.
	if ( !first && !multithread && (useSolver==0 || fluidBulkModulus>0)) flow->Interpolate ( flow->T[!flow->currentTes], Tes );
// 	if ( !first && (useSolver==0 || fluidBulkModulus>0)) flow->Interpolate ( flow->T[!flow->currentTes], flow->T[flow->currentTes] );
	
        if ( WaveAction ) flow->ApplySinusoidalPressure ( Tes.Triangulation(), Sinus_Amplitude, Sinus_Average, 30 );
        if ( viscousShear ) flow->ComputeEdgesSurfaces();
	if ( normalLubrication ) flow->ComputeEdgesSurfaces();
	if ( Debug ) cout << endl << "end buildTri------" << endl << endl;
}

void PeriodicFlowEngine::preparePShifts()
{
	CGT::PeriodicCellInfo::gradP = makeCgVect ( gradP );
        CGT::PeriodicCellInfo::hSize[0] = makeCgVect ( scene->cell->hSize.col ( 0 ) );
        CGT::PeriodicCellInfo::hSize[1] = makeCgVect ( scene->cell->hSize.col ( 1 ) );
        CGT::PeriodicCellInfo::hSize[2] = makeCgVect ( scene->cell->hSize.col ( 2 ) );
        CGT::PeriodicCellInfo::deltaP=CGT::Vecteur (
                                              CGT::PeriodicCellInfo::hSize[0]*CGT::PeriodicCellInfo::gradP,
                                              CGT::PeriodicCellInfo::hSize[1]*CGT::PeriodicCellInfo::gradP,
                                              CGT::PeriodicCellInfo::hSize[2]*CGT::PeriodicCellInfo::gradP );
}


YADE_PLUGIN((PeriodicFlowEngine));

#endif //FLOW_ENGINE

#endif /* YADE_CGAL */

