/*

Dessine des sph�res en 3D dans une fen�tre avec sauvegardes JPEG

*/


// #ifdef XVIEW
// 	#include "Vue3D.h"
// #endif

#include "FlowBoundingSphere.h"
#include "CGAL/constructions/constructions_on_weighted_points_cartesian_3.h"
#include <CGAL/Width_3.h>

#include <iostream>
#include <fstream>
#include <new>
#include <utility>
#include "vector"
// #include "Wm3Math.h"
// #include "Wm3Math.cpp"

#define FAST
//#define TESS_BASED_FORCES
// #define XVIEW
#include "def_types.h"


using namespace std;
namespace CGT
{

	const double RELAX = 1.9;
	const double TOLERANCE = 1e-09;

	bool TAKE_LATERALS = true;//compute pressure in lateral cells
	bool SLIP_ON_LATERALS = false;//no-slip/symmetry conditions on lateral boundaries

	int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};
	int permutations_facet [3][3] = {{0,1,2},{1,2,0},{2,0,1}};
	int permutations_vertices [4][4] = {{0,1,2,3},{1,2,0,3},{2,0,1,3},{3,2,1,0}};



	/*static*/ Point Corner_min;
	/*static*/ Point Corner_max;

// Tesselation Tes;

	bool Bizarre = false;

	double P_SUP = 1, P_INF = 0;

	typedef vector<double> VectorR;

#ifdef XVIEW
	Vue3D Vue1;
#endif

	FlowBoundingSphere::FlowBoundingSphere ( void )
	{
// 		x_min = 1000.0, x_max = -10000.0, y_min = 1000.0, y_max = -10000.0, z_min = 1000.0, z_max = -10000.0;
// 		currentTes = 0;
		Vsolid_tot = 0, Vtotalissimo = 0, Vporale = 0, Ssolid_tot = 0;
	}


	void FlowBoundingSphere::insert ( Real x, Real y, Real z, Real radius, int id )
	{
		Vertex_handle Vh;
		Vh = T[currentTes].insert ( x,y,z,radius,id );

		x_min = min ( x_min,Vh->point() [0]-radius );
		x_max = max ( x_max,Vh->point() [0]+radius );
		y_min = min ( y_min,Vh->point() [1]-radius );
		y_max = max ( y_max,Vh->point() [1]+radius );
		z_min = min ( z_min,Vh->point() [2]-radius );
		z_max = max ( z_max,Vh->point() [2]+radius );
		
// 		cout << "xmin = " << x_min << endl;

// 	Rmoy += rayon;
	}

	void FlowBoundingSphere::Tesselate ( )
	{
		T[currentTes].Compute();
	}

	void FlowBoundingSphere::Compute_Action ( )
	{
		Compute_Action ( 0,NULL,NULL );
	}

	void FlowBoundingSphere::Compute_Action ( int argc, char *argv[ ], char *envp[ ] )
	{
		Analytical_Consolidation();

		int hh=2;
		cout << "_________test________ " << facetVertices[hh][0] << " " << facetVertices[hh][1] << " " << facetVertices[hh][2] << endl;

		cout << "_________test2________ " << permutations_vertices[hh][0] << " " << permutations_vertices[hh][1] << " " << permutations_vertices[hh][2] << " " << permutations_vertices[hh][3] << endl;
// 	std::ofstream oFile ( "COMSOLcomparison",std::ios::out );

// 	for (double factor=0.90; factor<1.14; factor += 0.05)
// 	{

		double factor = 1.0;

// 	Vecteur* disp;

		VectorR X, Y, Z, R;

		Real_timer clock;
		clock.start();
		clock.top ( "start" );

		Tesselation& Tes = T[0];

		RTriangulation& Tri = Tes.Triangulation();

		/** READING SPHERES POSITIONS FROM A TEXT FILE CONTAINING COORDINATES **/

		double x, y, z, r, wx, wc;
		int nOfSpheres = 0;

		ifstream loadFile ( argc==1 ? "cube" : argv[1] ); // cree l'objet loadFile de la classe ifstream qui va permettre de lire importFilename
		int nPeriods = ( argc>2 ) ? atoi ( argv[2] ) : 1;

//  if ( !loadFile ) { cout << ( argc==1 ? "positions_YADE_2000" : argv[1] ) << " not found" << endl;};

		while ( !loadFile.eof() ) // tant qu'on n'est pas a la fin du fichier
		{
			loadFile >> x >> y >> z >> r >> wx >> wc;
			X.push_back ( x );
			Y.push_back ( y );
			Z.push_back ( z );
			R.push_back ( factor*r );
			nOfSpheres++;
		}

		Vertex_handle Vh;

		int V = X.size();
		cout << "V=" << V << "nOfSpheres=" << nOfSpheres << endl;

		Cell_handle neighbour_cell, cell, location;

		double SectionArea=0, Rmoy=0, H=0;

		Real rayon = 0;

// 	x_min = 1000.0, x_max = -10000.0, y_min = 1000.0, y_max = -10000.0, z_min = 1000.0, z_max = -10000.0;

		for ( int i=0; i<V; i++ )
		{
			Vh = Tes.insert ( X[i],Y[i],Z[i], /*factor**/R[i], i+6 ); /** EMPILEMENT QUELCONQUE **/

// 		Vh = Tes.insert ( X[i]+0.0001*Rmoy*Rand_d(),Y[i]+0.0001*Rmoy*Rand_d(),Z[i]+0.0001*Rmoy*Rand_d(), factor*R[i], i );
			/** EMPILEMENT REGULIER **/

#ifdef XVIEW
			Vue1.SetSpheresColor ( 0.8,0.6,0.6,1 );
			Vue1.Dessine_Sphere ( X[i],Y[i],Z[i], R[i], 15 );
#endif
			rayon = R[i];

			Rmoy += rayon;

			x_min = min ( x_min,Vh->point() [0]-rayon );
			x_max = max ( x_max,Vh->point() [0]+rayon );
			y_min = min ( y_min,Vh->point() [1]-rayon );
			y_max = max ( y_max,Vh->point() [1]+rayon );
			z_min = min ( z_min,Vh->point() [2]-rayon );
			z_max = max ( z_max,Vh->point() [2]+rayon );
		}

		Rmoy /= V;

		if ( 0 )  ///Periodic duplication
		{
			cout << "nperiods = " << nPeriods << endl;
			Real pdx=x_max-x_min;
			Real pdy=y_max-y_min;
			Real pdz=z_max-z_min;
			//pdx*=1.001;pdy*=1.001;pdz*=1.001;
			int V2=V;
			H = nPeriods* ( y_max - y_min );
			for ( int npx=0; npx<nPeriods; npx++ )
			{
				for ( int npy=0; npy<nPeriods; npy++ )
				{
					for ( int npz=0; npz<nPeriods; npz++ )
					{
						if ( ( npx+npy+npz ) > 0 )
						{
							for ( int ng=0; ng<V; ng++ )
							{
								///A random noise on coordinates is needed to avoid CGAL crash on periodic packings
								double x = X[ng]+ ( npx+0.01*Rmoy*Rand_d() ) *pdx;
								double y = Y[ng]+ ( npy+0.01*Rmoy*Rand_d() ) *pdy;
								double z = Z[ng]+ ( npz+0.01*Rmoy*Rand_d() ) *pdz;
								double r = R[ng];
								//cout << "inserting "<< x<<" "<<y<<" "<<z<<" "<< r <<" "<<V2<<endl;
								Vh = Tes.insert ( x,y,z, r, V2++ );
#ifdef XVIEW
								Vue1.Dessine_Sphere ( x,y,z, r, 15 );
#endif
								//cout << V2 << "inserted" << endl;
								x_min = min ( x_min,Vh->point() [0]-R[ng] );
								x_max = max ( x_max,Vh->point() [0]+R[ng] );
								y_min = min ( y_min,Vh->point() [1]-R[ng] );
								y_max = max ( y_max,Vh->point() [1]+R[ng] );
								z_min = min ( z_min,Vh->point() [2]-R[ng] );
								z_max = max ( z_max,Vh->point() [2]+R[ng] );
							}
						}
// 			X.push_back ( x );
// 			Y.push_back ( y );
// 			Z.push_back ( z );
// 			R.push_back ( r );
//   X2.push_back ( x );
//   Y2.push_back ( y );
//   Z2.push_back ( z );
					}
				}
			}
			cout << nPeriods<<" period(s) triangulated"<<endl;
		}


		H = y_max-y_min;

		SectionArea = ( x_max-x_min ) * ( z_max-z_min );

		clock.top ( "loading spheres + Triangulation" );

		AddBoundingPlanes ( Tes );

// 	Tri = Tes.Triangulation();

		Tes.Compute();

		clock.top ( "tesselation" );

		cout << "Rmoy " << Rmoy << endl;
		cout << "x_min = " << x_min << endl;
		cout << "x_max = " << x_max << endl;
		cout << "y_max = " << y_max << endl;
		cout << "y_min = " << y_min << endl;
		cout << "z_min = " << z_min << endl;
		cout << "z_max = " << z_max << endl;
		cout << "SectionArea = " << SectionArea << endl;

		Localize ( Tri );
		clock.top ( "Localize" );

		//GenerateVoxelFile(Tri); ///*GENERATION OF A VOXEL FILE *///

		/** INITIALIZATION OF VOLUMES AND PRESSURES **/

		Finite_cells_iterator cell_end = Tri.finite_cells_end();
//
		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ ) {cell->info().dv() = 0;}

		clock.top ( "initializing delta_volumes" );

		/** PERMEABILITY **/
		/** START PERMEABILITY CALCULATION**/

		Compute_Permeability ( Tri );

		clock.top ( "Compute_Permeability" );

		/** END PERMEABILITY CALCULATION**/

		/** STATISTICS **/

		DisplayStatistics ( Tri );
		clock.top ( "DisplayStatistics" );

		/** START GAUSS SEIDEL */

// 	Boundary_Conditions ( Tri );

		GaussSeidel ( Tri );

		clock.top ( "GaussSeidel" );

		/** END GAUSS SEIDEL */

		Permeameter ( Tri, P_INF, P_SUP, SectionArea, H );
		clock.top ( "Permeameter" );

		Compute_Forces ( Tri );
		clock.top ( "Compute_Forces" );

// 	oFile << factor << " " << Permeability << endl;

// 	save_vtk_file ( Tri );

		/** VISUALISATION FILES **/

		PermeameterCurve ( Tri );

		MGPost ( Tri );

		///*** VUE 3D ***///

//Vue1.SetCouleurSegments ( 0,10,0 );
// 	Dessine_Triangulation ( Vue1, Tes.Triangulation() );
#ifdef XVIEW
		Vue1.SetCouleurSegments ( 0.1,0,1 );
		Dessine_Short_Tesselation ( Vue1, Tes );

		Vue1.Affiche();
#endif
	}

	void FlowBoundingSphere::SpheresFileCreator ()
	{
		std::ofstream oFile ( "SAMPLE_POSITIONS",std::ios::out );

		for ( long i=0; i<3000; i++ )
		{
			double x=Rand_d(), y=Rand_d(), z=Rand_d(), rad=Rand_d() *0.03+0.02;
			oFile  << x << " " << y << " " << z << " " << rad << endl;
		}
	}

	void FlowBoundingSphere::Analytical_Consolidation ( )
	{
		std::ofstream oFile ( "Analytical_Consolidation",std::ios::out );

		int H = 10;
		double U = 0, Tv = 0, p0 = 10, dz = H/10;

		for ( double z=0; z<=H; z+=dz )
		{
			U=0;
			double Z = 2*z/H;
			for ( int m=0; m<10; m++ )
			{
				double M = ( ( 2*m+1 ) *M_PI ) /2;
				double u_m = ( 2/M ) *sin ( 2*M*z/H ) *exp ( - ( M*M*Tv ) );
				U+=u_m;
			}
			oFile << ( 1-U ) << " " << Z << endl;
		}
	}

	void FlowBoundingSphere::Boundary_Conditions ( RTriangulation& Tri )
	{
		Finite_vertices_iterator vertices_end = Tri.finite_vertices_end ();

		for ( Finite_vertices_iterator V_it = Tri.finite_vertices_begin (); V_it !=  vertices_end; V_it++ )
		{
			if ( V_it->info().isFictious )
			{
// 				if ( boundary ( V_it->info().id() ).coordinate == 1 )
				if ( boundaries[V_it->info().id()].coordinate == 1 )
				{
// 					boundary ( V_it->info().id() ).condition = 1;
					boundaries[V_it->info().id()].condition = 1;
					if ( boundaries[V_it->info().id()].p == Corner_min )
					{
						V_it->cell()->info().p() = P_INF;
						V_it->cell()->info().isExternal = true;
					}
					else
					{
						V_it->cell()->info().p() = P_SUP;
						V_it->cell()->info().isExternal = true;
					}
				}
				else
				{
					boundaries[V_it->info().id()].condition = 0;
// 					boundary ( V_it->info().id() ).condition = 0;
					V_it->cell()->info().p() = V_it->cell()->info().y();
				}
			}
			else {V_it->cell()->info().p() = V_it->cell()->info().y();}
		}
	}

	void FlowBoundingSphere::Interpolate ( Tesselation& Tes, Tesselation& NewTes )
	{
		Cell_handle old_cell;

		RTriangulation& NewTri = NewTes.Triangulation();
		RTriangulation& Tri = Tes.Triangulation();
		Finite_cells_iterator cell_end = NewTri.finite_cells_end();

		/**CALCULATION OF VORONOI CENTRES*/
		if ( !NewTes.Computed() ) NewTes.Compute();

		for ( Finite_cells_iterator new_cell = NewTri.finite_cells_begin(); new_cell != cell_end; new_cell++ )
		{
			old_cell = Tri.locate ( ( Point ) new_cell->info() );
			new_cell->info().p() = old_cell->info().p();
			new_cell->info().dv() = old_cell->info().dv();
		}
// 	return NewTes;
	}

	void FlowBoundingSphere::Localize ()
	{
		cout << "Localize--------------" << endl;
		RTriangulation& Tri = T[currentTes].Triangulation();
		Localize ( Tri );
	}

	void FlowBoundingSphere::Localize ( RTriangulation& Tri )
	{
		Vertex_handle V;
		bool pass = 0;
		Finite_cells_iterator cell_end = Tri.finite_cells_end();
		
		cout << "Localizing..." << endl;

		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			pass = 0;
			for ( int i=0;i<4;i++ )
			{
				V = cell->vertex ( i );
				if ( V->info().isFictious )
				{
					pass = 1;
					cell->info().isFictious = true;
// 					Boundary& bi = boundary ( V->info().id() );
					Boundary& bi = boundaries [V->info().id()];

					if ( bi.coordinate==0 || bi.coordinate==2 )
					{
						if ( !cell->info().isSuperior && !cell->info().isInferior )
						{
							cell->info().isLateral = true;
#ifdef XVIEW
							Vue1.SetSpheresColor ( 0.1,0.95,0.1,1 );
							Vue1.Dessine_Sphere ( cell->info().x(), cell->info().y(), cell->info().z(), 0.02 , 4 );
#endif
						}
					}
					else
					{
						if ( ( bi.normal ) [bi.coordinate] > 0 )
							{cell->info().isInferior=true;cell->info().isLateral = false;}
						else {cell->info().isSuperior=true;cell->info().isLateral = false;}
#ifdef XVIEW
						Vue1.SetSpheresColor ( 1,0.1,0.1,1 );
						Vue1.Dessine_Sphere ( cell->info().x(), cell->info().y(), cell->info().z(), 0.02 , 4 );
#endif
					}
				}
			}
			if ( !pass ) cell->info().isInside=true;
		}
		cout << "Localised -------------" << endl;
	}

	void FlowBoundingSphere::Compute_Permeability ( )
	{
		cout << "Permeability--------------------" << endl;
		RTriangulation& Tri = T[currentTes].Triangulation();
		Compute_Permeability ( Tri );
	}

	void FlowBoundingSphere::Compute_Permeability ( RTriangulation& Tri )
	{
		Finite_cells_iterator cell_end = Tri.finite_cells_end();

		Cell_handle neighbour_cell;
		Point p1, p2;

		double Rhv=0, k=0, distance = 0, radius = 0, viscosity = 1;
		int NEG=0, POS=0, pass=0;

		bool ref = Tri.finite_cells_begin()->info().isvisited;

		Vecteur n;

		std::ofstream oFile ( "Hydraulic_Radius",std::ios::out );

		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			p1 = cell->info();
			for ( int j=0; j<4; j++ )
			{
				neighbour_cell = cell->neighbor ( j );
				p2 = neighbour_cell->info();
				if ( !Tri.is_infinite ( neighbour_cell ) && neighbour_cell->info().isvisited==ref )
				{
					Bizarre = false;

					pass+=1;

					Rhv = Compute_HydraulicRadius ( Tri, cell, j );
					
					if ( Rhv<0 ) NEG+=0; else POS+=0;

					( cell->info().Rh() ) [j]=Rhv;
					( neighbour_cell->info().Rh() ) [Tri.mirror_index ( cell, j ) ]= Rhv;

					oFile << pass << " " << Rhv << endl;

					Vecteur l = p1 - p2;
					distance = sqrt ( l.squared_length() );

					n = l/distance;

					radius = 2* Rhv;
					
					if ( radius==0 ) {cout << "INS-INS PROBLEM!!!!!!!" << endl;}

					if ( distance!=0 )
					{
						k = ( M_PI * pow ( radius,4 ) ) / ( 8*viscosity*distance );
					}
					else  k = 10000;

					( cell->info().k_norm() ) [j]= k;
					( cell->info().k_vector() ) [j]= k*n;
					( neighbour_cell->info().k_norm() ) [Tri.mirror_index ( cell, j ) ]= k;
					( neighbour_cell->info().k_vector() ) [Tri.mirror_index ( cell, j ) ]= ( -k ) *n;
				}
// 			else if ( Tri.is_infinite ( neighbour_cell )) k = 0;//connection to an infinite cells
			}
			cell->info().isvisited = !ref;
		}
		cout << "POS = " << POS << " NEG = " << NEG << " pass = " << pass << endl;

		Finite_vertices_iterator vertices_end = Tri.finite_vertices_end ();
		Real Vgrains = 0;
		int grains=0;

		for ( Finite_vertices_iterator V_it = Tri.finite_vertices_begin (); V_it !=  vertices_end; V_it++ )
		{
			if ( !V_it->info().isFictious )
			{
				grains +=1;
				Vgrains += 1.33333333 * M_PI * pow ( V_it->point().weight(),1.5 );
			}
		}

		Real Vtotale = ( x_max-x_min ) * ( y_max-y_min ) * ( z_max-z_min );

		cout << grains << " grains ---> " << "Vtotale = " << Vtotale << " Vgrains = " << Vgrains << " Vporale1 = " << Vtotale-Vgrains << endl;

		cout << "Vtotalissimo = " << Vtotalissimo << " Vsolid_tot = " << Vsolid_tot << " Vporale2 = " << Vporale  << " Ssolid_tot = " << Ssolid_tot << endl<< endl;
	}

	void FlowBoundingSphere::DisplayStatistics ( )
	{
		RTriangulation& Tri = T[currentTes].Triangulation();
		DisplayStatistics ( Tri );
	}


	void FlowBoundingSphere::DisplayStatistics ( RTriangulation& Tri )
	{
		int Zero =0, Inside=0, Superior=0, Inferior=0, Lateral=0, Fictious=0;
		Finite_cells_iterator cell_end = Tri.finite_cells_end();

		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			int zeros =0;
			for ( int j=0; j!=4; j++ )
				{if ( ( cell->info().k_norm() ) [j]==0 ) {zeros+=1;}}

			if ( zeros==4 ) {Zero+=1;}
			if ( cell->info().isInside ) {Inside+=1;}
			if ( cell->info().isSuperior ) {Superior+=1;}
			if ( cell->info().isInferior ) {Inferior+=1;}
			if ( cell->info().isLateral ) {Lateral+=1;}
			if ( cell->info().isFictious ) {Fictious+=1;}
		}

		cout << "zeros = " << Zero << endl;
		
		int fict=0, real=0;
		
		for ( Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v ) 
		{
			if (v->info().isFictious) fict+=1; else real+=1;
		}

		long Vertices = Tri.number_of_vertices ();
		cout << "There are " << Vertices << " vertices, dont " << fict << " fictious et " << real << " reeeeeel" << std::endl;

		long Cells = Tri.number_of_finite_cells();
		cout << "There are " << Cells << " cells " << std::endl;

		long Facets = Tri.number_of_finite_facets();
		cout << "There are " << Facets << " facets " << std::endl;

		cout << "There are " << Inside << " cells INSIDE." << endl;
		cout << "There are " << Lateral << " cells LATERAL." << endl;
		cout << "There are " << Inferior << " cells INFERIOR." << endl;
		cout << "There are " << Superior << " cells SUPERIOR." << endl;
		cout << "There are " << Fictious << " cells FICTIOUS." << endl;
	}

	void FlowBoundingSphere::Compute_Forces ()
	{
		RTriangulation& Tri = T[currentTes].Triangulation();
		Compute_Forces ( Tri );
	}

	void FlowBoundingSphere::Compute_Forces ( RTriangulation& Tri )
	{
		Finite_cells_iterator cell_end = Tri.finite_cells_end();

		Vecteur nullVect ( 0,0,0 );
		Vecteur TotalForce = nullVect;

#ifndef TESS_BASED_FORCES
		for ( Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v ) {v->info().forces=nullVect;}

		Cell_handle neighbour_cell;
		Vertex_handle V;

		Point p1, p2;

		Vecteur Cell_Force= nullVect, Vertex_force= nullVect;

		Real invSsolid;
		int fict =0, V_fict=0;

		bool ref = Tri.finite_cells_begin()->info().isvisited;
		bool single_external_force, double_external_force, triple_external_force;
		bool VV [3];

		double info = 0, Sk=0;

		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			V_fict = 0;
			single_external_force = false, double_external_force=false, triple_external_force=false;

			if ( !cell->info().isInside )
				{for ( int i=0;i<4;i++ ) {if ( cell->vertex ( i )->info().isFictious ) {V_fict+=1;}}}

			switch ( V_fict )
			{
				case ( 1 ) : single_external_force=true; break;
				case ( 2 ) : double_external_force=true; break;
				case ( 3 ) : triple_external_force=true; break;
			}

			p1 = cell->info();
			Cell_Force = nullVect, Vertex_force = nullVect;

			for ( int j=0; j<4; j++ )
			{
				neighbour_cell = cell->neighbor ( j );
				if ( !Tri.is_infinite ( neighbour_cell ) && neighbour_cell->info().isvisited==ref )
				{
					p2 = neighbour_cell->info();

					Vecteur branch = p1 - p2;

					fict = 0;
// 				Vertex_handle Wf;//one finite vertex for later use

					Vertex_handle Vertices [3];
					Sphere facet_spheres [3];

					VV [0]=false;VV [1]=false;VV [2]=false;

					for ( int y=0;y<3;y++ )
					{
						Vertices [y] = cell->vertex ( facetVertices[j][y] );
						facet_spheres [y] = Vertices [y]->point();
						if ( Vertices [y]->info().isFictious ) {fict+=1; VV[y]=true;}
					}

					Triangle t ( facet_spheres [0],facet_spheres [1],facet_spheres [2] );

					if ( fict==2 )
					{
						Vertex_handle SW[3];

						for ( int y=0; y<3; y++ ) {SW[y]=Vertices[y];}
						if ( !VV[1] ) {SW[1]=Vertices[2];SW[2]=Vertices[1];}
						else if ( !VV[0] ) {SW[0]=Vertices[2];SW[2]=Vertices[0];}

						Sk = surface_solid_fictious_facet ( SW[0],SW[1],SW[2] );
					}
					else
					{
						Sk = sqrt ( t.squared_area () );
						for ( int y=0;y<3;y++ )
						{
							Sk-=surface_solid_facet ( facet_spheres[permutations_facet[y][0]], facet_spheres[permutations_facet[y][1]], facet_spheres[permutations_facet[y][2]] );
						}
					}

					if ( Sk==0 || Sk<0 ) {cout << "Problem surface!!!!!!!" << endl;}

					Vecteur n = p2 - p1;
					Real norme = sqrt ( n.squared_length() );
					n = n/norme;

					Real Pk = cell->info().p() + ( ( facet_spheres[0]-p1 ) *n ) / ( ( p2-p1 ) *n ) * ( neighbour_cell->info().p() - cell->info().p() );
					//linear interpolation between p1 and p2

					Vecteur v_int = cell->vertex ( j )->point() - facet_spheres[0];
					//a vector oriented toward the interior of the cell
					info = dotProduct ( v_int, n );//check "n" versus this vector v_ext

					if ( info < 0 )	{n = -n;}

					( cell->info().force() ) [j] = ( Pk * Sk ) * n;
					( neighbour_cell->info().force() ) [ Tri.mirror_index ( cell,j ) ] = ( Pk * Sk ) * ( -n );
				}
				else if ( Tri.is_infinite ( neighbour_cell ) ) { ( cell->info().force() ) [j] = nullVect;}
			}
			cell->info().isvisited = !ref;


			for ( int j=0;j<4;j++ ) {Cell_Force = Cell_Force + ( cell->info().force() ) [j];}

			if ( cell->info().isFictious )
			{
				Vecteur add_force = nullVect;
				Boundary b;
				Sk = 0;
				for ( int h=0; h<4; h++ )
				{
					if ( cell->vertex ( h )->info().isFictious )
					{
// 						b = boundary ( cell->vertex ( h )->info().id() );
						b = boundaries [cell->vertex ( h )->info().id()];

						if ( single_external_force ) Sk = surface_external_single_fictious ( cell, b );
						else if ( double_external_force ) Sk = surface_external_double_fictious ( cell, b );
						else if ( triple_external_force ) Sk = surface_external_triple_fictious ( cell, b );

						add_force = add_force + cell->info().p() *Sk*b.normal;
						cell->vertex ( h )->info().forces = cell->vertex ( h )->info().forces + cell->info().p() *Sk*b.normal;
					}
				}
				Cell_Force = Cell_Force + add_force;
			}

			double Ssolid = 0;
			double Ssolid_portion[4];
			Ssolid_portion[0]=0;Ssolid_portion[1]=0;Ssolid_portion[2]=0;Ssolid_portion[3]=0;
			Sphere V[4];

			for ( int y=0;y<4;y++ ) { V[y] = cell->vertex ( y )->point();}

// 		for ( int q=0;q<4;q++ )
// 		{
// 			if ( cell->vertex ( q )->info().isFictious )
// 			{
// 				if ( !cell->info().isSuperior && !cell->info().isInferior )
// 				{
// 					Ssolid_portion[q] = fast_spherical_triangle_area ( V[permutations_vertices[q][0]],V[permutations_vertices[q][1]],V[permutations_vertices[q][2]],V[permutations_vertices[q][3]] );
// 				}
// 			}
// 			else {Ssolid_portion[q] = fast_spherical_triangle_area ( V[permutations_vertices[q][0]],V[permutations_vertices[q][1]],V[permutations_vertices[q][2]],V[permutations_vertices[q][3]] );}
// 		}


			for ( int q=0;q<4;q++ )
			{
				if ( cell->vertex ( q )->info().isFictious )
				{
// 					if ( boundary ( cell->vertex ( q )->info().id() ).coordinate!=1 )
					if ( boundaries [cell->vertex ( q )->info().id()].coordinate!=1 )
					{

						if ( single_external_force ) Ssolid_portion[q] = surface_external_single_fictious 
							( cell, boundaries [cell->vertex ( q )->info().id()] );
						if ( double_external_force ) Ssolid_portion[q] = surface_external_double_fictious ( cell, boundaries [cell->vertex ( q )->info().id()] );
						if ( triple_external_force ) Ssolid_portion[q] = surface_external_triple_fictious ( cell, boundaries [cell->vertex ( q )->info().id()] );
					}
				}
				else Ssolid_portion[q] = fast_spherical_triangle_area ( V[permutations_vertices[q][0]], V[permutations_vertices[q][1]], V[permutations_vertices[q][2]], V[permutations_vertices[q][3]] );
			}

			for ( int k=0;k<4;k++ ) Ssolid += Ssolid_portion[k];

			invSsolid = 1/Ssolid;

			if ( Ssolid==0 ) cout << "inv solid = " << invSsolid << endl;

			for ( int h=0;h<4;h++ )
			{
				if ( !cell->vertex ( h )->info().isFictious )
				{
					cell->vertex ( h )->info().forces = cell->vertex ( h )->info().forces + Cell_Force * Ssolid_portion[h] * invSsolid;
				}
			}
		}

#else

		for ( Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v )
		{
			v->info().forces = nullVect;
		}

		Real fsurf = 0.5;//= 0.5/-0.5 for defining the direction of branch vector, "0.5" is for the average pressure
		///FIXME :all facets computed twice : the forces are x2, it needs a "visited" check before computing anything
		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			for ( int kk=0; kk<4;kk++ )  //Compute forces
			{

				Point& c1 = cell->info();
				Point& c2 = cell->neighbor ( kk )->info();
				Vertex_handle vertices [3] = { cell->vertex ( facetVertices[kk][0] ),
				                               cell->vertex ( facetVertices[kk][1] ),
				                               cell->vertex ( facetVertices[kk][2] )
				                             };
				Point& s1 = cell->vertex ( facetVertices[kk][0] )->point();
				Point& s2 = cell->vertex ( facetVertices[kk][1] )->point();
				Point& s3 = cell->vertex ( facetVertices[kk][2] )->point();

				Point middles [3] = { s2 + ( s3-s2 ) * ( ( c1-s2 ) * ( s3-s2 ) ) / ( ( s3-s2 ) * ( s3-s2 ) ),
				                      s1 + ( s3-s1 ) * ( ( c1-s1 ) * ( s3-s1 ) ) / ( ( s3-s1 ) * ( s3-s1 ) ),
				                      s1 + ( s2-s1 ) * ( ( c1-s1 ) * ( s2-s1 ) ) / ( ( s2-s1 ) * ( s2-s1 ) )
				                    };

				Real& pr1 = cell->info().p();
				Real& pr2 = cell->neighbor ( kk )->info().p();

// 			if ( !vertices[0]->info().isFictious || !vertices[1]->info().isFictious || !vertices[2]->info().isFictious )
				{
					for ( int jj=0; jj<3; jj++ )
					{
						if ( !vertices[permutations_facet[jj][0]]->info().isFictious || !vertices[permutations_facet[jj][1]]->info().isFictious )
						{
							int& i1 = permutations_facet[jj][0];
							int& i2 = permutations_facet[jj][1];
							int& i3 = permutations_facet[jj][2];
							Vecteur surface = 0.5*CGAL::cross_product ( middles[i3]-c1,middles[i3]-c2 );

							if ( surface* ( vertices[i2]->point()-vertices[i1]->point() ) <0 ) fsurf = -0.5; else fsurf = 0.5;///= 0.5/-0.5 for defining the direction of branch vector, "0.5" is for the mean pressure

							vertices[i1]->info().forces = vertices[i1]->info().forces-fsurf* ( pr1+pr2 ) *surface;
							vertices[i2]->info().forces = vertices[i2]->info().forces+fsurf* ( pr1+pr2 ) *surface;
						}
					}
				}
			}
		}

#endif


		for ( Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v )
		{
//   if ( v->info().id() == 8 ) {cout << "Force on small sphere = " << v->info().forces << endl;}
			if ( !v->info().isFictious )
			{
				/*if ( v->info().id()==8)*/
// 				cout << "id = " << v->info().id() << " force = " << v->info().forces << endl;

				TotalForce = TotalForce + v->info().forces;
			}
			else {cout << "fictious_id = " << v->info().id() << " force = " << v->info().forces << endl;}
		}
		cout << "TotalForce = "<< TotalForce << endl;
	}

	double FlowBoundingSphere::surface_external_triple_fictious ( Cell_handle cell, Boundary b )
	{
		vector<int> vertex_fictious_vec;

		vector<Boundary> B;

		double A[3], AS[3], AT[3];

		for ( int i=0;i<4;i++ )
		{
			if ( cell->vertex ( i )->info().isFictious && ( boundaries [cell->vertex ( i )->info().id()].normal != b.normal ) )
			{
				B.push_back ( boundaries [cell->vertex ( i )->info().id()] );
				vertex_fictious_vec.push_back ( i );
			}
			else if ( !cell->vertex ( i )->info().isFictious ) {Point V = cell->vertex ( i )->point(); for ( int m=0; m<3; m++ ) { A[m]=AS[m]=AT[m]= ( V ) [m];}}
		}

		A[b.coordinate]=AS[b.coordinate]=AT[b.coordinate]=b.p[b.coordinate];
		AS[B[0].coordinate]=B[0].p[B[0].coordinate];
		AT[B[1].coordinate]=B[1].p[B[1].coordinate];

		double a1[3], a2[3];

		for ( int i=0;i<3;i++ ) { a1[i] = A[i] - AS[i]; a2[i] = A[i] - AT[i];}

		Vecteur A1 ( a1[0],a1[1],a1[2] );
		Vecteur A2 ( a2[0],a2[1],a2[2] );

		Vecteur Sk = CGAL::cross_product ( A1,A2 );

		double sk = sqrt ( Sk.squared_length() );

		return sk;
	}
	
	double FlowBoundingSphere::surface_external_triple_fictious ( Real position[3], Cell_handle cell, Boundary b )
	{
		double A[3], AS[3], AT[3];
		Boundary B[2]; int d=0;
		
		for ( int m=0; m<3; m++ ) { A[m]=AS[m]=AT[m]= ( position ) [m];}
		for ( int i=0;i<4;i++ )
		{
			if ( cell->vertex ( i )->info().isFictious && ( boundaries [cell->vertex ( i )->info().id()].normal != b.normal ))
			{
				B[d] = boundaries [cell->vertex ( i )->info().id()];
				d++;
			}
		}

		A[b.coordinate]=AS[b.coordinate]=AT[b.coordinate]=b.p[b.coordinate];
		AS[B[0].coordinate]=B[0].p[B[0].coordinate];
		AT[B[1].coordinate]=B[1].p[B[1].coordinate];

		double a1[3], a2[3];

		for ( int i=0;i<3;i++ ) { a1[i] = A[i] - AS[i]; a2[i] = A[i] - AT[i];}

		Vecteur A1 ( a1[0],a1[1],a1[2] );
		Vecteur A2 ( a2[0],a2[1],a2[2] );

		Vecteur Sk = CGAL::cross_product ( A1,A2 );

		double sk = sqrt ( Sk.squared_length() );

		return sk;
	}

	double FlowBoundingSphere::surface_external_double_fictious ( Cell_handle cell, Boundary b )
	{
		Boundary bs;
		Point V[2];

		int j=0;
		for ( int i=0;i!=4;i++ )
		{
			if ( cell->vertex ( i )->info().isFictious && ( boundaries [cell->vertex ( i )->info().id()].normal != b.normal ) )
			{
				bs = boundaries [cell->vertex ( i )->info().id()];
			}
			else if ( !cell->vertex ( i )->info().isFictious ) {V[j]= ( cell->vertex ( i )->point() );j++;}
		}

		double A[3], B[3], H1[3], H2[3];

		for ( int m=0; m<3; m++ )
		{
			A[m]= H1[m]= ( V[0] ) [m];
			B[m]= H2[m]= ( V[1] ) [m];
		}

		A[b.coordinate]=b.p[b.coordinate];
		B[b.coordinate]=b.p[b.coordinate];

		H1[b.coordinate]=b.p[b.coordinate];
		H1[bs.coordinate]=bs.p[bs.coordinate];

		H2[b.coordinate]=b.p[b.coordinate];
		H2[bs.coordinate]=bs.p[bs.coordinate];

// 	cout << "A = " << A[0] << " " << A[1] << " " << A[2] << endl << "B = " << B[0] << " " << B[1] << " " << B[2] << endl << "H1 = " << H1[0] << " " << H1[1] << " " << H1[2] << endl << "H2 = " << H2[0] << " " << H2[1] << " " << H2[2] << endl << endl;

		double a1[3],b1[3];

		for ( int i=0;i<3;i++ ) { a1[i] = A[i] - B[i]; b1[i] = A[i] - H1[i];}
		Vecteur A1 ( a1[0],a1[1],a1[2] );
		Vecteur A2 ( b1[0],b1[1],b1[2] );

		Vecteur Sk1 = 0.5*CGAL::cross_product ( A1,A2 );

		double sk1 = sqrt ( Sk1.squared_length() );

		for ( int i=0;i<3;i++ ) { a1[i] = H2[i] - H1[i]; b1[i] = H2[i] - B[i];}
		Vecteur AS1 ( a1[0],a1[1],a1[2] );
		Vecteur AS2 ( b1[0],b1[1],b1[2] );

		Vecteur Sk2 = 0.5*CGAL::cross_product ( AS1,AS2 );

		double sk2 = sqrt ( Sk2.squared_length() );

		double Sk = sk1 + sk2;

		return Sk;
	}

	double FlowBoundingSphere::surface_external_single_fictious ( Cell_handle cell, Boundary b )
	{
		Point V[3];
		double a1[3],b1[3];

		int j=0;
		for ( int i=0;i!=4;i++ )
		{
			if ( !cell->vertex ( i )->info().isFictious )
			{
				V[j] = ( cell->vertex ( i )->point() );j++;
			}
		}

		double A [3], B [3], C [3];

		for ( int m=0; m<3;m++ ) {A[m]= ( V[0] ) [m];B[m]= ( V[1] ) [m];C[m]= ( V[2] ) [m];}

		A[b.coordinate]=b.p[b.coordinate];
		B[b.coordinate]=b.p[b.coordinate];
		C[b.coordinate]=b.p[b.coordinate];

		for ( int i=0;i<3;i++ ) { a1[i] = A[i] - B[i]; b1[i] = A[i] - C[i];}

		Vecteur A1 ( a1[0],a1[1],a1[2] );
		Vecteur A2 ( b1[0],b1[1],b1[2] );

		Vecteur Sk = 0.5*CGAL::cross_product ( A1,A2 );

		double sk = sqrt ( Sk.squared_length() );

		return sk;
	}

// 	double FlowBoundingSphere::epsilon_V_triple_fictious ( Cell_handle cell )
// 	{
// 		Boundary b;
//
// 		for (int g=0;g<4;g++)
// 		{
// 			if (cell->vertex(g)->info().isFictious)
// 			{
// 				b = boundary ( cell->vertex ( g )->info().id() );
// 				double sk = surface_external_triple_fictious (cell,b);
// 			}
// 		}
// 	}
	
	void FlowBoundingSphere::Fictious_cells ( )
	{
		Finite_cells_iterator cell_end = T[currentTes].Triangulation().finite_cells_end();
		
		int V_fict=0;
	
		cout << "Localizing fictious cells......" << endl;

		for ( Finite_cells_iterator cell = T[currentTes].Triangulation().finite_cells_begin(); cell != cell_end; cell++ )
		{
			V_fict = 0;
			for (int g=0;g<4;g++)
			{
				if (cell->vertex(g)->info().isFictious) V_fict+=1;
			}
			switch ( V_fict )
			{
				case ( 0 ) : cell->info().fictious() = 0; break;
				case ( 1 ) : cell->info().fictious() = 1; break;
				case ( 2 ) : cell->info().fictious() = 2; break;
				case ( 3 ) : cell->info().fictious() = 3; break;
			}
		}
	
		cout << "Fictious cells localized ------------" << endl;
	}
	
	double FlowBoundingSphere::volume_double_fictious_pore ( Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1 )
	{
		double A [3], B[3], C[3];
// 		Boundary &bi1 = boundary ( SV1->info().id() );
// 		Boundary &bi2 = boundary ( SV2->info().id() );
		
		Boundary &bi1 = boundaries [SV1->info().id()];
		Boundary &bi2 = boundaries [SV2->info().id()];
		
		for ( int m=0;m<3;m++ ) {A[m]=B[m]=C[m]= ( SV3->point() ) [m];}
		
		A[bi1.coordinate]=bi1.p[bi1.coordinate];
		B[bi2.coordinate]=bi2.p[bi2.coordinate];
		C[bi1.coordinate]=bi1.p[bi1.coordinate];
		C[bi2.coordinate]=bi2.p[bi2.coordinate];
		

// 	cout << "A = " << A[0] << " " << A[1] << " " << A[2] << endl;
// 	cout << "B = " << B[0] << " " << B[1] << " " << B[2] << endl;
// 	cout << "C = " << C[0] << " " << C[1] << " " << C[2] << endl;

		Point AA ( A[0],A[1],A[2] );
		Point BB ( B[0],B[1],B[2] );
		Point CC ( C[0],C[1],C[2] );

		Sphere A1 ( AA, 0 );
		Sphere B1 ( BB, 0 );
		Sphere C1 ( CC, 0 );
		Sphere SW3 = SV3->point();

		Real Vtot1 = ( std::abs ( ( Tetraedre ( AA, BB, CC, PV1 ) ).volume() ) );
		Real Vtot2 = ( std::abs ( ( Tetraedre ( AA, BB, SW3, PV1 ) ).volume() ) );

		Real Vsolid1 = spherical_triangle_volume ( SW3, A1, C1, PV1 );
		Real Vsolid2 = spherical_triangle_volume ( SW3, C1, B1, PV1 );

		Real Vpore = Vtot1 + Vtot2 - Vsolid1 - Vsolid2 ;

		Vtotalissimo += Vtot1 + Vtot2;
		Vporale += Vpore;
		Vsolid_tot += Vsolid1 + Vsolid2;

		return ( Vpore );
	}

	double FlowBoundingSphere::volume_single_fictious_pore ( Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1 )
	{
		double A [3], B[3];
		
// 		Boundary &bi1 = boundary ( SV1->info().id() );
		Boundary &bi1 = boundaries [SV1->info().id()];
		
		for ( int m=0;m<3;m++ ) {A[m]= ( SV2->point() ) [m];}
		for ( int m=0;m<3;m++ ) {B[m]= ( SV3->point() ) [m];}
		
		A[bi1.coordinate]=bi1.p[bi1.coordinate];
		B[bi1.coordinate]=bi1.p[bi1.coordinate];

		Point AA ( A[0],A[1],A[2] );
		Point BB ( B[0],B[1],B[2] );
		
		Sphere A1 ( AA, 0 );
		Sphere B1 ( BB, 0 );
		Sphere SW2 = SV2->point();
		Sphere SW3 = SV3->point();
		
		Real Vtot1 = ( std::abs ( ( Tetraedre ( AA, SW2, SW3, PV1 ) ).volume() ) );
		Real Vtot2 = ( std::abs ( ( Tetraedre ( SW3, AA, BB, PV1 ) ).volume() ) );
		
		Real Vsolid1 = spherical_triangle_volume ( SW2, A1, SW3, PV1 );
		Real Vsolid2 = spherical_triangle_volume ( SW3, SW2, B1, PV1 );

		Real Vpore = Vtot1 + Vtot2 - Vsolid1 - Vsolid2 ;

		Vsolid_tot += Vsolid1 + Vsolid2;
		Vtotalissimo += Vtot1 + Vtot2;
		Vporale += Vpore;
		
		return ( Vpore );
	}

	double FlowBoundingSphere::Compute_HydraulicRadius ( RTriangulation& Tri, Cell_handle cell, int j )
	{
		Cell_handle neighbour_cell;
		neighbour_cell = cell->neighbor ( j );

		Point p1 = cell->info();
		Point p2 = neighbour_cell->info();

		Vertex_handle SV1, SV2, SV3;

		double Vtot=0, Vsolid=0, Vpore=0, Vsolid1=0, Vsolid2=0, Vol1=0, Vol2=0;

		int F1=0, F2=0, Re1=0, Re2=0;
		int fictious_vertex=0, real_vertex=0;

		double Ssolid1=0, Ssolid1n=0, Ssolid2=0, Ssolid2n=0, Ssolid3=0, Ssolid3n=0, Ssolid=0;

		bool fictious_solid = false;

		Sphere v [3];
		Vertex_handle W [3];
		for ( int kk=0; kk<3; kk++ )
		{
			
			W[kk] = cell->vertex ( facetVertices[j][kk] );
			v[kk] = cell->vertex ( facetVertices[j][kk] )->point();
			if ( W[kk]->info().isFictious )
			{
				if ( fictious_vertex==0 ) {F1=facetVertices[j][kk];}
				else {F2 = facetVertices[j][kk];}
				fictious_vertex +=1;
			}
			else
			{
				if ( real_vertex==0 ) {Re1=facetVertices[j][kk];}
				else if ( real_vertex==1 ) Re2=facetVertices[j][kk];
				real_vertex+=1;
			}
		}
		

		if ( fictious_vertex==2 )
		{
			double A [3], B[3], C[3];

			/**PORE VOLUME**/
			Vpore = volume_double_fictious_pore ( cell->vertex ( F1 ), cell->vertex ( F2 ), cell->vertex ( Re1 ), p1 );
			Vpore += volume_double_fictious_pore ( cell->vertex ( F1 ), cell->vertex ( F2 ), cell->vertex ( Re1 ), p2 );
			/** **/

			/**PORE SOLID SURFACE**/
			Boundary &bi1 = boundaries [cell->vertex ( F1 )->info().id()];
			Boundary &bi2 = boundaries [cell->vertex ( F2 )->info().id()];
// 			Boundary &bi1 = boundary ( cell->vertex ( F1 )->info().id() );
// 			Boundary &bi2 = boundary ( cell->vertex ( F2 )->info().id() );
			for ( int m=0;m<3;m++ ) {A[m]=B[m]=C[m]= ( cell->vertex ( Re1 )->point() ) [m];}

			A[bi1.coordinate]=bi1.p[bi1.coordinate];
			B[bi2.coordinate]=bi2.p[bi2.coordinate];
			C[bi1.coordinate]=bi1.p[bi1.coordinate];
			C[bi2.coordinate]=bi2.p[bi2.coordinate];
			Point AA ( A[0],A[1],A[2] );
			Point BB ( B[0],B[1],B[2] );
			Point CC ( C[0],C[1],C[2] );

			Sphere A1 ( AA, 0 );
			Sphere B1 ( BB, 0 );
			Sphere C1 ( CC, 0 );
			Ssolid1 = fast_spherical_triangle_area ( cell->vertex ( Re1 )->point(), A1, B1, p1 );
			Ssolid1n = fast_spherical_triangle_area ( cell->vertex ( Re1 )->point(), A1, B1, p2 );
			if ( bi1.coordinate!=1 )
			{
				if ( !SLIP_ON_LATERALS )
				{
					Triangle t ( p1, A1, C1 );
					Ssolid2 = sqrt ( t.squared_area () );
					if ( !Tri.is_infinite ( neighbour_cell ) )
					{
						Triangle t2 ( p2, A1, C1 );
						Ssolid2n = sqrt ( t2.squared_area () );
					}
				}
			}

			if ( bi2.coordinate!=1 )
			{
				if ( !SLIP_ON_LATERALS )
				{
					Triangle t ( p1, B1, C1 );
					Ssolid3 = sqrt ( t.squared_area () );
					if ( !Tri.is_infinite ( neighbour_cell ) )
					{
						Triangle t2 ( p2, B1, C1 );
						Ssolid3n = sqrt ( t2.squared_area () );
					}
				}
			}
		}
		
		else
		{
			if ( fictious_vertex==1 )
			{
				SV1 = cell->vertex ( F1 );
				SV2 = cell->vertex ( Re1 );
				SV3 = cell->vertex ( Re2 );
				Vpore = volume_single_fictious_pore ( SV1, SV2, SV3, p1 );
				Vpore += volume_single_fictious_pore ( SV1, SV2, SV3, p2 );
// 				Boundary &bi1 = boundary ( SV1->info().id() );
				Boundary &bi1 = boundaries [SV1->info().id()];
				if ( bi1.coordinate!=1 && !SLIP_ON_LATERALS ) fictious_solid = true;
				
			}
			else
			{
				Vol1 = ( Tetraedre ( v[0], v[1], v[2], p1 ) ).volume();
				Vol2 = ( Tetraedre ( v[0], v[1], v[2], p2 ) ).volume();

				Vsolid1=0;Vsolid2=0;
				for ( int i=0;i<3;i++ )
				{
					Vsolid1 += spherical_triangle_volume ( v[permutations_facet[i][0]],v[permutations_facet[i][1]],v[permutations_facet[i][2]],p1 );
					Vsolid2 += spherical_triangle_volume ( v[permutations_facet[i][0]],v[permutations_facet[i][1]],v[permutations_facet[i][2]],p2 );
				}

				if ( ( Vol1*Vol2 ) > 0 )
				{
					Bizarre = true;
					Vsolid = std::abs ( Vsolid1 - Vsolid2 );
				}
				else
				{
					Bizarre = false;
					Vsolid = Vsolid1 + Vsolid2;
				}
				Vtot = std::abs ( Vol1 - Vol2 );

				Vsolid_tot += Vsolid;
				Vtotalissimo += Vtot;

				Vpore = Vtot - Vsolid;

				Vporale += Vpore;

				SV1 = W[0]; SV2 = W[1]; SV3 = W[2];
				fictious_solid = true;
			}

			if ( fictious_solid )
			{
				Ssolid1 = fast_spherical_triangle_area ( SV1->point(), SV2->point(), SV3->point(), p1 );
				Ssolid1n = fast_spherical_triangle_area ( SV1->point(), SV2->point(), SV3->point(), p2 );
			}
			Ssolid2 = fast_spherical_triangle_area ( SV2->point(),SV1->point(),SV3->point(), p1 );
			Ssolid2n = fast_spherical_triangle_area ( SV2->point(),SV1->point(),SV3->point(), p2 );
			Ssolid3 = fast_spherical_triangle_area ( SV3->point(),SV2->point(),SV1->point(), p1 );
			Ssolid3n = fast_spherical_triangle_area ( SV3->point(),SV2->point(),SV1->point(), p2 );
		}

		if ( Bizarre )
		{
			Ssolid += std::abs ( Ssolid2 - Ssolid2n );
			Ssolid += std::abs ( Ssolid1 - Ssolid1n );
			Ssolid += std::abs ( Ssolid3 - Ssolid3n );

		}
		else
		{
			Ssolid += std::abs ( Ssolid1 + Ssolid1n );
			Ssolid += std::abs ( Ssolid2 + Ssolid2n );
			Ssolid += std::abs ( Ssolid3 + Ssolid3n );
		}
	
		Ssolid_tot += Ssolid;
		return Vpore/Ssolid;
	}

	double FlowBoundingSphere::dotProduct ( Vecteur x, Vecteur y )
	{
		int i;
		double sum=0;

		for ( i=0; i<3; i++ )
			{sum += x[i]*y[i];}
		return sum;
	}

	double FlowBoundingSphere::crossProduct ( double x[3], double y[3] )
	{

		double sq_normeX = sqrt ( x[0]*x[0]+ x[1]*x[1]+ x[2]*x[2] );
		double sq_normeY = sqrt ( y[0]*y[0]+ y[1]*y[1]+ y[2]*y[2] );

		double cosTeta = ( x[0]*y[0]+x[1]*y[1]+x[2]*y[2] ) / ( ( sq_normeY * sq_normeX ) );

		double Teta = acos ( cosTeta );

		double cPmodule = ( sq_normeX*sq_normeY ) * sin ( Teta );

		return cPmodule;
	}

	double FlowBoundingSphere::surface_solid_fictious_facet ( Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3 )
	{
		double A [3], B [3];

		for ( int m=0;m<3;m++ ) {A[m]=B[m]= ( SV3->point() ) [m];}

// 		Boundary &bi1 = boundary ( SV1->info().id() );
// 		Boundary &bi2 = boundary ( SV2->info().id() );
		
		Boundary &bi1 = boundaries [SV1->info().id()];
		Boundary &bi2 = boundaries [SV2->info().id()];

		A[bi1.coordinate]=bi1.p[bi1.coordinate];
		B[bi2.coordinate]=bi2.p[bi2.coordinate];

		double board1=0, board2=0, total_surface=0;
		for ( int p=0;p<3;p++ )
		{
			board1 += pow ( ( SV3->point() [p]-A[p] ),2 );
			board2 += pow ( ( SV3->point() [p]-B[p] ),2 );
		}
		total_surface = sqrt ( board1 * board2 );

		double solid_surface = surface_solid_facet ( SV3->point(),SV2->point(),SV1->point() );

		return total_surface - solid_surface;
	}

	double FlowBoundingSphere::surface_solid_facet ( Sphere ST1, Sphere ST2, Sphere ST3 )
	{
		double Area;

		double squared_rayon = ST1.weight();

		Vecteur v12 = ST2.point() - ST1.point();
		Vecteur v13 = ST3.point() - ST1.point();

		double norme12 =  v12.squared_length();
		double norme13 =  v13.squared_length();

		double cosA = v12*v13 / ( sqrt ( norme13 * norme12 ) );
		double A = acos ( cosA );

		Area = ( A/ ( 2*M_PI ) ) * ( M_PI * squared_rayon );

		return Area;
	}

	double FlowBoundingSphere::spherical_triangle_volume ( Sphere ST1, Sphere ST2, Sphere ST3, Point PT1 )
	{
		double rayon = sqrt ( ST1.weight() );
		if ( rayon == 0.0 ) return 0.0;

		return ( ( 0.3333333 * rayon ) * ( fast_spherical_triangle_area ( ST1, ST2, ST3, PT1 ) ) ) ;
	}

	double FlowBoundingSphere::spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 )
	{
		double rayon = STA1.weight();
		if ( rayon == 0.0 ) return 0.0;

		Vecteur v12 = STA2.point() - STA1.point();
		Vecteur v13 = STA3.point() - STA1.point();
		Vecteur v14 = PTA1 - STA1.point();

		double norme12 = ( v12.squared_length () );
		double norme13 = ( v13.squared_length () );
		double norme14 = ( v14.squared_length () );

		double cosA = v12*v13 / sqrt ( ( norme13 * norme12 ) );
		double cosB = v12*v14 / sqrt ( ( norme14 * norme12 ) );
		double cosC = v14*v13 / sqrt ( ( norme13 * norme14 ) );

		double A = acos ( cosA );
		double B = acos ( cosB );
		double C = acos ( cosC );
		if ( A==0 || B==0 || C==0 ) return 0;

		double a = acos ( ( cosA - cosB * cosC ) / ( sin ( B ) * sin ( C ) ) );
		double b = acos ( ( cosB - cosC * cosA ) / ( sin ( C ) * sin ( A ) ) );
		double c = acos ( ( cosC - cosA * cosB ) / ( sin ( A ) * sin ( B ) ) );

		double aire_triangle_spherique = rayon * ( a + b + c - M_PI );

		return  aire_triangle_spherique;
	}

	double FlowBoundingSphere::fast_spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 )
	{
//! This function needs to be fast because it is used heavily. Avoid using vector operations which require constructing vectors (~50% of cpu time in the non-fast version), and compute angle using the 3x faster formula of Oosterom and StrackeeVan Oosterom, A; Strackee, J (1983). "The Solid Angle of a Plane Triangle". IEEE Trans. Biom. Eng. BME-30 (2): 125-126. (or check http://en.wikipedia.org/wiki/Solid_angle)
		using namespace CGAL;
#ifndef FAST
		return spherical_triangle_area ( STA1, STA2, STA3, PTA1 );
#endif

		double rayon2 = STA1.weight();
		if ( rayon2 == 0.0 ) return 0.0;

// 	int other_vertices [3];
// 	int k=0;
// 	for ( int i = 0; i<4; i++ ) other_vertices[k++] = ( i!=vertex ) ? i : ++i;

		double M[3][3];

// 	for (int j=0; j<3; j++)
// 	{
// 		M[j][0] = (cell->vertex ( other_vertices[0] )->point().point())[j] - (cell->vertex ( vertex )->point().point())[j];
// 		M[j][1] = (cell->vertex ( other_vertices[1] )->point().point())[j] - (cell->vertex ( vertex )->point().point())[j];
// 		M[j][2] = (cell->vertex ( other_vertices[2] )->point().point())[j] - (cell->vertex ( vertex )->point().point())[j];

		M[0][0] = STA2.x() - STA1.x();
		M[0][1] = STA3.x() - STA1.x();
		M[0][2] = PTA1.x() - STA1.x();

		M[1][0] = STA2.y() - STA1.y();
		M[1][1] = STA3.y() - STA1.y();
		M[1][2] = PTA1.y() - STA1.y();

		M[2][0] = STA2.z() - STA1.z();
		M[2][1] = STA3.z() - STA1.z();
		M[2][2] = PTA1.z() - STA1.z();
// 	}

		///
		double detM = M[0][0]* ( M[1][1]*M[2][2]-M[2][1]*M[1][2] ) +
		              M[1][0]* ( M[2][1]*M[0][2]-M[0][1]*M[2][2] ) +
		              M[2][0]* ( M[0][1]*M[1][2]-M[1][1]*M[0][2] );

		double pv12N2 = pow ( M[0][0],2 ) +pow ( M[1][0],2 ) +pow ( M[2][0],2 );
		double pv13N2 = pow ( M[0][1],2 ) +pow ( M[1][1],2 ) +pow ( M[2][1],2 );
		double pv14N2 = pow ( M[0][2],2 ) +pow ( M[1][2],2 ) +pow ( M[2][2],2 );

		double pv12N = sqrt ( pv12N2 );
		double pv13N = sqrt ( pv13N2 );
		double pv14N = sqrt ( pv14N2 );

		double cp12 = ( M[0][0]*M[0][1]+M[1][0]*M[1][1]+M[2][0]*M[2][1] );
		double cp13 = ( M[0][0]*M[0][2]+M[1][0]*M[1][2]+M[2][0]*M[2][2] );
		double cp23 = ( M[0][1]*M[0][2]+M[1][1]*M[1][2]+M[2][1]*M[2][2] );

		double ratio = detM/ ( pv12N*pv13N*pv14N+cp12*pv14N+cp13*pv13N+cp23*pv12N );

		return abs ( rayon2 * 2*atan ( ratio ) );
	}

	void FlowBoundingSphere::GaussSeidel ( )
	{
		RTriangulation& Tri = T[currentTes].Triangulation();
		GaussSeidel ( Tri );
	}

	void FlowBoundingSphere::GaussSeidel ( RTriangulation& Tri )
	{
		int j = 0;
		double m, n, dp_max, p_max, sum_p, p_moy, dp_moy, dp, sum_dp;
		double tolerance = TOLERANCE;
		double relax = RELAX;

		Finite_cells_iterator cell_end = Tri.finite_cells_end();

		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			if ( cell->info().isInside || cell->info().isLateral )
			{
				cell->info().p() = cell->info().y();
			}
			else if ( cell->info().isSuperior )
			{
				cell->info().p() = P_SUP;
			}
			else if ( cell->info().isInferior )
			{
				cell->info().p() = P_INF;
			}
			if ( isnan ( cell->info().p() ) ) cout << "OOOOOOOOOOOOOOOOOOH1111111111" << endl;
		}

		do
		{
			int cell2=0;

			dp_max = 0; p_max = 0; p_moy=0; dp_moy=0; sum_p=0; sum_dp=0;

			for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
			{
				m=0, n=0;

// 			if ( isnan ( cell->info().p() ) ) cout << "OOOOOOOOOOOOOOOOOOH2222222" << endl;

				if ( ( /*!cell->info().isExternal*/ !cell->info().isSuperior && !cell->info().isInferior ) )
				{
					cell2++;

					for ( int j2=0; j2<4; j2++ )
					{
						if ( !Tri.is_infinite ( cell->neighbor ( j2 ) ) )
						{
							m += ( cell->info().k_norm() ) [j2] * cell->neighbor ( j2 )->info().p();
							n += ( cell->info().k_norm() ) [j2];
						}
					}

					dp = cell->info().p();
					if ( n!=0 )
					{
//     cell->info().p() =   - ( cell->info().dv() - m ) / ( n );
						cell->info().p() = ( - ( cell->info().dv() - m ) / ( n ) - cell->info().p() ) * relax + cell->info().p();
					}

					dp -= cell->info().p();

					dp_max = std::max ( dp_max, std::abs ( dp ) );
					p_max = std::max ( p_max, std::abs ( cell->info().p() ) );
					sum_p += std::abs ( cell->info().p() );
					sum_dp += std::abs ( dp );
				}
			}

			p_moy = sum_p/cell2;
			dp_moy = sum_dp/cell2;

			j++;

			if ( j % 1000 == 0 )
			{
// 			cout << "pmax " << p_max << "; pmoy : " << p_moy << endl;
// 			cout << "iteration " << j <<"; erreur : " << dp_max/p_max << endl;
// 			save_vtk_file ( Tri );
			}
		}
		while ( ( dp_max/*/p_max*/ ) > tolerance  /*( dp_max > tolerance )*//* &&*/ /*( j<50 )*/ );
	}


	void FlowBoundingSphere::Permeameter ( RTriangulation &Tri, double P_Inf, double P_Sup, double Section, double DeltaY )
	{
		double PRESSION_EXT;
		//Vecteur Qin, Qout;
		double Qin=0, Qout=0;
		int cellQout=0, cellQin=0;
		double p_out_max=0, p_out_min=100000, p_in_max=0, p_in_min=100000,p_out_moy=0, p_in_moy=0;
//  Qin = [0,0,0];
//  Qout = [0,0,0];
		Finite_cells_iterator cell_end = Tri.finite_cells_end();

		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			if ( !cell->info().isSuperior && !cell->info().isInferior )
			{
				for ( int j2=0; j2<4; j2++ )
				{
					Cell_handle neighbour_cell = cell->neighbor ( j2 );

					if ( ( neighbour_cell->info().isInferior ) )
					{
						PRESSION_EXT=P_Inf;
						//Qout = Qout + ( cell->info().k_vector() ) [j2]* ( cell->info().p()-PRESSION_EXT );
						Qout = Qout + ( cell->info().k_norm() ) [j2]* ( PRESSION_EXT-cell->info().p() );
						cellQout+=1;/*( cell->info().k_vector() ) [j2]* ( cell->info().p()-PRESSION_EXT )*/;
						p_out_max = std::max ( cell->info().p(), p_out_max );
						p_out_min = std::min ( cell->info().p(), p_out_min );
						p_out_moy += cell->info().p();
					}
					if ( neighbour_cell->info().isSuperior )
					{
						PRESSION_EXT=P_Sup;
						//Qin = Qin + ( cell->info().k_vector() ) [j2]* ( cell->info().p()-PRESSION_EXT );
						Qin = Qin + ( cell->info().k_norm() ) [j2]* ( cell->info().p()-PRESSION_EXT );
						cellQin+=1;
						p_in_max = std::max ( cell->info().p(), p_in_max );
						p_in_min = std::min ( cell->info().p(), p_in_min );
						p_in_moy += cell->info().p();
					}
				}
			}
		}
		cout << "the maximum superior pressure is = " << p_in_max << " the min is = " << p_in_min << endl;
		cout << "the maximum inferior pressure is = " << p_out_max << " the min is = " << p_out_min << endl;

		cout << "superior average pressure is " << p_in_moy/cellQin << endl;
		cout << "inferior average pressure is " << p_out_moy/cellQout << endl;

		cout << "celle comunicanti in basso = " << cellQout << endl;
		cout << "celle comunicanti in alto = " << cellQin << endl;

		double density = 1;
		double viscosity = 1;
		double gravity = 1;
		double Vdarcy = Qout/Section;

		double GradP = ( P_Inf-P_Sup ) /DeltaY;

		double GradH = GradP/ ( density*gravity );

		double Ks= ( Vdarcy ) /GradH;

		double k= Ks*viscosity/ ( density*gravity );

		double k_darcy = Vdarcy*viscosity/ ( GradP );

		cout << "The incoming average flow rate is = " << Qin << " m^3/s " << endl;
		cout << "The outgoing average flow rate is = " << Qout << " m^3/s " << endl;
		cout << "The gradient of charge is = " << GradH << " [-]" << endl;
		cout << "Darcy's velocity is = " << Vdarcy << " m/s" <<endl;
		cout << "The hydraulic conductivity of the sample is = " << Ks << " m/s" <<endl;
		cout << "The permeability of the sample is = " << k << " m^2" <<endl;
		cout << "The Darcy permeability of the sample is = " << k_darcy/0.987e-12 << " darcys" << endl;
	}

	void FlowBoundingSphere::save_vtk_file ( RTriangulation &T )
	{
		static unsigned int number = 0;
		char filename[80];
		sprintf ( filename,"out_%d.vtk", number++ );

		basicVTKwritter vtkfile ( ( unsigned int ) T.number_of_vertices(), ( unsigned int ) T.number_of_finite_cells() );
		vtkfile.open ( filename,"test" );

		vtkfile.begin_vertices();
		double x,y,z;
		for ( Finite_vertices_iterator v = T.finite_vertices_begin(); v != T.finite_vertices_end(); ++v )
		{
			x = ( double ) ( v->point().point() [0] );
			y = ( double ) ( v->point().point() [1] );
			z = ( double ) ( v->point().point() [2] );
			vtkfile.write_point ( x,y,z );
		}
		vtkfile.end_vertices();

		vtkfile.begin_cells();
		for ( Finite_cells_iterator cell = T.finite_cells_begin(); cell != T.finite_cells_end(); ++cell )
		{
			vtkfile.write_cell ( cell->vertex ( 0 )->info().id(), cell->vertex ( 1 )->info().id(), cell->vertex ( 2 )->info().id(), cell->vertex ( 3 )->info().id() );
		}
		vtkfile.end_cells();
		vtkfile.begin_data ( "Pressure",CELL_DATA,SCALARS,FLOAT );

		for ( Finite_cells_iterator cell = T.finite_cells_begin(); cell != T.finite_cells_end(); ++cell )
		{
			vtkfile.write_data ( cell->info().p() );
		}
		vtkfile.end_data();

	}

	void FlowBoundingSphere::MGPost ( RTriangulation& Tri )
	{
		Point P;

		ofstream file ( "mgp.out.001" );
		file << "<?xml version=\"1.0\"?>" << endl;
		file << "<mgpost mode=\"3D\">" << endl;
		file << " <state time=\"0\">" << endl;

		Finite_cells_iterator cell_end = Tri.finite_cells_end();

		for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
		{
			double p [3] = {0,0,0};

			for ( int j2=0; j2!=4; j2++ )
			{
				Vertex_handle v = cell->vertex ( j2 );
				for ( int i=0; i<3;  i++ )
				{
					p[i] += 0.25* v->point().point() [i];
				}
			}

			double pressure =  cell->info().p();
			double rad = 0.05;

			file << "  <body>" << endl;
			file << "   <SPHER r=\""  <<  rad << "\">" << endl;
			file << "    <position x=\""  <<  p[0] << "\" y=\"" << p[1] << "\" z=\"" << p[2] << "\"/>" << endl;
			file << "    <velocity x=\""  <<  pressure << "\" y=\"" << pressure << "\" z=\"" << pressure << "\"/>" << endl;
			file << "   </SPHER>" << endl;
			file << "  </body>" << endl;
		}
		file << " </state>" << endl;
		file << "</mgpost>" << endl;
	}
#ifdef XVIEW
	void FlowBoundingSphere::Dessine_Triangulation ( Vue3D &Vue, RTriangulation &T )
	{
		double* Segments = NULL;
		long N_seg = New_liste_edges ( T, &Segments );
		Vue.Dessine_Segment ( Segments, N_seg );
		Delete_liste_edges ( &Segments, N_seg );
	}
	void FlowBoundingSphere::Dessine_Short_Tesselation ( Vue3D &Vue, Tesselation &Tes )
	{
		if ( !Tes.Computed() ) Tes.Compute();
		double* Segments = NULL;
		long N_seg = Tes.New_liste_short_edges ( &Segments );
		Vue.Dessine_Segment ( Segments, N_seg );
		Delete_liste_edges ( &Segments, N_seg );
	}
#endif
	void FlowBoundingSphere::GenerateVoxelFile ( RTriangulation& Tri )
	{
		double l = 1;
		int dx = 200;
		double eps = l/dx;

		std::ofstream voxelfile ( "MATRIX",std::ios::out );
		bool solid=false;

		for ( double y=0; y<=l; y+=eps )
		{
			for ( double z=0; z<=l; z+=eps )
			{
				for ( double x=0; x<=l; x+=eps )
				{
					solid=false;

					for ( Finite_vertices_iterator V_it = Tri.finite_vertices_begin (); V_it != Tri.finite_vertices_end(); V_it++ )
					{
						double rayon = sqrt ( V_it->point().weight() );
						if ( ( sqrt ( pow ( ( x- ( V_it->point() [0] ) ),2 ) +pow ( ( y- ( V_it->point() [1] ) ),2 ) +pow ( ( z- ( V_it->point() [2] ) ),2 ) ) ) <= rayon ) solid=true;
					}
					if ( solid ) voxelfile << 1;
					else voxelfile << 0;
				}
				voxelfile << endl;
			}
		}
	}

	void FlowBoundingSphere::PermeameterCurve ( RTriangulation& Tri )
	{
		/** CONSOLIDATION CURVES **/

		Cell_handle permeameter;
		int n=0; int k=0; int m=0; int o=0;
		vector<double> P_ave;

		std::ofstream consFile ( "PERMEAMETRE",std::ios::out );
		consFile << "j " << "p_ave " << endl;

		int intervals = 10;
		double Rx = ( x_max-x_min ) /intervals;
		double Ry = ( y_max-y_min ) /intervals;
		double Rz = ( z_max-z_min ) /intervals;

		for ( double Y=y_min; Y<=y_max; Y=Y+Ry )
		{
			P_ave.push_back ( 0 );
			for ( double X=x_min; X<=x_max; X=X+Rx )
			{
				for ( double Z=z_min; Z<=z_max; Z=Z+Rz )
				{
					permeameter = Tri.locate ( Point ( X, Y, Z ) );
					if ( permeameter->info().isInside )
					{
						n++;
						P_ave[k]+=permeameter->info().p();
					}
					if ( permeameter->info().isInferior )
					{
						m++;
						P_ave[k]+=permeameter->info().p();
					}
					if ( permeameter->info().isSuperior )
					{
						o++;
						P_ave[k]+=permeameter->info().p();
					}
				}
			}
			cout << "P_ave[" << k << "] = " << P_ave[k]/ ( m+n+o ) << " n = " << n << " m = " << m << " o = " << o << endl;
			P_ave[k]/= ( m+n+o );

			consFile << k << " "
			<< P_ave[k] << endl;

			n=0;m=0;o=0;
			k++;
		}
	}

	void FlowBoundingSphere::Sample_Permeability ( RTriangulation& Tri, double x_Min,double x_Max ,double y_Min,double y_Max,double z_Min,double z_Max )
	{
		double Section = ( x_Max-x_Min ) * ( z_Max-z_Min );

		double DeltaY = y_Max-y_Min;

		GaussSeidel ( Tri );

		Permeameter ( Tri, P_INF, P_SUP, Section, DeltaY );
	}

	void FlowBoundingSphere::AddBoundingPlanes ( Real center[3], Real Extents[3], int id )
	{
		Tesselation& Tes = T[currentTes];
		bool minimum=false;
		
		Corner_min = Point ( x_min, y_min, z_min );
		Corner_max = Point ( x_max, y_max, z_max );
		
		Real min_coord = min(Extents[0],min(Extents[1],Extents[2]));
		int coord=0;
		
		if (min_coord==Extents[0])
		{
			coord=0;
			if ((abs(center[0])-abs(center[1]))<0) minimum=true; else minimum=false;
		}
		else if(min_coord==Extents[1])
		{
			coord=1;
			if ((abs(center[1])-abs(center[0]))<0) minimum=true; else minimum=false;
		}
		else if (min_coord==Extents[2])
		{
			coord=2;
			if ((abs(center[2])-abs(center[0]))<0) minimum=true; else minimum=false;
		}
		
		double FAR = 50;
		
		if ( minimum ) boundaries[id].p = Corner_min; else boundaries[id].p = Corner_max;
		
		boundaries[id].coordinate = coord;
		
		switch ( coord )
		{
			case (0):
				{
					if ( minimum )
					{
						boundaries[id].normal = Vecteur ( -1,0,0 );
						Tes.insert ( Corner_min.x()-FAR* abs( center[1] ), center[1], center[2], FAR* center[1], id, true );
					}
					else
					{
						boundaries[id].normal = Vecteur ( 1,0,0 );
						Tes.insert ( Corner_max.x()+FAR* abs( center[1] ), center[1], center[2], FAR* center[1], id, true );
					}
				};break;
			case ( 1 ):
				{
					if ( minimum )
					{
						boundaries[id].normal = Vecteur ( 0,1,0 );
						Tes.insert ( center[0], Corner_min.y()-FAR* abs( center[0] ), center[2], FAR* center[0], id, true );
					}
					else 
					{
						boundaries[id].normal = Vecteur ( 0,-1,0 );
						Tes.insert ( center[0], Corner_max.y()+FAR* abs( center[0] ), center[2], FAR* center[0], id, true );
					}
				};break;
			case ( 2 ):
				{
					if ( minimum )
					{
						boundaries[id].normal = Vecteur ( 0,0,1 );
						Tes.insert ( center[0], center[1], Corner_min.z()-FAR* abs( center[1] ), FAR* center[1], id, true );
					}
					else 
					{
						boundaries[id].normal = Vecteur ( 0,0,-1 );
						Tes.insert ( center[0], center[1],Corner_max.z()+FAR* abs( center[1] ), FAR* center[1], id, true );
					}
				};break;
				
				
		}
	}

	void FlowBoundingSphere::AddBoundingPlanes ( )
	{
		cout << "Adding Boundary Planes----------" << endl;
		Tesselation& Tes = T[currentTes];
		AddBoundingPlanes ( Tes );

	}

	void FlowBoundingSphere::AddBoundingPlanes ( Tesselation& Tes )
	{
		double FAR = 50;

		Corner_min = Point ( x_min, y_min, z_min );
		Corner_max = Point ( x_max, y_max, z_max );
		
// 		y_min_id = Tes.Max_id() +1;
// 		y_max_id = Tes.Max_id() +2;
// 		x_min_id = Tes.Max_id() +3;
// 		x_max_id = Tes.Max_id() +4;
// 		z_min_id = Tes.Max_id() +5;
// 		z_max_id = Tes.Max_id() +6;

		id_offset = 0;//so that boundaries[vertex->id - offset] gives the ordered boundaries (also see function Boundary& boundary(int b))
		
// 		id_offset = Tes.Max_id() +1;//so that boundaries[vertex->id - offset] gives the ordered boundaries (also see function Boundary& boundary(int b))

		Tes.insert ( 0.5* ( Corner_min.x() +Corner_max.x() ), Corner_min.y()-FAR* ( Corner_max.x()-Corner_min.x() ), 0.5* ( Corner_max.z()-Corner_min.z() ), FAR* ( Corner_max.x()-Corner_min.x() ), 0/*y_min_id*/, true );
		boundaries[0].p = Corner_min;
		boundaries[0].normal = Vecteur ( 0,1,0 );
		boundaries[0].coordinate = 1;

		Tes.insert ( 0.5* ( Corner_min.x() +Corner_max.x() ), Corner_max.y() +FAR* ( Corner_max.x()-Corner_min.x() ), 0.5* ( Corner_max.z()-Corner_min.z() ), FAR* ( Corner_max.x()-Corner_min.x() ), 1/*y_max_id*/, true );
		boundaries[1].p = Corner_max;
		boundaries[1].normal = Vecteur ( 0,-1,0 );
		boundaries[1].coordinate = 1;

		Tes.insert ( Corner_min.x()-FAR* ( Corner_max.y()-Corner_min.y() ), 0.5* ( Corner_max.y()-Corner_min.y() ), 0.5* ( Corner_max.z()-Corner_min.z() ), FAR* ( Corner_max.y()-Corner_min.y() ), 2/*x_min_id*/, true );
		boundaries[2].p = Corner_min;
		boundaries[2].normal = Vecteur ( 1,0,0 );
		boundaries[2].coordinate = 0;

		Tes.insert ( Corner_max.x() +FAR* ( Corner_max.y()-Corner_min.y() ), 0.5* ( Corner_max.y()-Corner_min.y() ), 0.5* ( Corner_max.z()-Corner_min.z() ), FAR* ( Corner_max.y()-Corner_min.y() ), 3/*x_max_id*/, true );
		boundaries[3].p = Corner_max;
		boundaries[3].normal = Vecteur ( -1,0,0 );
		boundaries[3].coordinate = 0;

		Tes.insert ( 0.5* ( Corner_min.x() +Corner_max.x() ), 0.5* ( Corner_max.y()-Corner_min.y() ), Corner_min.z()-FAR* ( Corner_max.y()-Corner_min.y() ), FAR* ( Corner_max.y()-Corner_min.y() ),4 /*z_min_id*/, true );
		boundaries[4].p = Corner_min;
		boundaries[4].normal = Vecteur ( 0,0,1 );
		boundaries[4].coordinate = 2;

		Tes.insert ( 0.5* ( Corner_min.x() +Corner_max.x() ), 0.5* ( Corner_max.y()-Corner_min.y() ), Corner_max.z() +FAR* ( Corner_max.y()-Corner_min.y() ), FAR* ( Corner_max.y()-Corner_min.y() ),5 /*z_max_id*/, true );
		boundaries[5].p = Corner_max;
		boundaries[5].normal = Vecteur ( 0,0,-1 );
		boundaries[5].coordinate = 2;

	}

	bool FlowBoundingSphere::isInsideSphere ( RTriangulation& Tri, double x, double y, double z )
	{
		for ( Finite_vertices_iterator V_it = Tri.finite_vertices_begin (); V_it != Tri.finite_vertices_end(); V_it++ )
		{
			double rayon = V_it->point().weight();
			if ( pow ( ( x- ( V_it->point() [0] ) ),2 ) +pow ( ( y- ( V_it->point() [1] ) ),2 ) +pow ( ( z- ( V_it->point() [2] ) ),2 )   <= rayon ) return true;
		}
		return false;
	}

	void FlowBoundingSphere::SliceField ( RTriangulation& Tri )
	{
		/** Pressure field along one cutting plane **/

		Cell_handle permeameter;

		std::ofstream consFile ( "slices",std::ios::out );

		int intervals = 200;
		double Rx = 10* ( x_max-x_min ) /intervals;
		double Ry = ( y_max-y_min ) /intervals;
		double Rz = ( z_max-z_min ) /intervals;
		cout<<Rx<<" "<<Ry<<" "<<Rz<<" "<<z_max<<" "<<z_min<<" "<<y_max<<" "<<y_min<<" "<<x_max<<" "<<x_min<<endl;

		for ( double X=min ( x_min,x_max ); X<=max ( x_min,x_max ); X=X+abs ( Rx ) )
		{
			for ( double Y=min ( y_max,y_min ); Y<=max ( y_max,y_min ); Y=Y+abs ( Ry ) )
			{
				for ( double Z=min ( z_min,z_max ); Z<=max ( z_min,z_max ); Z=Z+abs ( Rz ) )
				{
					if ( !isInsideSphere ( Tri,X,Y,Z ) )
					{
						permeameter = Tri.locate ( Point ( X, Y, Z ) );
						consFile << permeameter->info().p() <<" ";
						//cout <<"valeur trouvée";
					}
					else consFile << "Nan ";
				}
				consFile << endl;
			}
			consFile << endl;
		}
		consFile.close();
	}
	/*
	Real secFastInvCos0 (double cos) {
		if (cos>0) return Wm3::Mathd::FastInvCos0 ( cos );
		else {
			//cout << "cos<0!!" << endl;
			return Wm3::Mathd::PI - Wm3::Mathd::FastInvCos0 ( -cos );
		}
	}

	Real secFastSin0 (double x) {
		if (x>0 && x<Wm3::Mathd::HALF_PI) return Wm3::Mathd::FastSin0(x);
		else {
			//cout << "x out of range!!" << endl;
			return  sin(x);
		}
	}*/

////////////////////////////
//THE END///////////////////
////////////////////////////


} //namespace CGT
