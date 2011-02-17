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
				cout << "stress controller engine found - FlowEngine" << endl;
				triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> (*itFirst);}}
		if (!triaxialCompressionEngine) cout << "stress controller engine NOT found" << endl;
	}
	currentStress = triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1];
	currentStrain = triaxialCompressionEngine->strain[1];

	timingDeltas->start();
	
	if (first) Build_Triangulation(P_zero);
	timingDeltas->checkpoint("Triangulating");

	if (!first) {
		eps_vol_max=0.f;
		UpdateVolumes ( );
		
		Eps_Vol_Cumulative += eps_vol_max;
		if ((Eps_Vol_Cumulative > EpsVolPercent_RTRG || retriangulationLastIter>1000) && retriangulationLastIter>10) {
			Update_Triangulation = true;
			Eps_Vol_Cumulative=0;
			retriangulationLastIter=0;
			ReTrg++;
		} else  retriangulationLastIter++;
		timingDeltas->checkpoint("Update_Volumes");

		///Compute flow and and forces here
		flow->GaussSeidel();
		timingDeltas->checkpoint("Gauss-Seidel");
		if (save_mgpost) flow->MGPost();
		if (!CachedForces) flow->ComputeFacetForces();
		else flow->ComputeFacetForcesWithCache();
		timingDeltas->checkpoint("Compute_Forces");

		///End Compute flow and forces
		CGT::Finite_vertices_iterator vertices_end = flow->T[flow->currentTes].Triangulation().finite_vertices_end();
		Vector3r f;
		int id;
		for (CGT::Finite_vertices_iterator V_it = flow->T[flow->currentTes].Triangulation().finite_vertices_begin(); V_it !=  vertices_end; V_it++)
		{
			id = V_it->info().id();
			for (int y=0;y<3;y++) f[y] = (V_it->info().forces)[y];
			scene->forces.addForce(id, f);
		}
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
			char *f = "slifile";
			flow->SliceField(f);
		}
		
		if (save_vtk) {flow->save_vtk_file();}
	}
// 	if ( scene->iter % PermuteInterval == 0 )
// 	{ Update_Triangulation = true; }

	if (Update_Triangulation && !first) {
		Build_Triangulation();
		Update_Triangulation = false;
	}

	if (velocity_profile) /*flow->FluidVelocityProfile();*/flow->Average_Fluid_Velocity();
	if (first && liquefaction){
	  wall_up_y = flow->y_max;
	  wall_down_y = flow->y_min;}
	if (liquefaction) flow->Measure_Pore_Pressure(wall_up_y, wall_down_y);

	first=false;
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
	flow->Compute_Permeability ( );
	
	porosity = flow->V_porale_porosity/flow->V_totale_porosity;

	if (first)
	{
		CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
		for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ){cell->info().dv() = 0; cell->info().p() = 0;}
		if (compute_K) {flow->TOLERANCE=1e-06; K = flow->Sample_Permeability ( flow->x_min, flow->x_max, flow->y_min, flow->y_max, flow->z_min, flow->z_max, flow->key );}
		BoundaryConditions();
		flow->Initialize_pressures( P_zero );
  		if (WaveAction) flow->ApplySinusoidalPressure(flow->T[flow->currentTes].Triangulation(), Sinus_Amplitude, Sinus_Average, 30);
		flow->TOLERANCE=Tolerance;
		flow->RELAX=Relax;
	}
	else
	{
		if (Debug && compute_K) cout << "---------UPDATE PERMEABILITY VALUE--------------" << endl;
		CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
		for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ){cell->info().dv() = 0; cell->info().p() = 0;}
		if (compute_K) {flow->TOLERANCE=1e-06; K = flow->Sample_Permeability ( flow->x_min, flow->x_max, flow->y_min, flow->y_max, flow->z_min, flow->z_max, flow->key );}
		BoundaryConditions();
		flow->Initialize_pressures(P_zero);// FIXME : why, if we are going to interpolate after that?
		flow->TOLERANCE=Tolerance;//So it can be changed at run time
		flow->Interpolate (flow->T[!flow->currentTes], flow->T[flow->currentTes]);
 		if (WaveAction) flow->ApplySinusoidalPressure(flow->T[flow->currentTes].Triangulation(), Sinus_Amplitude, Sinus_Average, 30);
		flow->TOLERANCE=Tolerance;
		flow->RELAX=Relax;
	}
	Initialize_volumes();
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
	
	flow->y_min_id=triaxialCompressionEngine->wall_bottom_id;
	flow->y_max_id=triaxialCompressionEngine->wall_top_id;
	flow->x_max_id=triaxialCompressionEngine->wall_right_id;
	flow->x_min_id=triaxialCompressionEngine->wall_left_id;
	flow->z_min_id=triaxialCompressionEngine->wall_back_id;
	flow->z_max_id=triaxialCompressionEngine->wall_front_id;
	
	flow->boundary ( flow->y_min_id ).useMaxMin = BOTTOM_Boundary_MaxMin;
	flow->boundary ( flow->y_max_id ).useMaxMin = TOP_Boundary_MaxMin;
	flow->boundary ( flow->x_max_id ).useMaxMin = RIGHT_Boundary_MaxMin;
	flow->boundary ( flow->x_min_id ).useMaxMin = LEFT_Boundary_MaxMin;
	flow->boundary ( flow->z_max_id ).useMaxMin = FRONT_Boundary_MaxMin;
	flow->boundary ( flow->z_min_id ).useMaxMin = BACK_Boundary_MaxMin;	
	
	//FIXME: Id's order in boundsIds is done according to the enumerotation of boundaries from TXStressController.hpp, line 31. DON'T CHANGE IT!
	flow->boundsIds[0]= &flow->y_min_id;
        flow->boundsIds[1]= &flow->y_max_id;
        flow->boundsIds[2]= &flow->x_min_id;
        flow->boundsIds[3]= &flow->x_max_id;
        flow->boundsIds[4]= &flow->z_max_id;
        flow->boundsIds[5]= &flow->z_min_id;
	
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
}

void FlowEngine::Initialize_volumes ()
{
	CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
	for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
	{
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = Volume_cell ( cell ); break;
			case ( 1 ) : cell->info().volume() = Volume_cell_single_fictious ( cell ); break;
			case ( 2 ) : cell->info().volume() = Volume_cell_double_fictious ( cell ); break;
			case ( 3 ) : cell->info().volume() = Volume_cell_triple_fictious ( cell ); break;
		}
	}
	if (Debug) cout << "Volumes initialised." << endl;
}

void FlowEngine::UpdateVolumes ()
{
	if (Debug) cout << "Updating volumes.............." << endl;
	Real invDeltaT = 1/scene->dt;
	CGT::Finite_cells_iterator cell_end = flow->T[flow->currentTes].Triangulation().finite_cells_end();
	double newVol=0; double dVol;
	eps_vol_max=0;
	for ( CGT::Finite_cells_iterator cell = flow->T[flow->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ) {
		switch ( cell->info().fictious()) {
			case ( 3 ):
				newVol= Volume_cell_triple_fictious ( cell );
				break;
			case ( 2 ) :
				newVol = Volume_cell_double_fictious ( cell );
				break;
			case ( 1 ) :
				newVol = Volume_cell_single_fictious ( cell );
				break;
			case ( 0 ) :
				newVol = Volume_cell ( cell );
				break;
		}
		dVol=newVol - cell->info().volume();
		eps_vol_max = max(eps_vol_max, abs(dVol/newVol));
		cell->info().dv() = dVol*invDeltaT;
		cell->info().volume() = newVol;
// 		if (Debug) cerr<<"v/dv : "<<cell->info().volume()<<" "<<cell->info().dv()<<" ("<<cell->info().fictious()<<")"<<endl;
	}
}

Real FlowEngine::Volume_cell_single_fictious ( CGT::Cell_handle cell )
{
	Real V[3][3];
	int b=0;
	int w=0;

	Real Wall_coordinate=0;
	
	Vector3r Vel[3];
	int id = 0;
	double Vel_x=0, Vel_y=0, Vel_z=0;

	for ( int y=0;y<4;y++ )
	{
		if ( ! ( cell->vertex ( y )->info().isFictious ) )
		{
			const shared_ptr<Body>& sph = Body::byId
			                              ( cell->vertex ( y )->info().id(), scene );
			for ( int g=0;g<3;g++ ) V[w][g]=sph->state->pos[g];
			Vel[w] = sph->state->pos;
			w++;
		}
		else
		{
			b = cell->vertex ( y )->info().id()-flow->id_offset;
			const shared_ptr<Body>& wll = Body::byId ( b , scene );
			if (!flow->boundaries[b].useMaxMin) Wall_coordinate = wll->state->pos[flow->boundaries[b].coordinate]+(flow->boundaries[b].normal[flow->boundaries[b].coordinate])*wall_thickness/2;
			else Wall_coordinate = flow->boundaries[b].p[flow->boundaries[b].coordinate];
			id = y;
		}
	}
	
	for ( int y=0;y<4;y++ ){
	  for ( int j=0;j<3;j++ ){
	    if (cell->vertex(j)->info().isFictious){
	      Vel_x += 0.33333333333*(Vel[facetVertices[y][j]][0]);
	      Vel_y += 0.33333333333*(Vel[facetVertices[y][j]][1]);
	      Vel_z += 0.33333333333*(Vel[facetVertices[y][j]][2]);}
	    else for (int j2=0;j2<3;j2++){
	      if (!cell->vertex(j2)->info().isFictious){
		Vel_x += 0.5*(Vel[facetVertices[y][j]][0]);
		Vel_y += 0.5*(Vel[facetVertices[y][j]][1]);
		Vel_z += 0.5*(Vel[facetVertices[y][j]][2]);}}}
	      CGT::Vecteur Vel_facet ( Vel_x, Vel_y, Vel_z );
	      (cell->info().facetVelocity())[y] = Vel_facet*1;}

	double v1[3], v2[3];

	for ( int g=0;g<3;g++ ) { v1[g]=V[0][g]-V[1][g]; v2[g]=V[0][g]-V[2][g];}

	Real Volume = ( CGAL::cross_product ( CGT::Vecteur ( v1[0],v1[1],v1[2] ),
	                                      CGT::Vecteur ( v2[0],v2[1],v2[2] ) ) *
	                flow->boundaries[b].normal ) * ( 0.33333333333* ( V[0][flow->boundaries[b].coordinate]+ V[1][flow->boundaries[b].coordinate]+ V[2][flow->boundaries[b].coordinate] ) - Wall_coordinate );

	return abs ( Volume );
}

Real FlowEngine::Volume_cell_double_fictious ( CGT::Cell_handle cell)
{
  	Real A[3]={0, 0, 0}, AS[3]={0, 0, 0}, AT[3]={0, 0, 0};
	Real B[3]={0, 0, 0}, BS[3]={0, 0, 0}, BT[3]={0, 0, 0};
	Real C[3]={0, 0, 0}, CS[3]={0, 0, 0}, CT[3]={0, 0, 0};
	
	int b[2];
	Real Wall_coordinate[2];
	int j=0;
	bool first_sph=true;
	
	Vector3r Vel[2];
	vector<int> id;
	id.resize(2);
	
	for ( int g=0;g<4;g++ )
	{
		if ( cell->vertex ( g )->info().isFictious )
		{
			b[j] = cell->vertex ( g )->info().id()-flow->id_offset;
			const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
			if (!flow->boundaries[b[j]].useMaxMin) Wall_coordinate[j] = wll->state->pos[flow->boundaries[b[j]].coordinate] +(flow->boundaries[b[j]].normal[flow->boundaries[b[j]].coordinate])*wall_thickness/2;
			else Wall_coordinate[j] = flow->boundaries[b[j]].p[flow->boundaries[b[j]].coordinate];
			j++;
		}
		else if ( first_sph )
		{
			const shared_ptr<Body>& sph1 = Body::byId
			                               ( cell->vertex ( g )->info().id(), scene );
			for ( int k=0;k<3;k++ ) { A[k]=AS[k]=AT[k]=sph1->state->pos[k]; first_sph=false;}
			Vel[0] = sph1->state->vel; id[0]=g;
		}
		else
		{
			const shared_ptr<Body>& sph2 = Body::byId
			                               ( cell->vertex ( g )->info().id(), scene );
			for ( int k=0;k<3;k++ ) { B[k]=BS[k]=BT[k]=sph2->state->pos[k]; }
			Vel[1] = sph2->state->vel; id[1]=g;
		}
	}
	
	for (int y=0;y<4;y++){
	  if ( y == id[0] ) {CGT::Vecteur Vel_facet (Vel[0][0], Vel[0][1], Vel[0][2]); (cell->info().facetVelocity())[y] = Vel_facet*1;}
	  else if ( y == id[1] ) {CGT::Vecteur Vel_facet (Vel[1][0], Vel[1][1], Vel[1][2]); (cell->info().facetVelocity())[y] = Vel_facet*1;}
	  else { CGT::Vecteur Vel_facet (0.5*(Vel[0][0]+Vel[0][1]+Vel[0][2]),0.5*(Vel[1][0]+Vel[1][1]+Vel[1][2]),0.5*(Vel[2][0]+Vel[2][1]+Vel[2][2]));
	    (cell->info().facetVelocity())[y] =  Vel_facet*1;}}
	
	
	AS[flow->boundaries[b[0]].coordinate]=BS[flow->boundaries[b[0]].coordinate] = Wall_coordinate[0];
	AT[flow->boundaries[b[1]].coordinate]=BT[flow->boundaries[b[1]].coordinate] = Wall_coordinate[1];

	for ( int h=0;h<3;h++ ) {C[h]= ( A[h]+B[h] ) /2; CS[h]= ( AS[h]+BS[h] ) /2; CT[h]= ( AT[h]+BT[h] ) /2;}

	CGT::Vecteur v1 ( AT[0]-BT[0],AT[1]-BT[1],AT[2]-BT[2] );
	CGT::Vecteur v2 ( C[0]-CT[0],C[1]-CT[1],C[2]-CT[2] );

	Real h = C[flow->boundaries[b[0]].coordinate]- CS[flow->boundaries[b[0]].coordinate];

	Real Volume = ( CGAL::cross_product ( v1,v2 ) *flow->boundaries[b[0]].normal ) *h;

	return abs ( Volume );
}

Real FlowEngine::Volume_cell_triple_fictious ( CGT::Cell_handle cell)
{
	Real A[3]={0, 0, 0}, AS[3]={0, 0, 0}, AT[3]={0, 0, 0}, AW[3]={0, 0, 0};

	int b[3];
	Real Wall_coordinate[3];
	int j=0;
	
	Vector3r Vel;
	
	for ( int g=0;g<4;g++ )
	{
		if ( cell->vertex ( g )->info().isFictious )
		{
		  b[j] = cell->vertex ( g )->info().id()-flow->id_offset;
		  const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
		  if (!flow->boundaries[b[j]].useMaxMin) Wall_coordinate[j] = wll->state->pos[flow->boundaries[b[j]].coordinate] + (flow->boundaries[b[j]].normal[flow->boundaries[b[j]].coordinate])*wall_thickness/2;
		  else Wall_coordinate[j] = flow->boundaries[b[j]].p[flow->boundaries[b[j]].coordinate];
		  j++;
		}
		else
		{
		  const shared_ptr<Body>& sph = Body::byId ( cell->vertex ( g )->info().id(), scene );
		  for ( int k=0;k<3;k++ ) { A[k]=AS[k]=AT[k]=AW[k]=sph->state->pos[k];}
		  Vel = sph->state->vel;
		}
	}
	
	CGT::Vecteur Vel_facet (Vel[0], Vel[1], Vel[2]);
	for (int y=0;y<4;y++) (cell->info().facetVelocity())[y] = Vel_facet*1;
	
	AS[flow->boundaries[b[0]].coordinate]= AT[flow->boundaries[b[0]].coordinate]= AW[flow->boundaries[b[0]].coordinate]= Wall_coordinate[0];
	AT[flow->boundaries[b[1]].coordinate]= Wall_coordinate[1];
	AW[flow->boundaries[b[2]].coordinate]= Wall_coordinate[2];
	
	CGT::Vecteur v1 ( AS[0]-AT[0],AS[1]-AT[1],AS[2]-AT[2] );
	CGT::Vecteur v2 ( AS[0]-AW[0],AS[1]-AW[1],AS[2]-AW[2] );

	CGT::Vecteur h ( AT[0] - A[0], AT[1] - A[1], AT[2] - A[2] );

	Real Volume = ( CGAL::cross_product ( v1,v2 ) ) * h;
	
	return abs ( Volume );
}

Real FlowEngine::Volume_cell ( CGT::Cell_handle cell)
{
	Vector3r A[4];
	Vector3r Vel[4];
	
	double Vel_x=0, Vel_y=0, Vel_z=0;

	for ( int y=0;y<4;y++ )
	{
		const shared_ptr<Body>& sph = Body::byId(cell->vertex ( y )->info().id(), scene);
		A[y]=sph->state->pos;
		Vel[y]=sph->state->vel;
	}
	
	for ( int y=0;y<4;y++ ){
	  for ( int j=0;j<3;j++ ){
	    Vel_x += 0.33333333333*(Vel[facetVertices[y][j]][0]);
	    Vel_y += 0.33333333333*(Vel[facetVertices[y][j]][1]);
	    Vel_z += 0.33333333333*(Vel[facetVertices[y][j]][2]);}
	    CGT::Vecteur Vel_facet ( Vel_x, Vel_y, Vel_z );
	    (cell->info().facetVelocity())[y] = Vel_facet*1; }

	CGT::Point p1 ( ( A[0] ) [0], ( A[0] ) [1], ( A[0] ) [2] );
	CGT::Point p2 ( ( A[1] ) [0], ( A[1] ) [1], ( A[1] ) [2] );
	CGT::Point p3 ( ( A[2] ) [0], ( A[2] ) [1], ( A[2] ) [2] );
	CGT::Point p4 ( ( A[3] ) [0], ( A[3] ) [1], ( A[3] ) [2] );

	Real Volume = ( std::abs ( ( CGT::Tetraedre ( p1,p2,p3,p4 ).volume() ) ) );

	return abs ( Volume );
}

YADE_PLUGIN ((FlowEngine));
#endif //FLOW_ENGINE

#endif /* YADE_CGAL */

// YADE_REQUIRE_FEATURE(PHYSPAR);

// 		if ( !cell->info().isFictious )
// 		{
// // 			for ( int i=0; i<4; i++ )
// // 			{
// // 				for ( int j=0; j<3;j++ ) id [j] = cell->vertex ( facetVertices[i][j] )->info().id();
// // 				for ( int m=0; m<3;m++ )
// // 				{
// // 					const shared_ptr<Body>& b = Body::byId ( id[m], scene );
// //
// // 					dx[m] = b->state->vel[0];
// // 					dy[m] = b->state->vel[1];
// // 					dz[m] = b->state->vel[2];
// //
// // 					( v[m] ) [0] = b->state->pos[0];
// // 					( v[m] ) [1] = b->state->pos[1];
// // 					( v[m] ) [2] = b->state->pos[2];
// //
// // 				}
// //
// // 				CGT::Vecteur v1 ( ( v[1] ) [0]- ( v[0] ) [0], ( v[1] ) [1]- ( v[0] ) [1], ( v[1] ) [2]- ( v[0] ) [2] );
// // 				CGT::Vecteur v2 ( ( v[2] ) [0]- ( v[1] ) [0], ( v[2] ) [1]- ( v[1] ) [1], ( v[2] ) [2]- ( v[1] ) [2] );
// //
// //
// // 				CGT::Vecteur V = 0.33333333333*CGT::Vecteur ( dx[0]+dx[1]+dx[2], dy[0]+dy[1]+dy[2], dz[0]+dz[1]+dz[2] );
// // 				CGT::Vecteur S = CGAL::cross_product ( v1,v2 ) /2.f;
// //
// // 				CGT::Somme ( grad_u, V, S );
// // 			}
// // 			cell->info().dv() = grad_u.Trace();
// 		}
// 		else
// 		{

// 			if ( triple_fictious )
// 			{
// 				double deltaT = 1;
// 				cell->info().dv() = (Volume_cell_double_fictious(cell,scene)-cell->info().volume())/deltaT;
// 				cell->info().volume() = Volume_cell_double_fictious(cell,scene);
/*
int id_real_local=0,id_real_global=0,V_fict=0;
double pos[3], surface=0;
for ( int g=0;g<4;g++ )
{
	if ( !cell->vertex ( g )->info().isFictious )
	{
		id_real_local=g;
		id_real_global=cell->vertex ( g )->info().id();
	}
}
const shared_ptr<Body>& sph = Body::byId ( id_real_global, scene );
for ( int i=0;i<3;i++ ) pos[i]=sph->state->pos[i];
for ( int j=0;j<4;j++ )
{
	if ( cell->vertex ( j )->info().isFictious )
	{
		CGT::Boundary b = flow->boundaries[cell->vertex ( j )->info().id() ];
		const shared_ptr<Body>& wall = Body::byId
				( cell->vertex ( j )->info().id(), scene );

		surface = flow->surface_external_triple_fictious ( pos, cell, b );

		Real Vs = sph->state->vel[b.coordinate];
		Real Vw = wall->state->vel[b.coordinate];
		Real Vrel = Vs - Vw;

		cell->info().dv() += Vrel*surface;
	}
}*/
// 			}
// 			if ( double_fictious )
// 			{
// 				double deltaT = 1;
// 				cell->info().dv() = (Volume_cell_double_fictious(cell,scene)-cell->info().volume())/deltaT;
// 				cell->info().volume() = Volume_cell_double_fictious(cell,scene);
// 				double A[3], AS[3], AT[3];
// 				double B[3], BS[3], BT[3];
// 				bool first=true, first_boundary=true;
// 				Vector3r Vel_A, Vel_B, Vel_W1, Vel_W2;
//
// 				CGT::Boundary b1, b2;
//
// 				for ( int g=0;g<4;g++ )
// 				{
// 					if ( !cell->vertex ( g )->info().isFictious && first )
// 					{
// 						const shared_ptr<Body>& sph1 = Body::byId
// 						                               ( cell->vertex ( g )->info().id(), scene );
// 						for ( int y=0;y<3;y++ )
// 							{A[y] = sph1->state->pos[y]; AS[y]=A[y]; AT[y]=A[y];}
// 						Vel_A = sph1->state->vel;
//
// 						first = false;
// 					}
// 					else if ( !cell->vertex ( g )->info().isFictious )
// 					{
// 						const shared_ptr<Body>& sph2 = Body::byId
// 						                               ( cell->vertex ( g )->info().id(), scene );
// 						for ( int y=0;y<3;y++ )
// 							{B[y] = ( cell->vertex ( g )->point() ) [y]; BS[y]=B[y]; BT[y]=B[y];}
//
// 						Vel_B = sph2->state->vel;
// 					}
// 					else if ( first_boundary )
// 					{
// 						b1 = flow->boundaries[cell->vertex ( g )->info().id() ];
// 						const shared_ptr<Body>& wll1 = Body::byId
// 						                               ( cell->vertex ( g )->info().id() , scene );
//
// 						Vel_W1=wll1->state->vel;
// 						first_boundary=false;
// 					}
// 					else
// 					{
// 						b2 = flow->boundaries[cell->vertex ( g )->info().id() ];
// 						const shared_ptr<Body>& wll2 = Body::byId
// 						                               ( cell->vertex ( g )->info().id() , scene );
//
// 						Vel_W2=wll2->state->vel;
// 					}
// 				}
//
// 				AS[b1.coordinate]=BS[b1.coordinate]=b1.p[b1.coordinate];
// 				AT[b2.coordinate]=BT[b2.coordinate]=b2.p[b2.coordinate];
//
// 				double Vmoy[3];
//
// 				for ( int y=0;y<3;y++ ) Vmoy[y]= ( Vel_A[y]+2*Vel_W1[y] ) /3;
// 				CGT::Vecteur Surface = ( CGAL::cross_product ( CGT::Vecteur ( A[0]-AS[0],A[1]-AS[1],A[2]-AS[2] ),
// 				                         CGT::Vecteur ( AS[0]-BS[0],AS[1]-BS[1],AS[2]-BS[2] ) ) ) /2;
// 				cell->info().dv() += CGT::Vecteur ( Vmoy[0],Vmoy[1],Vmoy[2] ) *Surface;
//
// 				for ( int y=0;y<3;y++ ) Vmoy[y]= ( Vel_A[y]+Vel_B[y]+Vel_W1[y] ) /3;
// 				Surface = ( CGAL::cross_product ( CGT::Vecteur ( A[0]-B[0],A[1]-B[1],A[2]-B[2] ),
// 				                                 CGT::Vecteur ( B[0]-BS[0],B[1]-BS[1],B[2]-BS[2] ) ) ) /2;
// 				cell->info().dv() += CGT::Vecteur ( Vmoy[0],Vmoy[1],Vmoy[2] ) *Surface;
//
// 				for ( int y=0;y<3;y++ ) Vmoy[y]= ( Vel_A[y]+Vel_W1[y]+Vel_W2[y] ) /3;
// 				Surface = ( CGAL::cross_product ( CGT::Vecteur ( A[0]-AS[0],A[1]-AS[1],A[2]-AS[2] ),
// 				                                 CGT::Vecteur ( A[0]-AT[0],A[1]-AT[1],A[2]-AT[2] ) ) ) /2;
// 				cell->info().dv() += CGT::Vecteur ( Vmoy[0],Vmoy[1],Vmoy[2] ) *Surface;
//
// 				for ( int y=0;y<3;y++ ) Vmoy[y]= ( Vel_B[y]+Vel_W1[y]+Vel_W2[y] ) /3;
// 				Surface = ( CGAL::cross_product ( CGT::Vecteur ( B[0]-BS[0],B[1]-BS[1],B[2]-BS[2] ),
// 				                                 CGT:: Vecteur ( B[0]-BT[0],B[1]-BT[1],B[2]-BT[2] ) ) ) /2;
// 				cell->info().dv() += CGT::Vecteur ( Vmoy[0],Vmoy[1],Vmoy[2] ) *Surface;
// 			}
// 			if ( single_fictious )
// 			{
// 				double deltaT = 1;
// 				cell->info().dv() = (Volume_cell_single_fictious(cell,scene)-cell->info().volume())/deltaT;
// 				cell->info().volume() = Volume_cell_single_fictious(cell,scene);
// 			}
// 		}

