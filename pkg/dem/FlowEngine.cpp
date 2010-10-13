/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano                               *
*  emanuele.catalano@hmg.inpg.fr                                         *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "FlowEngine.hpp"
#include<yade/core/Scene.hpp>
#include<yade/lib-base/Math.hpp>

#include<yade/pkg-common/Sphere.hpp>
#include<yade/pkg-common/Wall.hpp>
#include<yade/pkg-common/Box.hpp>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef FLOW_ENGINE

YADE_REQUIRE_FEATURE (CGAL);
CREATE_LOGGER (FlowEngine);

FlowEngine::~FlowEngine()
{
}

void FlowEngine::action ( )
{
	if (!flow) {
	  flow = shared_ptr<CGT::FlowBoundingSphere> (new CGT::FlowBoundingSphere);
	  network = shared_ptr<CGT::Network> (new CGT::Network);
	  first=true;Update_Triangulation=false;}
	if ( !isActivated ) return;
	else
	{
		timingDeltas->start();
		
		if ( !triaxialCompressionEngine )
		{vector<shared_ptr<Engine> >::iterator itFirst = scene->engines.begin();vector<shared_ptr<Engine> >::iterator itLast = scene->engines.end();for ( ;itFirst!=itLast; ++itFirst ){
		if ( ( *itFirst )->getClassName() == "TriaxialCompressionEngine" ){
		cout << "stress controller engine found - FlowEngine" << endl;
		triaxialCompressionEngine =  YADE_PTR_CAST<TriaxialCompressionEngine> ( *itFirst );}}
		if ( !triaxialCompressionEngine ) cout << "stress controller engine NOT found" << endl;}

		currentStress = triaxialCompressionEngine->stress[triaxialCompressionEngine->wall_top][1];
		currentStrain = triaxialCompressionEngine->strain[1];
		current_state = triaxialCompressionEngine->currentState;

		if ( current_state==3 )
		{
			if ( first ) { Build_Triangulation( P_zero );}
      
				timingDeltas->checkpoint("Triangulating");
				
				UpdateVolumes ( );
			
				timingDeltas->checkpoint("Update_Volumes");
			
			///Compute flow and and forces here
			
				if (!first) flow->GaussSeidel ( );
				timingDeltas->checkpoint("Gauss-Seidel");
				
				if (save_mplot){int j = scene->iter;
				char plotfile [50];
				mkdir("./mplot", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				string visu_consol = "./mplot/"+flow->key+"%d_Visu_Consol";
				const char* key_visu_consol = visu_consol.c_str();
				sprintf (plotfile, key_visu_consol, j);	char *gg = plotfile;
				flow->mplot(gg);}
			
 				flow->MGPost();

				flow->ComputeFacetForces();
				
				timingDeltas->checkpoint("Compute_Forces");

			///End Compute flow and forces

				CGT::Finite_vertices_iterator vertices_end = network->T[network->currentTes].Triangulation().finite_vertices_end ();
				Vector3r f; int id;
				for ( CGT::Finite_vertices_iterator V_it = network->T[network->currentTes].Triangulation().finite_vertices_begin (); V_it !=  vertices_end; V_it++ )
				{
					id = V_it->info().id();
					for ( int y=0;y<3;y++ ) f[y] = ( V_it->info().forces ) [y];
					scene->forces.addForce ( id, f );
				//scene->forces.addTorque(id,t);
				}
				
				timingDeltas->checkpoint("Applying Forces");
			
				Real time = scene->time;
			
				int j = scene->iter;
				char file [50];
				mkdir("./Consol", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
				string consol = "./Consol/"+flow->key+"%d_Consol";
				const char* keyconsol = consol.c_str();
				sprintf (file, keyconsol, j);
				char *g = file;
				timingDeltas->checkpoint("Writing cons_files");
				
				MaxPressure = flow->PressureProfile( g, time, intervals);
				
				std::ofstream max_p ("pressures.txt", std::ios::app);
				max_p << j << " " << time << " " << MaxPressure << endl;
				
				std::ofstream settle ("settle.txt", std::ios::app);
				settle << j << " " << time << " " << currentStrain << endl;
				
				if ( scene->iter % PermuteInterval == 0 )
				{ Update_Triangulation = true; }
				
				if ( Update_Triangulation && !first) { Build_Triangulation( );}
				
				first=false;
				Update_Triangulation = false;
				
				timingDeltas->checkpoint("Storing Max Pressure");
				
				flow->Average_Grain_Velocity();
				if (save_vtk) flow->save_vtk_file();
				
// 				int numero = flow->Average_Cell_Velocity(id_sphere, network->T[network->currentTes].Triangulation());
				
// 				flow->vtk_average_cell_velocity(network->T[network->currentTes].Triangulation(), id_sphere, numero);
		}
	}
}

void FlowEngine::BoundaryConditions()
{
	network->boundary ( network->y_min_id ).flowCondition=Flow_imposed_BOTTOM_Boundary;
	network->boundary ( network->y_max_id ).flowCondition=Flow_imposed_TOP_Boundary;
	network->boundary ( network->x_max_id ).flowCondition=Flow_imposed_RIGHT_Boundary;
	network->boundary ( network->x_min_id ).flowCondition=Flow_imposed_LEFT_Boundary;
	network->boundary ( network->z_max_id ).flowCondition=Flow_imposed_FRONT_Boundary;
	network->boundary ( network->z_min_id ).flowCondition=Flow_imposed_BACK_Boundary;
	
	network->boundary ( network->y_min_id ).value=Pressure_BOTTOM_Boundary;
	network->boundary ( network->y_max_id ).value=Pressure_TOP_Boundary;
	network->boundary ( network->x_max_id ).value=Pressure_RIGHT_Boundary;
	network->boundary ( network->x_min_id ).value=Pressure_LEFT_Boundary;
	network->boundary ( network->z_max_id ).value=Pressure_FRONT_Boundary;
	network->boundary ( network->z_min_id ).value=Pressure_BACK_Boundary;
}


void FlowEngine::Build_Triangulation ()
{
	Build_Triangulation (0.f);
}

void FlowEngine::Build_Triangulation (double P_zero)
{
	if (first)
	{
		network->currentTes=0;
		network->Vtotalissimo=0; network->Vsolid_tot=0; network->Vporale=0; network->Ssolid_tot=0;
		flow->SLIP_ON_LATERALS=slip_boundary;
		flow->key = triaxialCompressionEngine->Key;
		flow->k_factor = permeability_factor;
	}
	else
	{
		network->currentTes=!network->currentTes;
		cout << "--------RETRIANGULATION-----------" << endl;
	}
	network->T[network->currentTes].Clear();
	network->T[network->currentTes].max_id=-1;
	network->x_min = 1000.0, network->x_max = -10000.0, network->y_min = 1000.0, network->y_max = -10000.0, network->z_min = 1000.0, network->z_max = -10000.0;

	AddBoundary ( );
	Triangulate ( );
	
	cout << endl << "Tesselating------" << endl << endl;
	network->T[network->currentTes].Compute();
	
	network->Define_fictious_cells();
	network->DisplayStatistics ();	

	flow->meanK_LIMIT = meanK_correction;
	flow->meanK_STAT = meanK_opt;
	flow->Compute_Permeability ();

	if (first)
	{
		CGT::Finite_cells_iterator cell_end = network->T[network->currentTes].Triangulation().finite_cells_end();
		for ( CGT::Finite_cells_iterator cell = network->T[network->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ){cell->info().dv() = 0; cell->info().p() = 0;}
		if (compute_K) {flow->TOLERANCE=1e-06; K = flow->Sample_Permeability ( network->x_min, network->x_max, network->y_min, network->y_max, network->z_min, network->z_max, flow->key );}
		BoundaryConditions();
		flow->Initialize_pressures( P_zero );
  		if (WaveAction) flow->ApplySinusoidalPressure(network->T[network->currentTes].Triangulation(), Sinus_Pressure, 5);
		flow->TOLERANCE=Tolerance;
		flow->RELAX=Relax;
	}
	else
	{
		cout << "---------UPDATE PERMEABILITY VALUE--------------" << endl;
		CGT::Finite_cells_iterator cell_end = network->T[network->currentTes].Triangulation().finite_cells_end();
		for ( CGT::Finite_cells_iterator cell = network->T[network->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ ){cell->info().dv() = 0; cell->info().p() = 0;}
		if (compute_K) {flow->TOLERANCE=1e-06; K = flow->Sample_Permeability ( network->x_min, network->x_max, network->y_min, network->y_max, network->z_min, network->z_max, flow->key );}
		BoundaryConditions();
		flow->Initialize_pressures( P_zero );
		flow->Interpolate ( network->T[!network->currentTes], network->T[network->currentTes] );

 		if (WaveAction) flow->ApplySinusoidalPressure(network->T[network->currentTes].Triangulation(), Sinus_Pressure, 15);
	}
	Initialize_volumes ( );
}

void FlowEngine::AddBoundary ()
{
  
	shared_ptr<Sphere> sph ( new Sphere );

	int Sph_Index = sph->getClassIndexStatic();
	int contator = 0;
	
	FOREACH ( const shared_ptr<Body>& b, *scene->bodies )
	{
		if ( !b ) continue;
		if ( b->shape->getClassIndex() ==  Sph_Index )
		{
		  Sphere* s=YADE_CAST<Sphere*> ( b->shape.get() );
			//const Body::id_t& id = b->getId();
			Real rad = s->radius;
			Real x = b->state->pos[0];
			Real y = b->state->pos[1];
			Real z = b->state->pos[2];
			
			network->x_min = min ( network->x_min, x-rad);
			network->x_max = max ( network->x_max, x+rad);
			network->y_min = min ( network->y_min, y-rad);
			network->y_max = max ( network->y_max, y+rad);
			network->z_min = min ( network->z_min, z-rad);
			network->z_max = max ( network->z_max, z+rad);
			
			contator+=1;
		}
	}
	
	cout << "Adding Boundary------" << endl;

	shared_ptr<Box> bx ( new Box );
	int Bx_Index = bx->getClassIndexStatic();

	FOREACH ( const shared_ptr<Body>& b, *scene->bodies )
	{
		if ( !b ) continue;
		if ( b->shape->getClassIndex() == Bx_Index )
		{
			Box* w = YADE_CAST<Box*> ( b->shape.get() );
// 			const Body::id_t& id = b->getId();
			Real center [3], Extent[3];
			for ( int h=0;h<3;h++ ) {center[h] = b->state->pos[h]; Extent[h] = w->extents[h];}
			wall_thickness = min(min(Extent[0],Extent[1]),Extent[2]);
		}
	}
	
	id_offset = network->T[network->currentTes].max_id+1;
	
	network->id_offset = id_offset;

	network->y_min_id=triaxialCompressionEngine->wall_bottom_id;
	network->y_max_id=triaxialCompressionEngine->wall_top_id;
	network->x_max_id=triaxialCompressionEngine->wall_right_id;
	network->x_min_id=triaxialCompressionEngine->wall_left_id;
	network->z_min_id=triaxialCompressionEngine->wall_back_id;
	network->z_max_id=triaxialCompressionEngine->wall_front_id;

	network->AddBoundingPlanes(true);
}

void FlowEngine::Triangulate ()
{
	shared_ptr<Sphere> sph ( new Sphere );

	int Sph_Index = sph->getClassIndexStatic();
	int contator = 0;
	
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
			
			network->T[network->currentTes].insert(x, y, z, rad, id);
			
			contator+=1;
		}
	}
	double SectionArea = ( network->x_max - network->x_min ) * ( network->z_max-network->z_min );

	cout << "Section area = " << SectionArea << endl;
// 	cout << "Rmoy " << Rmoy << endl;
	cout << "x_min = " << network->x_min << endl;
	cout << "x_max = " << network->x_max << endl;
	cout << "y_max = " << network->y_max << endl;
	cout << "y_min = " << network->y_min << endl;
	cout << "z_min = " << network->z_min << endl;
	cout << "z_max = " << network->z_max << endl;
}

void FlowEngine::Initialize_volumes ()
{
	CGT::Finite_cells_iterator cell_end = network->T[network->currentTes].Triangulation().finite_cells_end();
	for ( CGT::Finite_cells_iterator cell = network->T[network->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
	{
		switch ( cell->info().fictious() )
		{
			case ( 0 ) : cell->info().volume() = Volume_cell ( cell ); break;
			case ( 1 ) : cell->info().volume() = Volume_cell_single_fictious ( cell ); break;
			case ( 2 ) : cell->info().volume() = Volume_cell_double_fictious ( cell ); break;
			case ( 3 ) : cell->info().volume() = Volume_cell_triple_fictious ( cell ); break;
		}
	}
	cout << "Volumes initialised." << endl;
}

void FlowEngine::UpdateVolumes ()
{
	cout << "Updating volumes.............." << endl;

	Real deltaT = scene->dt;
	CGT::Finite_cells_iterator cell_end = network->T[network->currentTes].Triangulation().finite_cells_end();
	for ( CGT::Finite_cells_iterator cell = network->T[network->currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
	{
		switch ( cell->info().fictious() )
		{
			case ( 3 ):
			{
				cell->info().dv() = ( Volume_cell_triple_fictious ( cell ) - cell->info().volume() ) /deltaT;
				cell->info().volume() = Volume_cell_triple_fictious ( cell );
			}break;
			case ( 2 ) :
			{
				cell->info().dv() = ( Volume_cell_double_fictious ( cell )-cell->info().volume() ) /deltaT;
				cell->info().volume() = Volume_cell_double_fictious ( cell );
			}break;
			case ( 1 ) :
			{
				cell->info().dv() = ( Volume_cell_single_fictious ( cell )-cell->info().volume() ) /deltaT;
				cell->info().volume() = Volume_cell_single_fictious ( cell );
			}break;
			case ( 0 ) :
			{
				cell->info().dv() = ( Volume_cell ( cell )-cell->info().volume() ) /deltaT;
				cell->info().volume() = Volume_cell ( cell );
			}break;
		}
	}
}

Real FlowEngine::Volume_cell_single_fictious ( CGT::Cell_handle cell)
{
	Real V[3][3];
	int b=0;
	int w=0;

	Real Wall_point[3];

	for ( int y=0;y<4;y++ )
	{
		if ( ! ( cell->vertex ( y )->info().isFictious ) )
		{
			const shared_ptr<Body>& sph = Body::byId
			                              ( cell->vertex ( y )->info().id(), scene );
			for ( int g=0;g<3;g++ ) V[w][g]=sph->state->pos[g];
			w++;
		}
		else
		{
			b = cell->vertex ( y )->info().id()-network->id_offset;
			const shared_ptr<Body>& wll = Body::byId ( b , scene );
			for ( int i=0;i<3;i++ ) Wall_point[i] = network->boundaries[b].p[i];
	Wall_point[network->boundaries[b].coordinate] = wll->state->pos[network->boundaries[b].coordinate]+(network->boundaries[b].normal[network->boundaries[b].coordinate])*wall_thickness;
		}
	}

	double v1[3], v2[3];

	for ( int g=0;g<3;g++ ) { v1[g]=V[0][g]-V[1][g]; v2[g]=V[0][g]-V[2][g];}

	Real Volume = ( CGAL::cross_product ( CGT::Vecteur ( v1[0],v1[1],v1[2] ),
	                                      CGT::Vecteur ( v2[0],v2[1],v2[2] ) ) *
	                network->boundaries[b].normal ) * ( 0.33333333333* ( V[0][network->boundaries[b].coordinate]+ V[1][network->boundaries[b].coordinate]+ V[2][network->boundaries[b].coordinate] ) - Wall_point[network->boundaries[b].coordinate] );

	return abs ( Volume );
}

Real FlowEngine::Volume_cell_double_fictious ( CGT::Cell_handle cell)
{
	Real A[3]={0, 0, 0}, AS[3]={0, 0, 0}, AT[3]={0, 0, 0};
	Real B[3]={0, 0, 0}, BS[3]={0, 0, 0}, BT[3]={0, 0, 0};
	Real C[3]={0, 0, 0}, CS[3]={0, 0, 0}, CT[3]={0, 0, 0};	

	//Real A[3], AS[3], AT[3];
	//Real B[3], BS[3], BT[3];
	//Real C[3], CS[3], CT[3];
	int b[2];

	Real Wall_point[2][3];

	int j=0;
	bool first_sph=true;
	for ( int g=0;g<4;g++ )
	{
		if ( cell->vertex ( g )->info().isFictious )
		{
			b[j] = cell->vertex ( g )->info().id()-network->id_offset;
			const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
			for ( int i=0;i<3;i++ ) Wall_point[j][i] = network->boundaries[b[j]].p[i];
			Wall_point[j][network->boundaries[b[j]].coordinate] = wll->state->pos[network->boundaries[b[j]].coordinate] +(network->boundaries[b[j]].normal[network->boundaries[b[j]].coordinate])*wall_thickness;
			j++;
		}
		else if ( first_sph )
		{
			const shared_ptr<Body>& sph1 = Body::byId
			                               ( cell->vertex ( g )->info().id(), scene );
			for ( int k=0;k<3;k++ ) { A[k]=AS[k]=AT[k]=sph1->state->pos[k]; first_sph=false;}
		}
		else
		{
			const shared_ptr<Body>& sph2 = Body::byId
			                               ( cell->vertex ( g )->info().id(), scene );
			for ( int k=0;k<3;k++ ) { B[k]=BS[k]=BT[k]=sph2->state->pos[k]; }
		}
	}

	AS[network->boundaries[b[0]].coordinate]=BS[network->boundaries[b[0]].coordinate] = Wall_point[0][network->boundaries[b[0]].coordinate];
	AT[network->boundaries[b[1]].coordinate]=BT[network->boundaries[b[1]].coordinate] = Wall_point[1][network->boundaries[b[1]].coordinate];

	for ( int h=0;h<3;h++ ) {C[h]= ( A[h]+B[h] ) /2; CS[h]= ( AS[h]+BS[h] ) /2; CT[h]= ( AT[h]+BT[h] ) /2;}

	CGT::Vecteur v1 ( AT[0]-BT[0],AT[1]-BT[1],AT[2]-BT[2] );
	CGT::Vecteur v2 ( C[0]-CT[0],C[1]-CT[1],C[2]-CT[2] );

	Real h = C[network->boundaries[b[0]].coordinate]- CS[network->boundaries[b[0]].coordinate];

	Real Volume = ( CGAL::cross_product ( v1,v2 ) *network->boundaries[b[0]].normal ) *h;

	return abs ( Volume );
}

Real FlowEngine::Volume_cell_triple_fictious ( CGT::Cell_handle cell)
{
	Real A[3]={0, 0, 0}, AS[3]={0, 0, 0}, AT[3]={0, 0, 0}, AW[3]={0, 0, 0};	
//Real A[3], AS[3], AT[3], AW[3];
// 	CGT::Boundary b[3];
	int b[3];
	Real Wall_point[3][3];
	int j=0;

	for ( int g=0;g<4;g++ )
	{
		if ( cell->vertex ( g )->info().isFictious )
		{
			b[j] = cell->vertex ( g )->info().id()-network->id_offset;
			const shared_ptr<Body>& wll = Body::byId ( b[j] , scene );
			for ( int i=0;i<3;i++ ) Wall_point[j][i] = network->boundaries[b[j]].p[i];
			Wall_point[j][network->boundaries[b[j]].coordinate] = wll->state->pos[network->boundaries[b[j]].coordinate]+(network->boundaries[b[j]].normal[network->boundaries[b[j]].coordinate])*wall_thickness;
			j++;
		}
		else
		{
			const shared_ptr<Body>& sph = Body::byId
			                              ( cell->vertex ( g )->info().id(), scene );
			for ( int k=0;k<3;k++ ) { A[k]=AS[k]=AT[k]=AW[k]=sph->state->pos[k];}
		}
	}

	AS[network->boundaries[b[0]].coordinate]= AT[network->boundaries[b[0]].coordinate]= AW[network->boundaries[b[0]].coordinate]= Wall_point[0][network->boundaries[b[0]].coordinate];
	AT[network->boundaries[b[1]].coordinate]= Wall_point[1][network->boundaries[b[1]].coordinate];
	AW[network->boundaries[b[2]].coordinate]= Wall_point[2][network->boundaries[b[2]].coordinate];

	CGT::Vecteur v1 ( AS[0]-AT[0],AS[1]-AT[1],AS[2]-AT[2] );
	CGT::Vecteur v2 ( AS[0]-AW[0],AS[1]-AW[1],AS[2]-AW[2] );

	CGT::Vecteur h ( AT[0] - A[0], AT[1] - A[1], AT[2] - A[2] );

	Real Volume = ( CGAL::cross_product ( v1,v2 ) ) * h;

	return abs ( Volume );
}

Real FlowEngine::Volume_cell ( CGT::Cell_handle cell)
{
	Vector3r A[4];
	int j=0;

	for ( int y=0;y<4;y++ )
	{
		const shared_ptr<Body>& sph = Body::byId
		                              ( cell->vertex ( y )->info().id(), scene );
		for ( int i=0;i<3;i++ ) A[j]=sph->state->pos;
		j++;
	}

	CGT::Point p1 ( ( A[0] ) [0], ( A[0] ) [1], ( A[0] ) [2] );
	CGT::Point p2 ( ( A[1] ) [0], ( A[1] ) [1], ( A[1] ) [2] );
	CGT::Point p3 ( ( A[2] ) [0], ( A[2] ) [1], ( A[2] ) [2] );
	CGT::Point p4 ( ( A[3] ) [0], ( A[3] ) [1], ( A[3] ) [2] );

	Real Volume = ( std::abs ( ( CGT::Tetraedre ( p1,p2,p3,p4 ).volume() ) ) );

	return abs ( Volume );
}

YADE_PLUGIN ((FlowEngine));
#endif //FLOW_ENGINE

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
		CGT::Boundary b = network->boundaries[cell->vertex ( j )->info().id() ];
		const shared_ptr<Body>& wall = Body::byId
				( cell->vertex ( j )->info().id(), scene );

		surface = network->surface_external_triple_fictious ( pos, cell, b );

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
// 						b1 = network->boundaries[cell->vertex ( g )->info().id() ];
// 						const shared_ptr<Body>& wll1 = Body::byId
// 						                               ( cell->vertex ( g )->info().id() , scene );
//
// 						Vel_W1=wll1->state->vel;
// 						first_boundary=false;
// 					}
// 					else
// 					{
// 						b2 = network->boundaries[cell->vertex ( g )->info().id() ];
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

