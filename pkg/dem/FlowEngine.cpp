/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano                               *
*  emanuele.catalano@hmg.inpg.fr                                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef YADE_CGAL

#ifdef FLOW_ENGINE
#include "FlowEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/lib/base/Math.hpp>
#include<yade/pkg/dem/TesselationWrapper.hpp>
#include<yade/pkg/common/Sphere.hpp>
#include<yade/pkg/common/Wall.hpp>
#include<yade/pkg/common/Box.hpp>

#include <sys/stat.h>
#include <sys/types.h>

CREATE_LOGGER (FlowEngine);

CGT::Vecteur makeCgVect (const Vector3r& yv) {return CGT::Vecteur(yv[0],yv[1],yv[2]);} 
CGT::Point makeCgPoint (const Vector3r& yv) {return CGT::Point(yv[0],yv[1],yv[2]);}

FlowEngine::~FlowEngine()
{
}

const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

void FlowEngine::action()
{
	if (!isActivated) return;
	if (!flow) {
		flow = shared_ptr<FlowSolver> (new FlowSolver);
		first=true;
		cerr <<"first = true"<<endl;
		Update_Triangulation=false;
		eps_vol_max=0.f;
		Eps_Vol_Cumulative=0.f;
		ReTrg=1;
		retriangulationLastIter=0;
	}
	if (!triaxialCompressionEngine)
	{
		vector<shared_ptr<Engine> >::iterator itFirst = scene->engines.begin();
		vector<shared_ptr<Engine> >::iterator itLast = scene->engines.end();
		for (;itFirst!=itLast; ++itFirst) {
			if ((*itFirst)->getClassName() == "TriaxialCompressionEngine") {
// 				cout << "stress controller engine found - FlowEngine" << endl;
				triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> (*itFirst);}}
		if (!triaxialCompressionEngine) cout << "stress controller engine NOT found" << endl;
	}
	currentStress = triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1];
	currentStrain = triaxialCompressionEngine->strain[1];

	timingDeltas->start();

	if (first) Build_Triangulation(P_zero);
	timingDeltas->checkpoint("Triangulating");
	UpdateVolumes ( );
	if (!first) {
// 		eps_vol_max=0.f;//huh? in that case Eps_Vol_Cumulative will always be zero
		Eps_Vol_Cumulative += eps_vol_max;
		if ((Eps_Vol_Cumulative > EpsVolPercent_RTRG || retriangulationLastIter>1000) && retriangulationLastIter>10) {
			Update_Triangulation = true;
			Eps_Vol_Cumulative=0;
			retriangulationLastIter=0;
			ReTrg++;
		} else  retriangulationLastIter++;
		timingDeltas->checkpoint("Update_Volumes");
		///Update boundary conditions
		BoundaryConditions();

		///Compute flow and and forces here
		flow->GaussSeidel();
		timingDeltas->checkpoint("Gauss-Seidel");
		if (save_mgpost) flow->MGPost();
		if (!CachedForces) flow->ComputeFacetForces();
		else flow->ComputeFacetForcesWithCache();
		timingDeltas->checkpoint("Compute_Forces");
		
		///Application of vicscous forces
		scene->forces.sync();
		if (viscousShear) ApplyViscousForces();

		CGT::Finite_vertices_iterator vertices_end = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
		for (CGT::Finite_vertices_iterator V_it = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); V_it !=  vertices_end; V_it++)
		{
			if (!viscousShear)
				scene->forces.addForce(V_it->info().id(), Vector3r ((V_it->info().forces)[0],V_it->info().forces[1],V_it->info().forces[2]));
			else
				scene->forces.addForce(V_it->info().id(), Vector3r((V_it->info().forces)[0],V_it->info().forces[1],V_it->info().forces[2])+flow->viscousShearForces[V_it->info().id()]);
		}
		///End Compute flow and forces
		timingDeltas->checkpoint("Applying Forces");

		Real time = scene->time;
		int j = scene->iter;

		if (consolidation) {
			char file [50];
			mkdir("./Consol", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			string consol = "./Consol/"+flow->key+"%d_Consol";
			const char* keyconsol = consol.c_str();
			sprintf(file, keyconsol, j);
			char *g = file;
			timingDeltas->checkpoint("Writing cons_files");
			MaxPressure = flow->PressureProfile(g, time, intervals);

			std::ofstream max_p("pressures.txt", std::ios::app);
			max_p << j << " " << time << " " << MaxPressure << endl;

			std::ofstream settle("settle.txt", std::ios::app);
			settle << j << " " << time << " " << currentStrain << endl;
		}

		if (slice_pressures){
			char slifile [30];
			mkdir("./Slices", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
			string slice = "./Slices/Slice_"+flow->key+"_%d";
			const char* keyslice = slice.c_str();
			sprintf(slifile, keyslice, j);
			const char *f = "slifile";
			flow->SliceField(f);
		}
// 		if (save_vtk) {flow->save_vtk_file();}
		timingDeltas->checkpoint("Writing files");
	}
// 	if ( scene->iter % PermuteInterval == 0 )
// 	{ Update_Triangulation = true; }

	if (Update_Triangulation && !first) {
		Build_Triangulation();
		Update_Triangulation = false;}
		
	if (velocity_profile) /*flow->FluidVelocityProfile();*/flow->Average_Fluid_Velocity();
	if (first && liquefaction){
	  wall_up_y = flow->y_max;
	  wall_down_y = flow->y_min;}
	if (liquefaction) flow->Measure_Pore_Pressure(wall_up_y, wall_down_y);
	first=false;
	timingDeltas->checkpoint("Ending");
	
// 	if(id_sphere>=0) flow->Average_Fluid_Velocity_On_Sphere(id_sphere);
// }
}

void FlowEngine::BoundaryConditions()
{
	flow->boundary ( flow->y_min_id ).flowCondition=Flow_imposed_BOTTOM_Boundary;
	flow->boundary ( flow->y_max_id ).flowCondition=Flow_imposed_TOP_Boundary;
	flow->boundary ( flow->x_max_id ).flowCondition=Flow_imposed_RIGHT_Boundary;
	flow->boundary ( flow->x_min_id ).flowCondition=Flow_imposed_LEFT_Boundary;
	flow->boundary ( flow->z_max_id ).flowCondition=Flow_imposed_FRONT_Boundary;
	flow->boundary ( flow->z_min_id ).flowCondition=Flow_imposed_BACK_Boundary;

	flow->boundary ( flow->y_min_id ).value=Pressure_BOTTOM_Boundary;
	flow->boundary ( flow->y_max_id ).value=Pressure_TOP_Boundary;
	flow->boundary ( flow->x_max_id ).value=Pressure_RIGHT_Boundary;
	flow->boundary ( flow->x_min_id ).value=Pressure_LEFT_Boundary;
	flow->boundary ( flow->z_max_id ).value=Pressure_FRONT_Boundary;
	flow->boundary ( flow->z_min_id ).value=Pressure_BACK_Boundary;
	
	flow->boundary ( flow->y_min_id ).velocity = Vector3r::Zero();
	flow->boundary ( flow->y_max_id ).velocity = topBoundaryVelocity;
	flow->boundary ( flow->x_max_id ).velocity = Vector3r::Zero();
	flow->boundary ( flow->x_min_id ).velocity = Vector3r::Zero();
	flow->boundary ( flow->z_max_id ).velocity = Vector3r::Zero();
	flow->boundary ( flow->z_min_id ).velocity = Vector3r::Zero();
}


unsigned int FlowEngine::imposePressure(Vector3r pos, Real p)
{
	flow->imposedP.push_back( pair<CGT::Point,Real>(CGT::Point(pos[0],pos[1],pos[2]),p) );
	//force immediate update of boundary conditions
	Update_Triangulation=true;
	return flow->imposedP.size();
}

void FlowEngine::setImposedPressure(unsigned int cond, Real p)
{
	if (cond>=flow->imposedP.size()) LOG_ERROR("Setting p with cond higher than imposedP size.");
	flow->imposedP[cond].second=p;
	//force immediate update of boundary conditions
	Update_Triangulation=true;
}

void FlowEngine::clearImposedPressure() { flow->imposedP.clear();}

Real FlowEngine::getFlux(unsigned int cond) {
	if (cond>=flow->imposedP.size()) LOG_ERROR("Getting flux with cond higher than imposedP size.");
	CGT::RTriangulation& Tri = flow->T[flow->currentTes].Triangulation();
	double flux=0;
	CGT::Cell_handle cell= Tri.locate(flow->imposedP[cond].first);
	for (int ngb=0;ngb<4;ngb++) {
		if (!cell->neighbor(ngb)->info().Pcondition) flux+= cell->info().k_norm()[ngb]*(cell->info().p()-cell->neighbor(ngb)->info().p());}
	return flux;
}


void FlowEngine::Build_Triangulation ()
{
	Build_Triangulation (0.f);
}

void FlowEngine::Build_Triangulation (double P_zero)
{
	flow->ResetNetwork();
	if (first) flow->currentTes=0;
	else {
		flow->currentTes=!flow->currentTes;
		if (Debug) cout << "--------RETRIANGULATION-----------" << endl;}

	flow->Vtotalissimo=0; flow->Vsolid_tot=0; flow->Vporale=0; flow->Ssolid_tot=0;
	flow->SLIP_ON_LATERALS=slip_boundary;
	flow->key = triaxialCompressionEngine->Key;
	flow->k_factor = permeability_factor;
	flow->DEBUG_OUT = Debug;
	flow->useSolver = useSolver;
	flow->VISCOSITY = viscosity;
	flow->areaR2Permeability=areaR2Permeability;

	flow->T[flow->currentTes].Clear();
	flow->T[flow->currentTes].max_id=-1;
	flow->x_min = 1000.0, flow->x_max = -10000.0, flow->y_min = 1000.0, flow->y_max = -10000.0, flow->z_min = 1000.0, flow->z_max = -10000.0;

	AddBoundary ( );
	Triangulate ( );
	if (Debug) cout << endl << "Tesselating------" << endl << endl;
	flow->T[flow->currentTes].Compute();

	flow->Define_fictious_cells();
	flow->DisplayStatistics ();

	flow->meanK_LIMIT = meanK_correction;
	flow->meanK_STAT = meanK_opt;
	flow->permeability_map = permeability_map;
	flow->Compute_Permeability ( );

	porosity = flow->V_porale_porosity/flow->V_totale_porosity;

	if (first)
	{
		flow->TOLERANCE=Tolerance;
		flow->RELAX=Relax;
		CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
		for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ){cell->info().dv() = 0; cell->info().p() = 0;}
		if (compute_K) {K = flow->Sample_Permeability ( flow->x_min, flow->x_max, flow->y_min, flow->y_max, flow->z_min, flow->z_max, flow->key );}
		BoundaryConditions();
		flow->Initialize_pressures( P_zero );
  		if (WaveAction) flow->ApplySinusoidalPressure(flow->T[flow->currentTes].Triangulation(), Sinus_Amplitude, Sinus_Average, 30);
		

	}
	else
	{
		flow->TOLERANCE=Tolerance;
		flow->RELAX=Relax;
		if (Debug && compute_K) cout << "---------UPDATE PERMEABILITY VALUE--------------" << endl;
		CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
		for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ){cell->info().dv() = 0; cell->info().p() = 0;}
		if (compute_K) {K = flow->Sample_Permeability ( flow->x_min, flow->x_max, flow->y_min, flow->y_max, flow->z_min, flow->z_max, flow->key );}
		BoundaryConditions();
		flow->Initialize_pressures(P_zero);// FIXME : why, if we are going to interpolate after that?
		flow->Interpolate (flow->T[!flow->currentTes], flow->T[flow->currentTes]);
 		if (WaveAction) flow->ApplySinusoidalPressure(flow->T[flow->currentTes].Triangulation(), Sinus_Amplitude, Sinus_Average, 30);
	}
	Initialize_volumes();
	if (viscousShear) flow->ComputeEdgesSurfaces();
}

void FlowEngine::AddBoundary ()
{
	shared_ptr<Sphere> sph ( new Sphere );
	int Sph_Index = sph->getClassIndexStatic();

	FOREACH ( const shared_ptr<Body>& b, *scene->bodies )
	{
		if ( !b ) continue;
		if ( b->shape->getClassIndex() ==  Sph_Index )
		{
			Sphere* s=YADE_CAST<Sphere*> ( b->shape.get() );
			//const Body::id_t& id = b->getId();
			Real& rad = s->radius;
			Real& x = b->state->pos[0];
			Real& y = b->state->pos[1];
			Real& z = b->state->pos[2];

			flow->x_min = min ( flow->x_min, x-rad);
			flow->x_max = max ( flow->x_max, x+rad);
			flow->y_min = min ( flow->y_min, y-rad);
			flow->y_max = max ( flow->y_max, y+rad);
			flow->z_min = min ( flow->z_min, z-rad);
			flow->z_max = max ( flow->z_max, z+rad);
		}
	}

	id_offset = flow->T[flow->currentTes].max_id+1;

	flow->id_offset = id_offset;

	flow->SectionArea = ( flow->x_max - flow->x_min ) * ( flow->z_max-flow->z_min );
	flow->Vtotale = (flow->x_max-flow->x_min) * (flow->y_max-flow->y_min) * (flow->z_max-flow->z_min);

	if (triaxialCompressionEngine) {
		flow->y_min_id=triaxialCompressionEngine->wall_bottom_id;
		flow->y_max_id=triaxialCompressionEngine->wall_top_id;
		flow->x_max_id=triaxialCompressionEngine->wall_right_id;
		flow->x_min_id=triaxialCompressionEngine->wall_left_id;
		flow->z_min_id=triaxialCompressionEngine->wall_back_id;
		flow->z_max_id=triaxialCompressionEngine->wall_front_id;
	} else {
		flow->y_min_id=wallBottomId;
		flow->y_max_id=wallTopId;
		flow->x_max_id=wallRightId;
		flow->x_min_id=wallLeftId;
		flow->z_min_id=wallBackId;
		flow->z_max_id=wallFrontId;
	}
	
	flow->boundary ( flow->y_min_id ).useMaxMin = BOTTOM_Boundary_MaxMin;
	flow->boundary ( flow->y_max_id ).useMaxMin = TOP_Boundary_MaxMin;
	flow->boundary ( flow->x_max_id ).useMaxMin = RIGHT_Boundary_MaxMin;
	flow->boundary ( flow->x_min_id ).useMaxMin = LEFT_Boundary_MaxMin;
	flow->boundary ( flow->z_max_id ).useMaxMin = FRONT_Boundary_MaxMin;
	flow->boundary ( flow->z_min_id ).useMaxMin = BACK_Boundary_MaxMin;


	//FIXME: Id's order in boundsIds is done according to the enumeration of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
	flow->boundsIds[0]= &flow->x_min_id;
        flow->boundsIds[1]= &flow->x_max_id;
        flow->boundsIds[2]= &flow->y_min_id;
        flow->boundsIds[3]= &flow->y_max_id;
        flow->boundsIds[4]= &flow->z_min_id;
        flow->boundsIds[5]= &flow->z_max_id;

	wall_thickness = triaxialCompressionEngine->thickness;

	flow->Corner_min = CGT::Point(flow->x_min, flow->y_min, flow->z_min);
	flow->Corner_max = CGT::Point(flow->x_max, flow->y_max, flow->z_max);

	if (Debug) {
	cout << "Section area = " << flow->SectionArea << endl;
	cout << "Vtotale = " << flow->Vtotale << endl;
// 	cout << "Rmoy " << Rmoy << endl;
	cout << "x_min = " << flow->x_min << endl;
	cout << "x_max = " << flow->x_max << endl;
	cout << "y_max = " << flow->y_max << endl;
	cout << "y_min = " << flow->y_min << endl;
	cout << "z_min = " << flow->z_min << endl;
	cout << "z_max = " << flow->z_max << endl;
	cout << endl << "Adding Boundary------" << endl;}
	
	//assign BCs types and values
	BoundaryConditions();

	double center[3];

	for (int i=0; i<6; i++)
	{
	  CGT::Vecteur Normal (triaxialCompressionEngine->normal[i].x(), triaxialCompressionEngine->normal[i].y(), triaxialCompressionEngine->normal[i].z());
	  if (flow->boundary(*flow->boundsIds[i]).useMaxMin) flow->AddBoundingPlane (true, Normal, *flow->boundsIds[i]);
	  else {
            const shared_ptr<Body>& wll = Body::byId ( *flow->boundsIds[i] , scene );
            for ( int h=0;h<3;h++ ){center[h] = wll->state->pos[h];}
            flow->AddBoundingPlane (center, wall_thickness, Normal,*flow->boundsIds[i]);}}

// 	flow->AddBoundingPlanes(true);

}

void FlowEngine::Triangulate ()
{
///Using Tesselation wrapper (faster)
// 	TesselationWrapper TW;
// 	if (TW.Tes) delete TW.Tes;
// 	TW.Tes = &(flow->T[flow->currentTes]);//point to the current Tes we have in Flowengine
// 	TW.insertSceneSpheres();//TW is now really inserting in FlowEngine, using the faster insert(begin,end)
// 	TW.Tes = NULL;//otherwise, Tes would be deleted by ~TesselationWrapper() at the end of the function.
///Using one-by-one insertion
	shared_ptr<Sphere> sph ( new Sphere );
	int Sph_Index = sph->getClassIndexStatic();
	FOREACH ( const shared_ptr<Body>& b, *scene->bodies )
	{
		if ( !b ) continue;
		if ( b->shape->getClassIndex() ==  Sph_Index )
		{
			Sphere* s=YADE_CAST<Sphere*> ( b->shape.get() );
			const Body::id_t& id = b->getId();
			Real rad = s->radius;
			Real x = b->state->pos[0];
			Real y = b->state->pos[1];
			Real z = b->state->pos[2];
			flow->T[flow->currentTes].insert(x, y, z, rad, id);
		}
	}
	flow->viscousShearForces.resize(flow->T[flow->currentTes].max_id+1);
}

void FlowEngine::Initialize_volumes ()
{
	CGT::Finite_vertices_iterator vertices_end = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
	CGT::Vecteur Zero(0,0,0);
	for (CGT::Finite_vertices_iterator V_it = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); V_it!= vertices_end; V_it++) V_it->info().forces=Zero;
	
	CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
	for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
	{
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = Volume_cell ( cell ); break;
			case ( 1 ) : cell->info().volume() = Volume_cell_single_fictious ( cell ); break;
			case ( 2 ) : cell->info().volume() = Volume_cell_double_fictious ( cell ); break;
			case ( 3 ) : cell->info().volume() = Volume_cell_triple_fictious ( cell ); break;
			default: break; 
		}
	}
	if (Debug) cout << "Volumes initialised." << endl;
}

void FlowEngine::Average_real_cell_velocity()
{
    flow->Average_Relative_Cell_Velocity();
    Vector3r Vel (0,0,0);
    //AVERAGE CELL VELOCITY
    CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
    for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
      switch ( cell->info().fictious()) {
	case ( 3 ):
	  for ( int g=0;g<4;g++ )
	  {
		if ( !cell->vertex ( g )->info().isFictious ) {
		  const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
		  for (int i=0;i<3;i++) Vel[i] = Vel[i] + sph->state->vel[i]/4;}
	  }
	  break;
	case ( 2 ):
	  for ( int g=0;g<4;g++ )
	  {
	    if ( !cell->vertex ( g )->info().isFictious ) {
		  const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
		  for (int i=0;i<3;i++) Vel[i] = Vel[i] + sph->state->vel[i]/4;}
	  }
	  break;
	case ( 1 ):
	  for ( int g=0;g<4;g++ )
	  {
	    if ( !cell->vertex ( g )->info().isFictious ) {
		  const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
		  for (int i=0;i<3;i++) Vel[i] = Vel[i] + sph->state->vel[i]/4;}
	  }
	  break;
	case ( 0 ) :
	   for ( int g=0;g<4;g++ )
	  {
	       	  const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
		  for (int i=0;i<3;i++) Vel[i] = Vel[i] + sph->state->vel[i]/4;}
	  }
	  break;
      
    
      CGT::RTriangulation& Tri = flow->T[flow->currentTes].Triangulation();
      CGT::Point pos_av_facet;
      double volume_facet_translation = 0;
      CGT::Vecteur Vel_av (Vel[0], Vel[1], Vel[2]);
      for ( int i=0; i<4; i++ ) {
	      volume_facet_translation = 0;
	      if (!Tri.is_infinite(cell->neighbor(i))){
		    CGT::Vecteur Surfk = cell->info()-cell->neighbor(i)->info();
		    Real area = sqrt ( Surfk.squared_length() );
		    Surfk = Surfk/area;
		    CGT::Vecteur branch = cell->vertex ( facetVertices[i][0] )->point() - cell->info();
		    pos_av_facet = (CGT::Point) cell->info() + ( branch*Surfk ) *Surfk;
		    volume_facet_translation += Vel_av*cell->info().facetSurfaces[i];
		    cell->info().av_vel() = cell->info().av_vel() - volume_facet_translation/cell->info().volume() * ( pos_av_facet-CGAL::ORIGIN );}}
    }
}

void FlowEngine::UpdateVolumes ()
{
	if (Debug) cout << "Updating volumes.............." << endl;
	Real invDeltaT = 1/scene->dt;
	CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
	double newVol, dVol;
	eps_vol_max=0;
	Real totVol=0; Real totDVol=0; Real totVol0=0; Real totVol1=0; Real totVol2=0; Real totVol3=0;
	for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
		switch ( cell->info().fictious()) {
			case ( 3 ): newVol= Volume_cell_triple_fictious ( cell ); totVol3+=newVol; break;
			case ( 2 ) : newVol = Volume_cell_double_fictious ( cell ); totVol2+=newVol; break;
			case ( 1 ) : newVol = Volume_cell_single_fictious ( cell ); totVol1+=newVol; break;
			case ( 0 ) : newVol = Volume_cell ( cell ); totVol0+=newVol; break;
			default: newVol = 0; break;
		}
		totVol+=newVol;
		dVol=cell->info().volumeSign*(newVol - cell->info().volume());
		totDVol+=dVol;
		eps_vol_max = max(eps_vol_max, abs(dVol/newVol));

		cell->info().dv() = (!cell->info().Pcondition)?dVol*invDeltaT:0;
		cell->info().volume() = newVol;
// 		if (Debug) cerr<<"v/dv : "<<cell->info().volume()<<" "<<cell->info().dv()<<" ("<<cell->info().fictious()<<")"<<endl;
	}
	if (Debug) cout << "Updated volumes, total =" <<totVol<<", dVol="<<totDVol<<endl;
}

Real FlowEngine::Volume_cell_single_fictious ( CGT::Cell_handle cell )
{
	Vector3r V[3];
	int b=0;
	int w=0;
	cell->info().volumeSign=1;
	Real Wall_coordinate=0;

	for ( int y=0;y<4;y++ ) {
		if ( ! ( cell->vertex ( y )->info().isFictious ) ){
			const shared_ptr<Body>& sph = Body::byId(cell->vertex ( y )->info().id(), scene);
			V[w]=sph->state->pos;
			w++;}
		else {
			b = cell->vertex ( y )->info().id();
			const shared_ptr<Body>& wll = Body::byId ( b , scene );
			if (!flow->boundary(b).useMaxMin) Wall_coordinate = wll->state->pos[flow->boundary(b).coordinate]+(flow->boundary(b).normal[flow->boundary(b).coordinate])*wall_thickness/2;
			else Wall_coordinate = flow->boundary(b).p[flow->boundary(b).coordinate];}
	}

	double v1[3], v2[3];

	for ( int g=0;g<3;g++ ) { v1[g]=V[0][g]-V[1][g]; v2[g]=V[0][g]-V[2][g];}
	
	Real Volume = 0.5*((V[0]-V[1]).cross(V[0]-V[2]))[flow->boundary(b).coordinate] * ( 0.33333333333* ( V[0][flow->boundary(b).coordinate]+ V[1][flow->boundary(b).coordinate]+ V[2][flow->boundary(b).coordinate] ) - Wall_coordinate );

	return abs ( Volume );
}

Real FlowEngine::Volume_cell_double_fictious ( CGT::Cell_handle cell)
{
	Vector3r A=Vector3r::Zero(), AS=Vector3r::Zero(),B=Vector3r::Zero(), BS=Vector3r::Zero();

	cell->info().volumeSign=1;
	int b[2];
	int coord[2];
	Real Wall_coordinate[2];
	int j=0;
	bool first_sph=true;
	
	for ( int g=0;g<4;g++ )
	{
		if ( cell->vertex(g)->info().isFictious )
		{
			b[j] = cell->vertex (g)->info().id();
			coord[j]=flow->boundary(b[j]).coordinate;
			const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
			if (!flow->boundary(b[j]).useMaxMin) Wall_coordinate[j] = wll->state->pos[coord[j]] +(flow->boundary(b[j]).normal[coord[j]])*wall_thickness/2;
			else Wall_coordinate[j] = flow->boundary(b[j]).p[coord[j]];
			j++;
		}
		else if ( first_sph ){
			const shared_ptr<Body>& sph1 = Body::byId (cell->vertex ( g )->info().id(), scene );
			A=AS=/*AT=*/(sph1->state->pos);
			first_sph=false;}
		else {	const shared_ptr<Body>& sph2 = Body::byId(cell->vertex ( g )->info().id(), scene );
			B=BS=/*BT=*/(sph2->state->pos);}
	}
	AS[coord[0]]=BS[coord[0]] = Wall_coordinate[0];

	//first pyramid with triangular base (A,B,BS)
	Real Vol1=0.5*((A-BS).cross(B-BS))[coord[1]]*(0.333333333*(2*B[coord[1]]+A[coord[1]])-Wall_coordinate[1]);
	//second pyramid with triangular base (A,AS,BS)
	Real Vol2=0.5*((AS-BS).cross(A-BS))[coord[1]]*(0.333333333*(B[coord[1]]+2*A[coord[1]])-Wall_coordinate[1]);
	return abs ( Vol1+Vol2 );
}

Real FlowEngine::Volume_cell_triple_fictious ( CGT::Cell_handle cell)
{
	Vector3r A;

	int b[3];
	int coord[3];
	Real Wall_coordinate[3];
	int j=0;
	cell->info().volumeSign=1;
	
	for ( int g=0;g<4;g++ )
	{
		if ( cell->vertex ( g )->info().isFictious )
		{
		  b[j] = cell->vertex ( g )->info().id();
		  coord[j]=flow->boundary(b[j]).coordinate;
		  const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
		  if (!flow->boundary(b[j]).useMaxMin) Wall_coordinate[j] = wll->state->pos[coord[j]] + (flow->boundary(b[j]).normal[coord[j]])*wall_thickness/2;
		  else Wall_coordinate[j] = flow->boundary(b[j]).p[coord[j]];
		  j++;
		}
		else
		{
		  const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
		  A=(sph->state->pos);  
		}
	}
	Real Volume = (A[coord[0]]-Wall_coordinate[0])*(A[coord[1]]-Wall_coordinate[1])*(A[coord[2]]-Wall_coordinate[2]);
	return abs ( Volume );
}

Real FlowEngine::Volume_cell(CGT::Cell_handle cell)
{
	Real volume = CGT::Tetraedre(makeCgPoint(Body::byId(cell->vertex(0)->info().id(), scene)->state->pos),
				     makeCgPoint(Body::byId(cell->vertex(1)->info().id(), scene)->state->pos),
				     makeCgPoint(Body::byId(cell->vertex(2)->info().id(), scene)->state->pos),
				     makeCgPoint(Body::byId(cell->vertex(3)->info().id(), scene)->state->pos))
				     .volume();

	if (!(cell->info().volumeSign)) cell->info().volumeSign=(volume>0)?1:-1;
	return volume;
}

void FlowEngine::ApplyViscousForces()
{
//   flow->ComputeEdgesSurfaces(); //only done in buildTriangulation
  if (Debug) cout << "Application of viscous forces" << endl;
  if (Debug) cout << "Number of edges = " << flow->Edge_ids.size() << endl;
  for (unsigned int k=0; k<flow->viscousShearForces.size(); k++) flow->viscousShearForces[k]=Vector3r::Zero();

  const CGT::Tesselation& Tes = flow->T[flow->currentTes];
  for (int i=0; i<(int)flow->Edge_ids.size(); i++)
  {
    int hasFictious= Tes.vertex(flow->Edge_ids[i].first)->info().isFictious +  Tes.vertex(flow->Edge_ids[i].second)->info().isFictious;
    const shared_ptr<Body>& sph1 = Body::byId( flow->Edge_ids[i].first, scene );
    const shared_ptr<Body>& sph2 = Body::byId( flow->Edge_ids[i].second, scene );
    Vector3r deltaV;
    if (!hasFictious)
	    deltaV = (sph2->state->vel - sph1->state->vel);
    else {
	    if (hasFictious==1) {//for the fictious sphere, use velocity of the boundary, not of the body
		Vector3r v1 = (Tes.vertex(flow->Edge_ids[i].first)->info().isFictious)? flow->boundary(flow->Edge_ids[i].first).velocity:sph1->state->vel;
		Vector3r v2 = (Tes.vertex(flow->Edge_ids[i].second)->info().isFictious)? flow->boundary(flow->Edge_ids[i].second).velocity:sph2->state->vel;
		deltaV = v2-v1;
	    } else {//both fictious, ignore
		deltaV = Vector3r::Zero();}
    }
    deltaV = deltaV - (flow->Edge_normal[i].dot(deltaV))*flow->Edge_normal[i];
    Vector3r visc_f = flow->ComputeViscousForce(deltaV, i);
    if (Debug) cout << "la force visqueuse entre " << flow->Edge_ids[i].first << " et " << flow->Edge_ids[i].second << "est " << visc_f << endl;
///    //(1) directement sur le body Yade...
//     scene->forces.addForce(flow->Edge_ids[i].first,visc_f);
//     scene->forces.addForce(flow->Edge_ids[i].second,-visc_f);
///   //(2) ou dans CGAL? On a le choix (on pourrait même avoir info->viscousF pour faire la différence entre les deux types de forces... mais ça prend un peu plus de mémoire et de temps de calcul)
//     Tes.vertex(flow->Edge_ids[i].first)->info().forces=Tes.vertex(flow->Edge_ids[i].first)->info().forces+makeCgVect(visc_f);
//     Tes.vertex(flow->Edge_ids[i].second)->info().forces=Tes.vertex(flow->Edge_ids[i].second)->info().forces+makeCgVect(visc_f);
/// //(3) ou dans un vecteur séparé (rapide)
    flow->viscousShearForces[flow->Edge_ids[i].first]+=visc_f;
    flow->viscousShearForces[flow->Edge_ids[i].second]-=visc_f;
  }
}

YADE_PLUGIN ((FlowEngine));
#endif //FLOW_ENGINE

#endif /* YADE_CGAL */
