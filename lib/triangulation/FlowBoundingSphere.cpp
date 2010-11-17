/*************************************************************************
*  Copyright (C) 2010 by Emanuele Catalano <catalano@grenoble-inp.fr>    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE
#include "def_types.h"
// #include "def_flow_types.h"
#include "CGAL/constructions/constructions_on_weighted_points_cartesian_3.h"
#include <CGAL/Width_3.h>
#include <iostream>
#include <fstream>
#include <new>
#include <utility>
#include "vector"
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Network.h"
#include <omp.h>

// #define XVIEW
#include "FlowBoundingSphere.h"//include after #define XVIEW

#ifdef XVIEW
// #include "Vue3D.h" //FIXME implicit dependencies will look for this class (out of tree) even ifndef XVIEW
#endif


#define FAST
#define TESS_BASED_FORCES
#define FACET_BASED_FORCES 1

#ifdef YADE_OPEN_MP
//   #define GS_OPEN_MP //It should never be defined if Yade is not using openmp
#endif

// #define USE_FAST_MATH 1


using namespace std;
namespace CGT
{
typedef vector<double> VectorR;

//! Use this factor, or minLength, to reduce max permeability values (see usage below))
const double MAXK_DIV_KMEAN = 1;
const double minLength = 0.20;//percentage of mean rad

//! Factors including the effect of 1/2 symmetry in hydraulic radii
const Real multSym1 = 1/pow(2,0.25);
const Real multSym2 = 1/pow(4,0.25);

const int permut3 [3][3] = {{0,1,2},{1,2,0},{2,0,1}};
const int permut4 [4][4] = {{0,1,2,3},{1,2,3,0},{2,3,0,1},{3,0,1,2}};


const int facetVertices [4][3] = {{1,2,3},{0,2,3},{0,1,3},{0,1,2}};

#ifdef XVIEW
Vue3D Vue1;
#endif

FlowBoundingSphere::~FlowBoundingSphere()
{
}

FlowBoundingSphere::FlowBoundingSphere()
{
	x_min = 1000.0, x_max = -10000.0, y_min = 1000.0, y_max = -10000.0, z_min = 1000.0, z_max = -10000.0;
	currentTes = 0;
	nOfSpheres = 0;
	fictious_vertex = 0;
	SectionArea = 0, Height=0, Vtotale=0;
	vtk_infinite_vertices=0, vtk_infinite_cells=0;

	tess_based_force = true;
	for (int i=0;i<6;i++) boundsIds[i] = 0;
	minPermLength=-1;
	SLIP_ON_LATERALS = false;//no-slip/symmetry conditions on lateral boundaries
	TOLERANCE = 1e-06;
	RELAX = 1.9;
	ks=0;
	distance_correction = true;
	meanK_LIMIT = true;
	meanK_STAT = false; K_opt_factor=0;
	noCache=true;
	computeAllCells=true;//might be turned false IF the code is reorganized (we can make a separate function to compute unitForceVectors outside Compute_Permeability) AND it really matters for CPU time
	DEBUG_OUT = false;
	RAVERAGE = false; /** use the average between the effective radius (inscribed sphere in facet) and the equivalent (circle surface = facet fluid surface) **/
	OUTPUT_BOUDARIES_RADII = false;
	RAVERAGE = false; /** if true use the average between the effective radius (inscribed sphere in facet) and the equivalent (circle surface = facet fluid surface) **/
}
Tesselation& FlowBoundingSphere::Compute_Action()
{
        return Compute_Action(0,NULL,NULL);
}

Tesselation& FlowBoundingSphere::Compute_Action(int argc, char *argv[ ], char *envp[ ])
{
// 	for (K_opt_factor=0;K_opt_factor<0.061;K_opt_factor+=0.003){
// 	std::ofstream K_sigma("Permeability_Deviation", std::ios::app);
// 	cout << " r ==== " << K_opt_factor << endl;

	double factor = 1.001;
	VectorR X, Y, Z, R;
        Real_timer clock;
        clock.start();
        clock.top("start");
        Tesselation& Tes = T[0];
        RTriangulation& Tri = Tes.Triangulation();

        /** READING SPHERES POSITIONS FROM A TEXT FILE CONTAINING COORDINATES **/
        double x, y, z, r;
        ifstream loadFile(argc==1 ? "cube" : argv[1]);    // cree l'objet loadFile de la classe ifstream qui va permettre de lire importFilename
        while (!loadFile.eof()) {
                loadFile >> x >> y >> z >> r;
                X.push_back(x);
                Y.push_back(y);
                Z.push_back(z);
                R.push_back(factor*r);
                nOfSpheres++;
                Rmoy += r;
                x_min = min(x_min,x-r);
                x_max = max(x_max,x+r);
                y_min = min(y_min,y-r);
                y_max = max(y_max,y+r);
                z_min = min(z_min,z-r);
                z_max = max(z_max,z+r);
        }
        Rmoy /= nOfSpheres;
        minPermLength = Rmoy*minLength;
	if (DEBUG_OUT) cout << "Rmoy = " << Rmoy << endl;
	if (DEBUG_OUT) cout << "x_min = " << x_min << " x_max = " << x_max << " y_min = " << y_min << " y_max = " << y_max << " y_max = " << z_min << " x_min = " << z_max << endl;

        Vertex_handle Vh;
	Cell_handle neighbour_cell, cell, location;

	int V = X.size();
        if (DEBUG_OUT) cout << "V =" << V << "nOfSpheres = " << nOfSpheres << endl;
        if (DEBUG_OUT) cout << Tes.Max_id() << endl;
        clock.top("loading spheres");


	vector<Sphere> vs; RTriangulation testT;
	for (int i=0; i<V; i++) {
		vs.push_back(Sphere(Point(X[i],Y[i],Z[i]), R[i]));
        }
        clock.top("make a spheres vector");
	testT.insert(vs.begin(),vs.end());
	clock.top("test speed");

        AddBoundingPlanes();
        for (int i=0; i<V; i++) {
                int id = Tes.Max_id() +1;
                Vh = Tes.insert(X[i],Y[i],Z[i],R[i],id);    /** EMPILEMENT QUELCONQUE **/
#ifdef XVIEW
                Vue1.SetSpheresColor(0.8,0.6,0.6,1);
                Vue1.Dessine_Sphere(X[i],Y[i],Z[i], R[i], 15);
#endif
        }
        Height = y_max-y_min;
        SectionArea = (x_max-x_min) * (z_max-z_min);
	Vtotale = (x_max-x_min) * (y_max-y_min) * (z_max-z_min);
        clock.top("Triangulation");

        Tes.Compute();
        clock.top("tesselation");

        boundary(y_min_id).flowCondition=0;
        boundary(y_max_id).flowCondition=0;
        boundary(y_min_id).value=0;
        boundary(y_max_id).value=1;
	Define_fictious_cells();
        clock.top("BoundaryConditions");

        //GenerateVoxelFile(); ///*GENERATION OF A VOXEL FILE *///

        /** INITIALIZATION OF VOLUMES AND PRESSURES **/
	Real totV=0;
        Finite_cells_iterator cell_end = Tri.finite_cells_end();
        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
		cell->info().volume() = Volume_Pore(cell); totV+=cell->info().volume();
                cell->info().dv() = 0;
        }
        if(DEBUG_OUT) cerr << "TOTAL VOID VOLUME: "<<totV<<endl;

        clock.top("initializing delta_volumes");

        /** PERMEABILITY **/
        /** START PERMEABILITY CALCULATION**/
        k_factor = 1;
        Compute_Permeability();
        clock.top("Compute_Permeability");
        /** END PERMEABILITY CALCULATION**/

        /** STATISTICS **/
        DisplayStatistics();
        clock.top("DisplayStatistics");
        /** START GAUSS SEIDEL */
        //  Boundary_Conditions ( Tri );
	double P_zero = abs((boundary(y_min_id).value-boundary(y_max_id).value)/2);
        Initialize_pressures( P_zero );
	clock.top("Initialize_pressures");
        GaussSeidel();
        clock.top("GaussSeidel");

        /** END GAUSS SEIDEL */
        char* file ("Permeability");
        ks = Permeameter(boundary(y_min_id).value, boundary(y_max_id).value, SectionArea, Height, file);
// 	K_sigma << K_opt_factor << " " << ks << " "<< Iterations << endl;
        clock.top("Permeameter");

	ComputeFacetForcesWithCache();
        clock.top("Compute_Forces");

        /** VISUALISATION FILES **/
        //  save_vtk_file ( );
        //   PressureProfile ( );
        MGPost();

        ///*** VUE 3D ***///
        //Vue1.SetCouleurSegments ( 0,10,0 );
        //  Dessine_Triangulation ( Vue1, Tes.Triangulation() );
#ifdef XVIEW
        Vue1.SetCouleurSegments(0.1,0,1);
        Dessine_Short_Tesselation(Vue1, Tes);
        Vue1.Affiche();
#endif
	if (SLIP_ON_LATERALS && DEBUG_OUT) cout << "SLIP CONDITION IS ACTIVATED" << endl;
	else if (DEBUG_OUT) cout << "NOSLIP CONDITION IS ACTIVATED" << endl;
// }
  return Tes;
}


Tesselation& FlowBoundingSphere::LoadPositions(int argc, char *argv[ ], char *envp[ ])
{
	double factor = 1.001;
	VectorR X, Y, Z, R;
        Tesselation& Tes = T[0];
//         RTriangulation& Tri = Tes.Triangulation();
        /** READING SPHERES POSITIONS FROM A TEXT FILE CONTAINING COORDINATES **/
        double x, y, z, r;
        ifstream loadFile(argc==1 ? "cube" : argv[1]);    // cree l'objet loadFile de la classe ifstream qui va permettre de lire importFilename
        while (!loadFile.eof()) {
                loadFile >> x >> y >> z >> r;
                X.push_back(x);
                Y.push_back(y);
                Z.push_back(z);
                R.push_back(factor*r);
                nOfSpheres++;
                Rmoy += r;
                x_min = min(x_min,x-r);
                x_max = max(x_max,x+r);
                y_min = min(y_min,y-r);
                y_max = max(y_max,y+r);
                z_min = min(z_min,z-r);
                z_max = max(z_max,z+r);
        }
        Rmoy /= nOfSpheres;
        minPermLength = Rmoy*minLength;
        Vertex_handle Vh;
	Cell_handle neighbour_cell, cell, location;

	int V = X.size();
        if (DEBUG_OUT) cout << "V =" << V << "nOfSpheres = " << nOfSpheres << endl;
        if (DEBUG_OUT) cout << Tes.Max_id() << endl;

// 	vector<Sphere> vs; RTriangulation testT;
// 	for (int i=0; i<V; i++) {
// 		vs.push_back(Sphere(Point(X[i],Y[i],Z[i]), R[i]));
//         }
//         clock.top("make a spheres vector");
// 	testT.insert(vs.begin(),vs.end());
// 	clock.top("test speed");

        AddBoundingPlanes();
        for (int i=0; i<V; i++) {
                int id = Tes.Max_id() +1;
                Vh = Tes.insert(X[i],Y[i],Z[i],R[i],id);    /** EMPILEMENT QUELCONQUE **/
#ifdef XVIEW
                Vue1.SetSpheresColor(0.8,0.6,0.6,1);
                Vue1.Dessine_Sphere(X[i],Y[i],Z[i], R[i], 15);
#endif
        }
        Height = y_max-y_min;
        SectionArea = (x_max-x_min) * (z_max-z_min);
	Vtotale = (x_max-x_min) * (y_max-y_min) * (z_max-z_min);

        Tes.Compute();
 	Define_fictious_cells();

  return Tes;
}

void FlowBoundingSphere::Average_Cell_Velocity()
{
        RTriangulation& Tri = T[currentTes].Triangulation();
        Point pos_av_facet;
        int num_cells = 0;
        double facet_flow_rate;
        std::ofstream oFile ( "Average_Cells_Velocity",std::ios::app );
	Real tVel=0; Real tVol=0;
        Finite_cells_iterator cell_end = Tri.finite_cells_end();
        for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ ) {
		cell->info().av_vel() =CGAL::NULL_VECTOR;
                num_cells++;
                for ( int i=0; i<4; i++ ) {
                        Vecteur Surfk = cell->info()-cell->neighbor(i)->info();
                        Real area = sqrt ( Surfk.squared_length() );
			Surfk = Surfk/area;
//                         Vecteur facetNormal = Surfk/area;
                        Vecteur branch = cell->vertex ( facetVertices[i][0] )->point() - cell->info();
// 			 pos_av_facet=CGAL::ORIGIN + ((cell->vertex(facetVertices[i][0])->point() - CGAL::ORIGIN) +
// 	    			(cell->vertex(facetVertices[i][1])->point() - CGAL::ORIGIN) +
// 	    			(cell->vertex(facetVertices[i][2])->point() - CGAL::ORIGIN))*0.3333333333;
                        pos_av_facet = (Point) cell->info() + ( branch*Surfk ) *Surfk;
                        facet_flow_rate = (cell->info().k_norm())[i] * (cell->neighbor (i)->info().p() - cell->info().p());
                        cell->info().av_vel() = cell->info().av_vel() + facet_flow_rate* ( pos_av_facet-CGAL::ORIGIN );
 		}
 		if (cell->info().volume()){ tVel+=cell->info().av_vel()[1]; tVol+=cell->info().volume();}
                cell->info().av_vel() = cell->info().av_vel() /cell->info().volume();
// 		cerr << cell->info().av_vel()<<" "<<facet_flow_rate<<" "<<cell->info().volume()<<endl;
//                 oFile << cell->info().av_vel() << "Cell Pressure = " << cell->info().p() << endl;
        }
//         cerr <<"TOT Vol/Vel: "<<tVol<<" "<<tVel<<endl;
}

void FlowBoundingSphere::Average_Grain_Velocity()
{
	RTriangulation& Tri = T[currentTes].Triangulation();

	Finite_vertices_iterator vertices_end = Tri.finite_vertices_end();
        for (Finite_vertices_iterator V_it = Tri.finite_vertices_begin(); V_it !=  vertices_end; V_it++) {
	  V_it->info().vel() = CGAL::NULL_VECTOR;
	  V_it->info().vol_cells() = 0;}

        Point pos_av_facet;
        double facet_flow_rate;
	Finite_cells_iterator cell_end = Tri.finite_cells_end();
        for ( Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++ )
	{
	  int pass=0;
	  for ( int i=0; i<4; i++ )
	  {
	    if(!Tri.is_infinite(cell->neighbor(i))){
	    pass+=1;
	    Vecteur Surfk = cell->info()-cell->neighbor(i)->info();
            Real area = sqrt ( Surfk.squared_length() );
	    Surfk = Surfk/area;
	    Vecteur branch = cell->vertex ( facetVertices[i][0] )->point() - cell->info();
	    pos_av_facet = (Point) cell->info() + ( branch*Surfk ) *Surfk;
	    facet_flow_rate = (cell->info().k_norm())[i] * (cell->info().p() - cell->neighbor (i)->info().p() );

	    for (int g=0;g<4;g++)
	    {cell->vertex ( g )->info().vel() = cell->vertex ( g )->info().vel() + facet_flow_rate*( pos_av_facet-CGAL::ORIGIN );}
	    }
// 	    else cout << "CI SONO INFINITE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
	   }
	    for (int g=0;g<pass;g++)
	    {cell->vertex ( g )->info().vol_cells() = cell->vertex ( g )->info().vol_cells() + cell->info().volume();}
	  }

// 	Finite_vertices_iterator vertices_end = Tri.finite_vertices_end();
        for (Finite_vertices_iterator V_it = Tri.finite_vertices_begin(); V_it !=  vertices_end; V_it++) {
	  V_it->info().vel() = V_it->info().vel() / V_it->info().vol_cells();}

// 	for (Finite_vertices_iterator V_it = Tri.finite_vertices_begin(); V_it !=  vertices_end; V_it++) {
// 	  V_it->info().vel() = V_it->info().vel() / sqrt ( V_it->info().vel().squared_length() );}
}

void FlowBoundingSphere::vtk_average_cell_velocity(RTriangulation &Tri, int id_sphere, int num_cells )
{
        static unsigned int number = 0;
        char filename[80];
	mkdir("./VTK", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        sprintf(filename,"./VTK/out_%d.vtk", number++);

        basicVTKwritter vtkfile((unsigned int) 4*num_cells, (unsigned int) num_cells);

        vtkfile.open(filename,"test");

	Tesselation::Vector_Cell tmp_cells;
	tmp_cells.resize(10000);
	Tesselation::VCell_iterator cells_it = tmp_cells.begin();
	Tesselation::VCell_iterator cells_end = Tri.incident_cells(T[currentTes].vertexHandles[id_sphere],cells_it);

        vtkfile.begin_vertices();
	for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
	{
	  for (int y2=0; y2<4; y2++){
	    double x,y,z;
	    x = (double)((*it)->vertex(y2)->point().point()[0]);
	    y = (double)((*it)->vertex(y2)->point().point()[1]);
	    z = (double)((*it)->vertex(y2)->point().point()[2]);
	    vtkfile.write_point(x,y,z);}
	}

        vtkfile.end_vertices();

        vtkfile.begin_cells();
	for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
	{
		vtkfile.write_cell((*it)->vertex(0)->info().id()-id_offset, (*it)->vertex(1)->info().id()-id_offset, (*it)->vertex(2)->info().id()-id_offset, (*it)->vertex(3)->info().id()-id_offset);
        }
        vtkfile.end_cells();

// 	vtkfile.begin_data("Force",POINT_DATA,SCALARS,FLOAT);
// 	vtkfile.write_data((T[currentTes].vertexHandles[id_sphere]->info().forces)[1]);
// 	vtkfile.end_data();

	vtkfile.begin_data("Velocity",CELL_DATA,SCALARS,FLOAT);

	for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
	{
		vtkfile.write_data((*it)->info().av_vel()[1]);
	}
	vtkfile.end_data();
}

void FlowBoundingSphere::ComputeFacetForces()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
	Finite_cells_iterator cell_end = Tri.finite_cells_end();
	Vecteur nullVect(0,0,0);
	bool ref = Tri.finite_cells_begin()->info().isvisited;
	//reset forces
	for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
		v->info().forces=nullVect;
	}
	Cell_handle neighbour_cell;
	Vertex_handle mirror_vertex;
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
		for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j)) && cell->neighbor(j)->info().isvisited==ref) {
				neighbour_cell = cell->neighbor(j);
				const Vecteur& Surfk = cell->info().facetSurfaces[j];
				//FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
				//The ratio void surface / facet surface
				Real area = sqrt(Surfk.squared_length());
				Vecteur facetNormal = Surfk/area;
				const std::vector<Vecteur>& crossSections = cell->info().facetSphereCrossSections;
				Real fluidSurfRatio = (area-crossSections[j][0]-crossSections[j][1]-crossSections[j][2])/area;
				Vecteur fluidSurfk = cell->info().facetSurfaces[j]*fluidSurfRatio;
				/// handle fictious vertex since we can get the projected surface easily here
				if (cell->vertex(j)->info().isFictious) {
					Real projSurf=abs(Surfk[boundary(cell->vertex(j)->info().id()).coordinate]);
					cell->vertex(j)->info().forces = cell->vertex(j)->info().forces -projSurf*boundary(cell->vertex(j)->info().id()).normal*cell->info().p();
				}
				/// handle the opposite fictious vertex (remember each facet is seen only once)
				mirror_vertex = neighbour_cell->vertex(Tri.mirror_index(cell,j));
				Vertex_Info& info = neighbour_cell->vertex(Tri.mirror_index(cell,j))->info();
				if (info.isFictious) {
					Real projSurf=abs(Surfk[boundary(info.id()).coordinate]);
					info.forces = info.forces - projSurf*boundary(info.id()).normal*neighbour_cell->info().p();
				}
				/// Apply weighted forces f_k=sqRad_k/sumSqRad*f
				Vecteur Facet_Force = (neighbour_cell->info().p()-cell->info().p())*fluidSurfk*cell->info().solidSurfaces[j][3];
				for (int y=0; y<3;y++) {
					cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces + Facet_Force*cell->info().solidSurfaces[j][y]/* + (cell->vertex(facetVertices[j][y])->info().isFictious ? 0 : facetNormal*(neighbour_cell->info().p()-cell->info().p())*crossSections[j][y])*/;
					if (!cell->vertex(facetVertices[j][y])->info().isFictious) {
						cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces +  facetNormal*(neighbour_cell->info().p()-cell->info().p())*crossSections[j][y];
					}
					///TEST BEGING
					//Conclusion, we can't easily get ordered vertices from mirror facet...
// 				for (int y=0; y<3;y++) {
// 					int id1 = cell->vertex(facetVertices[j][y])->info().id();
// 					int id2 = neighbour_cell->vertex(facetVertices[Tri.mirror_index(cell,j)][y])->info().id();
// 					cerr <<"id1/id2 : "<<id1<<" "<<id2<<endl;}

					///TEST END
				}
			}
		cell->info().isvisited=!ref;
	}
// 	if (DEBUG_OUT) {
// 		cout << "Facet scheme" <<endl;
// 		Vecteur TotalForce = nullVect;
// 		for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
// 			if (!v->info().isFictious) {
// 				TotalForce = TotalForce + v->info().forces;
// 				cout << "real_id = " << v->info().id() << " force = " << v->info().forces << endl;
// 			} else {
// 				if (boundary(v->info().id()).flowCondition==1) TotalForce = TotalForce + v->info().forces;
// 				cout << "fictious_id = " << v->info().id() << " force = " << v->info().forces << endl;
// 			}
// 		}
// 		cout << "TotalForce = "<< TotalForce << endl;
// 	}
}

void FlowBoundingSphere::ComputeFacetForcesWithCache()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
	Finite_cells_iterator cell_end = Tri.finite_cells_end();
	Vecteur nullVect(0,0,0);
	//reset forces
	for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) v->info().forces=nullVect;

	Cell_handle neighbour_cell;
	Vertex_handle mirror_vertex;
	Vecteur tempVect;
	//FIXME : Ema, be carefull with this (noCache), it needs to be turned true after retriangulation
	if (noCache) for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			//reset cache
			for (int k=0;k<4;k++) cell->info().unitForceVectors[k]=nullVect;
			for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j))) {
					neighbour_cell = cell->neighbor(j);
					const Vecteur& Surfk = cell->info().facetSurfaces[j];
					//FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
					//The ratio void surface / facet surface
					Real area = sqrt(Surfk.squared_length());
					Vecteur facetNormal = Surfk/area;
					const std::vector<Vecteur>& crossSections = cell->info().facetSphereCrossSections;
					Real fluidSurfRatio = (area-crossSections[j][0]-crossSections[j][1]-crossSections[j][2])/area;
					Vecteur fluidSurfk = cell->info().facetSurfaces[j]*fluidSurfRatio;
					/// handle fictious vertex since we can get the projected surface easily here
					if (cell->vertex(j)->info().isFictious) {
						Real projSurf=abs(Surfk[boundary(cell->vertex(j)->info().id()).coordinate]);
						tempVect=-projSurf*boundary(cell->vertex(j)->info().id()).normal;
						cell->vertex(j)->info().forces = cell->vertex(j)->info().forces+tempVect*cell->info().p();
						//define the cached value for later use with cache*p
						cell->info().unitForceVectors[j]=cell->info().unitForceVectors[j]+ tempVect;
					}
					/// Apply weighted forces f_k=sqRad_k/sumSqRad*f
					Vecteur Facet_Unit_Force = -fluidSurfk*cell->info().solidSurfaces[j][3];
					Vecteur Facet_Force = cell->info().p()*Facet_Unit_Force;
					for (int y=0; y<3;y++) {
						cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces + Facet_Force*cell->info().solidSurfaces[j][y];
						//add to cached value
						cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]+Facet_Unit_Force*cell->info().solidSurfaces[j][y];
						//uncomment to get total force / comment to get only viscous forces (Bruno)
						if (!cell->vertex(facetVertices[j][y])->info().isFictious) {
							cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces -facetNormal*cell->info().p()*crossSections[j][y];
							//add to cached value
							cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]-facetNormal*crossSections[j][y];
						}
					}

				}
		}
	else //use cached values
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++)
			for (int yy=0;yy<4;yy++) cell->vertex(yy)->info().forces = cell->vertex(yy)->info().forces + cell->info().unitForceVectors[yy]*cell->info().p();
	noCache=false;//cache should always be defined after execution of this function
// 	if (DEBUG_OUT) {
// 		cout << "Facet cached scheme" <<endl;
// 		Vecteur TotalForce = nullVect;
// 		for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v)
// 		{
// 			if (!v->info().isFictious) {
// 				TotalForce = TotalForce + v->info().forces;
// 				if (DEBUG_OUT) cout << "real_id = " << v->info().id() << " force = " << v->info().forces << endl;
// 			} else {
// 				if (boundary(v->info().id()).flowCondition==1) TotalForce = TotalForce + v->info().forces;
// 				if (DEBUG_OUT) cout << "fictious_id = " << v->info().id() << " force = " << v->info().forces << endl;
// 			}
// 		}
// 		cout << "TotalForce = "<< TotalForce << endl;
// 	}
}

void FlowBoundingSphere::ComputeTetrahedralForces()
{
        RTriangulation& Tri = T[currentTes].Triangulation();
        Finite_cells_iterator cell_end = Tri.finite_cells_end();
        Vecteur nullVect(0,0,0);
        bool ref = Tri.finite_cells_begin()->info().isvisited;

	for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
                v->info().forces=nullVect;
        }

        Cell_handle neighbour_cell;
        Vertex_handle mirror_vertex;
        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
                for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j)) && cell->neighbor(j)->info().isvisited==ref) {
                                neighbour_cell = cell->neighbor(j);
                                const Vecteur& Surfk = cell->info().facetSurfaces[j];
                                /// handle fictious vertex since we can get the projected surface easily here
                                if (cell->vertex(j)->info().isFictious) {
                                        Real projSurf=abs(Surfk[boundary(cell->vertex(j)->info().id()).coordinate]);
                                        cell->vertex(j)->info().forces = cell->vertex(j)->info().forces -projSurf*boundary(cell->vertex(j)->info().id()).normal*cell->info().p();
                                }
                                /// handle the opposite fictious vertex (remember each facet is seen only once)
                                mirror_vertex = neighbour_cell->vertex(Tri.mirror_index(cell,j));
                                Vertex_Info& info = neighbour_cell->vertex(Tri.mirror_index(cell,j))->info();
                                if (info.isFictious) {
                                        Real projSurf=abs(Surfk[boundary(info.id()).coordinate]);
                                        info.forces = info.forces - projSurf*boundary(info.id()).normal*neighbour_cell->info().p();
                                }
                                /// Apply weighted forces f_k=sqRad_k/sumSqRad*f
                                Vecteur Facet_Force = (neighbour_cell->info().p()-cell->info().p())*Surfk*cell->info().solidSurfaces[j][3];
                                for (int y=0; y<3;y++) {
                                        cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces + Facet_Force*cell->info().solidSurfaces[j][y];
                                }
                        }
                cell->info().isvisited=!ref;
        }
	if (DEBUG_OUT) cout << "tetrahedral scheme" <<endl;
	Vecteur TotalForce = nullVect;
	for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
		if (!v->info().isFictious) {
			TotalForce = TotalForce + v->info().forces;
		} else {
			if (boundary(v->info().id()).flowCondition==1) TotalForce = TotalForce + v->info().forces;
			if (DEBUG_OUT) cout << "fictious_id = " << v->info().id() << " force = " << v->info().forces << endl;
		}
	}
	cout << "TotalForce = "<< TotalForce << endl;
}

void FlowBoundingSphere::ApplySinusoidalPressure(RTriangulation& Tri, double Pressure, double load_intervals)
{

	double step = 1/load_intervals;
	Tesselation::Vector_Cell tmp_cells;
	tmp_cells.resize(10000);
	Tesselation::VCell_iterator cells_it = tmp_cells.begin();
	for (double alpha=0; alpha<1.001; alpha+=step)
	{
	  Tesselation::VCell_iterator cells_end = Tri.incident_cells(T[currentTes].vertexHandles[y_max_id],cells_it);
	  for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
	  {
	    if(!Tri.is_infinite(*it)){
	      Point& p1 = (*it)->info();
	      Cell_handle& cell = *it;
	      if (p1.x()>(alpha*(x_max-x_min)) && p1.x()<((alpha+step)*(x_max-x_min))){cell->info().p() = (Pressure/2)*(1+cos(alpha*M_PI));}
	  }
	  }
	}
}

void FlowBoundingSphere::ApplySinusoidalPressure_Space_Time(RTriangulation& Tri, double Pressure, double load_intervals, double time, double dt)
{
	//FIXME : rivedere!!

	double step = 1/load_intervals;
	Tesselation::Vector_Cell tmp_cells;
	tmp_cells.resize(1000);
	Tesselation::VCell_iterator cells_it = tmp_cells.begin();
	for (double alpha=0; alpha<1.001; alpha+=step)
	{
	  Tesselation::VCell_iterator cells_end = Tri.incident_cells(T[currentTes].vertexHandles[y_max_id],cells_it);
	  for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
	  {
	    if(!Tri.is_infinite(*it)){
	      Point& p1 = (*it)->info();
	      Cell_handle& cell = *it;
	      if (p1.x()>(alpha*(x_max-x_min)) && p1.x()<((alpha+step)*(x_max-x_min)))
	      {
		if (alpha<0.5) cell->info().p() = (Pressure/2)*(1+cos(alpha*M_PI)-(1-cos(time/(20*dt)))*M_PI);
		if (alpha>0.5) cell->info().p() = (Pressure/2)*(1+cos(alpha*M_PI)+(1-cos(time/(20*dt)))*M_PI);
	      }
	  }
	  }
	}
}

void FlowBoundingSphere::Interpolate(Tesselation& Tes, Tesselation& NewTes)
{
        Cell_handle old_cell;

        RTriangulation& NewTri = NewTes.Triangulation();
        RTriangulation& Tri = Tes.Triangulation();
        Finite_cells_iterator cell_end = NewTri.finite_cells_end();
        /*CALCULATION OF VORONOI CENTRES*/
//        if ( !NewTes.Computed() ) NewTes.Compute();
        for (Finite_cells_iterator new_cell = NewTri.finite_cells_begin(); new_cell != cell_end; new_cell++) {
                old_cell = Tri.locate((Point) new_cell->info());
                new_cell->info().p() = old_cell->info().p();
        }
	Tes.Clear();
}

void FlowBoundingSphere::Compute_Permeability()
{
	if (DEBUG_OUT)  cout << "----Computing_Permeability------" << endl;
	RTriangulation& Tri = T[currentTes].Triangulation();
	Vsolid_tot = 0, Vtotalissimo = 0, Vporale = 0, Ssolid_tot = 0;
	Finite_cells_iterator cell_end = Tri.finite_cells_end();

	Cell_handle neighbour_cell;

	double k=0, distance = 0, radius = 0, viscosity = 1;
	int NEG=0, POS=0, pass=0;

	bool ref = Tri.finite_cells_begin()->info().isvisited;

	Vecteur n;
//         std::ofstream oFile( "Radii",std::ios::out);
// 	std::ofstream fFile( "Radii_Fictious",std::ios::out);
//         std::ofstream kFile ( "LocalPermeabilities" ,std::ios::app );
	Real meanK=0, STDEV=0;
	Real infiniteK=1e10;

	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
		Point& p1 = cell->info();
		for (int j=0; j<4; j++) {
			neighbour_cell = cell->neighbor(j);
			Point& p2 = neighbour_cell->info();
			if (!Tri.is_infinite(neighbour_cell) && (neighbour_cell->info().isvisited==ref || computeAllCells)) {
				//Compute and store the area of sphere-facet intersections for later use
				Vertex_handle W [3];
				for (int kk=0; kk<3; kk++) {
					W[kk] = cell->vertex(facetVertices[j][kk]);
				}
				Sphere& v0 = W[0]->point();
				Sphere& v1 = W[1]->point();
				Sphere& v2 = W[2]->point();
#ifdef USE_FAST_MATH
				//FIXME : code not compiling,, do the same as in "else"
				assert((W[permut3[jj][1]]->point()-W[permut3[jj][0]]->point())*(W[permut3[jj][2]]->point()-W[permut3[jj][0]]->point())>=0 && (W[permut3[jj][1]]->point()-W[permut3[jj][0]]->point())*(W[permut3[jj][2]]->point()-W[permut3[jj][0]]->point())<=1);
				for (int jj=0;jj<3;jj++)
					cell->info().facetSphereCrossSections[j][jj]=0.5*W[jj]->point().weight()*Wm3::FastInvCos1((W[permut3[jj][1]]->point()-W[permut3[jj][0]]->point())*(W[permut3[jj][2]]->point()-W[permut3[jj][0]]->point()));
#else
				cell->info().facetSphereCrossSections[j]=Vecteur(
				   W[0]->info().isFictious ? 0 : 0.5*v0.weight()*acos((v1-v0)*(v2-v0)/sqrt((v1-v0).squared_length()*(v2-v0).squared_length())),
				   W[1]->info().isFictious ? 0 : 0.5*v1.weight()*acos((v0-v1)*(v2-v1)/sqrt((v1-v0).squared_length()*(v2-v1).squared_length())),
				   W[2]->info().isFictious ? 0 : 0.5*v2.weight()*acos((v0-v2)*(v1-v2)/sqrt((v1-v2).squared_length()*(v2-v0).squared_length())));
#endif
				pass+=1;
				Vecteur l = p1 - p2;
				distance = sqrt(l.squared_length());
				n = l/distance;
				if (!RAVERAGE) radius = 2* Compute_HydraulicRadius(cell, j);
				else radius = (Compute_EffectiveRadius(cell, j)+Compute_EquivalentRadius(cell,j))*0.5;
				if (radius<0) NEG++;
				else POS++;
				if (radius==0) {
					cout << "INS-INS PROBLEM!!!!!!!" << endl;
				}

				if (distance!=0) {
					if (minPermLength>0 && distance_correction) distance=max(minPermLength,distance);
					k = (M_PI * pow(radius,4)) / (8*viscosity*distance);
				} else  k = infiniteK;//Will be corrected in the next loop

				(cell->info().k_norm())[j]= k*k_factor;
//     (cell->info().facetSurf())[j]= k*n;
				(neighbour_cell->info().k_norm())[Tri.mirror_index(cell, j)]= k*k_factor;

				meanK += (cell->info().k_norm())[j];

// 				if (!meanK_LIMIT) kFile << ( cell->info().k_norm() )[j] << endl;
//     (neighbour_cell->info().facetSurf())[Tri.mirror_index(cell, j)]= (-k) *n;
			}
			//    else if ( Tri.is_infinite ( neighbour_cell )) k = 0;//connection to an infinite cells
		}
		cell->info().isvisited = !ref;
	}
	meanK /= pass;
	double maxKdivKmean=MAXK_DIV_KMEAN;
	if (DEBUG_OUT) {
		cout << "PassCompK = " << pass << endl;
		cout << "meanK = " << meanK << endl;
		cout << "maxKdivKmean = " << maxKdivKmean << endl;
	}
	ref = Tri.finite_cells_begin()->info().isvisited;
	pass=0;
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
		for (int j=0; j<4; j++) {
			neighbour_cell = cell->neighbor(j);
			if (!Tri.is_infinite(neighbour_cell) && neighbour_cell->info().isvisited==ref) {
				pass++;
				(cell->info().k_norm())[j] = min((cell->info().k_norm())[j], maxKdivKmean*meanK);
				(neighbour_cell->info().k_norm())[Tri.mirror_index(cell, j)]=(cell->info().k_norm())[j];
// 				kFile << (cell->info().k_norm())[j] << endl;
			}
		}cell->info().isvisited = !ref;

	}
	if (DEBUG_OUT) cout << "PassKcorrect = " << pass << endl;

	if (DEBUG_OUT) cout << "POS = " << POS << " NEG = " << NEG << " pass = " << pass << endl;

// A loop to compute the standard deviation of the local K distribution, and use it to include/exclude K values higher then (meanK +/- K_opt_factor*STDEV)
	if (meanK_STAT)
	{
		std::ofstream k_opt_file("k_stdev.txt" ,std::ios::out);
		ref = Tri.finite_cells_begin()->info().isvisited;
		pass=0;
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			for (int j=0; j<4; j++) {
				neighbour_cell = cell->neighbor(j);
				if (!Tri.is_infinite(neighbour_cell) && neighbour_cell->info().isvisited==ref) {
					pass++;
					STDEV += pow(((cell->info().k_norm())[j]-meanK),2);
				}
			}cell->info().isvisited = !ref;
		}
		STDEV = sqrt(STDEV/pass);
		if (DEBUG_OUT) cout << "PassSTDEV = " << pass << endl;
		cout << "STATISTIC K" << endl;
		double k_min = 0, k_max = meanK + K_opt_factor*STDEV;
		cout << "Kmoy = " << meanK << " Standard Deviation = " << STDEV << endl;
		cout << "kmin = " << k_min << " kmax = " << k_max << endl;
		ref = Tri.finite_cells_begin()->info().isvisited;
		pass=0;
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			for (int j=0; j<4; j++) {
				neighbour_cell = cell->neighbor(j);
				if (!Tri.is_infinite(neighbour_cell) && neighbour_cell->info().isvisited==ref) {
					pass+=1;
					if ((cell->info().k_norm())[j]>k_max) {
						(cell->info().k_norm())[j]=k_max;
						(neighbour_cell->info().k_norm())[Tri.mirror_index(cell, j)]= (cell->info().k_norm())[j];
					}
					k_opt_file << K_opt_factor << " " << (cell->info().k_norm())[j] << endl;
				}
			}cell->info().isvisited=!ref;
		}
		if (DEBUG_OUT) cout << "PassKopt = " << pass << endl;
	}

	Finite_vertices_iterator vertices_end = Tri.finite_vertices_end();
	Real Vgrains = 0;
	int grains=0;

	for (Finite_vertices_iterator V_it = Tri.finite_vertices_begin(); V_it !=  vertices_end; V_it++) {
		if (!V_it->info().isFictious) {
			grains +=1;
			Vgrains += 1.33333333 * M_PI * pow(V_it->point().weight(),1.5);
		}
	}
	if (DEBUG_OUT) {
		cout<<grains<<"grains - " <<"Vtotale = " << 2*Vtotale << " Vgrains = " << 2*Vgrains << " Vporale1 = " << 2*(Vtotale-Vgrains) << endl;
		cout << "Vtotalissimo = " << Vtotalissimo << " Vsolid_tot = " << Vsolid_tot << " Vporale2 = " << Vporale  << " Ssolid_tot = " << Ssolid_tot << endl<< endl;

		if (!RAVERAGE) cout << "------Hydraulic Radius is used for permeability computation------" << endl << endl;
		else cout << "------Average Radius is used for permeability computation------" << endl << endl;
		cout << "-----Computed_Permeability-----" << endl;}
}

double FlowBoundingSphere::Compute_EffectiveRadius(Cell_handle cell, int j)
{
	RTriangulation& Tri = T[currentTes].Triangulation();
        if (Tri.is_infinite(cell->neighbor(j))) return 0;

	Vecteur B = cell->vertex(facetVertices[j][1])->point().point()-cell->vertex(facetVertices[j][0])->point().point();
	Vecteur x = B/sqrt(B.squared_length());
	Vecteur C = cell->vertex(facetVertices[j][2])->point().point()-cell->vertex(facetVertices[j][0])->point().point();
	Vecteur z = CGAL::cross_product(x,C);
	z = z/sqrt(z.squared_length());
	Vecteur y = CGAL::cross_product(x,z);
	y = y/sqrt(y.squared_length());

	double b1[2]; b1[0] = B*x; b1[1] = B*y;
	double c1[2]; c1[0] = C*x; c1[1] = C*y;

	double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
	double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
	double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());

	double A = ((pow(rA,2))*(1-c1[0]/b1[0])+((pow(rB,2)*c1[0])/b1[0])-pow(rC,2)+pow(c1[0],2)+pow(c1[1],2)-((pow(b1[0],2)+pow(b1[1],2))*c1[0]/b1[0]))/(2*c1[1]-2*b1[1]*c1[0]/b1[0]);
	double BB = (rA-rC-((rA-rB)*c1[0]/b1[0]))/(c1[1]-b1[1]*c1[0]/b1[0]);
	double CC = (pow(rA,2)-pow(rB,2)+pow(b1[0],2)+pow(b1[1],2))/(2*b1[0]);
	double D = (rA-rB)/b1[0];
	double E = b1[1]/b1[0];
	double F = pow(CC,2)+pow(E,2)*pow(A,2)-2*CC*E*A;

	double c = -F-pow(A,2)+pow(rA,2);
	double b = 2*rA-2*(D-BB*E)*(CC-E*A)-2*A*BB;
	double a = 1-pow((D-BB*E),2)-pow(BB,2);

	if ((pow(b,2)-4*a*c)<0){cout << "NEGATIVE DETERMINANT" << endl; }
	double reff = (-b+sqrt(pow(b,2)-4*a*c))/(2*a);
	if (reff<0 || reff==0) {cout << "reff1 = " << reff << endl; reff = (-b-sqrt(pow(b,2)-4*a*c))/(2*a); cout << endl << "reff2 = " << reff << endl;return reff;} else
	return reff;
}

double FlowBoundingSphere::Compute_EquivalentRadius(Cell_handle cell, int j)
{
	const Vecteur& Surfk = cell->info().facetSurfaces[j];
	//FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
	//The ratio void surface / facet surface
	Real area = sqrt(Surfk.squared_length());
	Vecteur facetNormal = Surfk/area;
	const std::vector<Vecteur>& crossSections = cell->info().facetSphereCrossSections;
	Real fluidSurf = (area-crossSections[j][0]-crossSections[j][1]-crossSections[j][2]);
	double r_equiv = sqrt(fluidSurf/M_PI);
	return r_equiv;
}

double FlowBoundingSphere::Compute_HydraulicRadius(Cell_handle cell, int j)
{
//   cerr << "test11" << endl;
	RTriangulation& Tri = T[currentTes].Triangulation();
        if (Tri.is_infinite(cell->neighbor(j))) return 0;

        double Vpore = Volume_Pore_VoronoiFraction(cell, j);
	double Ssolid = Surface_Solid_Pore(cell, j, SLIP_ON_LATERALS);

	#ifdef FACET_BASED_FORCES
		//FIXME : cannot be done here because we are still comparing the different methods
	// 			if (FACET_BASED_FORCES) cell->info().facetSurfaces[j]=cell->info().facetSurfaces[j]*((facetSurfaces[j].length()-facetSphereCrossSections[j][0]-facetSphereCrossSections[j][1]-facetSphereCrossSections[j][2])/facetSurfaces[j].length());
	#endif
	//         if (DEBUG_OUT) std::cerr << "total facet surface "<< cell->info().facetSurfaces[j] << " with solid sectors : " << cell->info().facetSphereCrossSections[j][0] << " " << cell->info().facetSphereCrossSections[j][1] << " " << cell->info().facetSphereCrossSections[j][2] << " difference "<<sqrt(cell->info().facetSurfaces[j].squared_length())-cell->info().facetSphereCrossSections[j][0]-cell->info().facetSphereCrossSections[j][2]-cell->info().facetSphereCrossSections[j][1]<<endl;
// cerr << "test14" << endl;
	//handle symmetry (tested ok)
	if (/*SLIP_ON_LATERALS &&*/ fictious_vertex>0)
	{
		//! Include a multiplier so that permeability will be K/2 or K/4 in symmetry conditions
		Real mult= fictious_vertex==1 ? multSym1 : multSym2;
		return Vpore/Ssolid*mult;
	}
// 	cerr << "test15" << endl;
	return Vpore/Ssolid;
}

void FlowBoundingSphere::Initialize_pressures( double P_zero )
{
        RTriangulation& Tri = T[currentTes].Triangulation();
        Finite_cells_iterator cell_end = Tri.finite_cells_end();

        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++){
		cell->info().p() = P_zero;cell->info().dv()=0;}

        for (int bound=0; bound<6;bound++) {
                int& id = *boundsIds[bound];
                Boundary& bi = boundary(id);
                if (!bi.flowCondition) {
                        Tesselation::Vector_Cell tmp_cells;
                        tmp_cells.resize(10000);
                        Tesselation::VCell_iterator cells_it = tmp_cells.begin();
                        Tesselation::VCell_iterator cells_end = Tri.incident_cells(T[currentTes].vertexHandles[id],cells_it);
                        for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cells_end; it++)
			{(*it)->info().p() = bi.value;(*it)->info().Pcondition=true;}
                }
        }
}

void FlowBoundingSphere::GaussSeidel()
{

// 	std::ofstream iter("Gauss_Iterations", std::ios::app);
// 	std::ofstream p_av("P_moyenne", std::ios::app);

	RTriangulation& Tri = T[currentTes].Triangulation();
	int j = 0;
	double m, n, dp_max, p_max, sum_p, p_moy, dp_moy, dp, sum_dp;
	double tolerance = TOLERANCE;
	double relax = RELAX;
	const int num_threads=1;
#ifdef GS_OPEN_MP
	omp_set_num_threads(num_threads);
#endif


       if(DEBUG_OUT){ cout << "tolerance = " << tolerance << endl;
        cout << "relax = " << relax << endl;}
			vector<Real> t_sum_p, t_dp_max, t_sum_dp, t_p_max;
			t_sum_dp.resize(num_threads);
			t_dp_max.resize(num_threads);
			t_p_max.resize(num_threads);
			t_sum_p.resize(num_threads);
// 			cerr <<"====    THREADS : "<<num_threads<<"    ===="<<endl;


//         cout << "tolerance = " << tolerance << endl;
//         cout << "relax = " << relax << endl;

        Finite_cells_iterator cell_end = Tri.finite_cells_end();
	#ifdef GS_OPEN_MP
		vector<Finite_cells_iterator> cells_its;
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) if ( !cell->info().Pcondition ) cells_its.push_back(cell);
		int num_cells=cells_its.size();
		cout<<"cells_its.size() "<<cells_its.size();
	#endif
// 	#pragma omp parallel shared(t_sum_dp, t_dp_max, sum_p, sum_dp,cells_its, j, Tri, relax)
	{
        do {
                int cell2=0; dp_max = 0;p_max = 0;p_moy=0;dp_moy=0;sum_p=0;sum_dp=0;
		#ifdef GS_OPEN_MP
		cell2=num_cells;
		for (int ii=0;ii<num_threads;ii++) t_p_max[ii] =0;
		for (int ii=0;ii<num_threads;ii++) t_dp_max[ii] =0;
		for (int ii=0;ii<num_threads;ii++) t_sum_p[ii]=0;
                for (int ii=0;ii<num_threads;ii++) t_sum_dp[ii]=0;
		int kk=0;
		const int num_cells2 = num_cells;
		#pragma omp parallel for private(dp, m, n, kk) shared(tolerance, t_sum_dp, t_dp_max, sum_p, sum_dp,cells_its, j, Tri, relax) schedule(dynamic, 1000)
		for (kk=0; kk<num_cells2; kk++) {
			const Finite_cells_iterator& cell = cells_its[kk];
			{
		#else
                for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
			 if ( !cell->info().Pcondition ) {
		                cell2++;
		#endif
				 m=0, n=0;
                                for (int j2=0; j2<4; j2++) {
					if (!Tri.is_infinite(cell->neighbor(j2))) {
                                                m += (cell->info().k_norm())[j2] * cell->neighbor(j2)->info().p();
                                                if (j==0) n += (cell->info().k_norm())[j2];}
                                }
                                dp = cell->info().p();
                                if (n!=0 || j!=0) {
                                        //     cell->info().p() =   - ( cell->info().dv() - m ) / ( n );
					if (j==0) cell->info().inv_sum_k=1/n;
// 					cell->info().p() = (- (cell->info().dv() - m) / (n) - cell->info().p()) * relax + cell->info().p();
					cell->info().p() = (- (cell->info().dv() - m) * cell->info().inv_sum_k - cell->info().p()) * relax + cell->info().p();
					#ifdef GS_OPEN_MP
// 					double r = sqrt(sqrt(sqrt(cell->info().p())/(1+sqrt(cell->info().p()))));
// 					if (j % 100 == 0) cout<<"cell->info().p() "<<cell->info().p()<<" vs. "<< (- (cell->info().dv() - m) / (n) - cell->info().p())* relax<<endl;
					#endif
				}
                                dp -= cell->info().p();
                                #ifdef GS_OPEN_MP
                                const int tn=omp_get_thread_num();
				t_sum_dp[tn] += std::abs(dp);
				t_dp_max[tn]=std::max(t_dp_max[tn], std::abs(dp));
				t_p_max[tn]= std::max(t_p_max[tn], std::abs(cell->info().p()));
				t_sum_p[tn]+= std::abs(cell->info().p());
				#else
                                dp_max = std::max(dp_max, std::abs(dp));
                                p_max = std::max(p_max, std::abs(cell->info().p()));
                                sum_p += std::abs(cell->info().p());
                                sum_dp += std::abs(dp);
				#endif
                        }
                }
                #ifdef GS_OPEN_MP

                for (int ii=0;ii<num_threads;ii++) p_max =std::max(p_max, t_p_max[ii]);
		for (int ii=0;ii<num_threads;ii++) dp_max =std::max(dp_max, t_dp_max[ii]);
		for (int ii=0;ii<num_threads;ii++) sum_p+=t_sum_p[ii];
                for (int ii=0;ii<num_threads;ii++) sum_dp+=t_sum_dp[ii];
//                 cerr<< p_max<< " "<<dp_max<<" "<<sum_p<<" "<<sum_dp<<endl;
                #endif
		p_moy = sum_p/cell2;
                dp_moy = sum_dp/cell2;
		#ifdef GS_OPEN_MP
		#pragma omp master
		#endif
		j++;
//                  		if (j % 100 == 0) {
//                         // cout << "pmax " << p_max << "; pmoy : " << p_moy << "; dpmax : " << dp_max << endl;
//                         cout << "iteration " << j <<"; erreur : " << dp_max/p_max << endl;
//                         //     save_vtk_file ( Tri );
//                  }
	#ifdef GS_OPEN_MP
	} while (j<1500);
	#else
	} while ((dp_max/p_max) > tolerance /*&& ( dp_max > tolerance )*//* &&*/ /*( j<50 )*/);
	#endif
	}

        if (DEBUG_OUT) {cout << "pmax " << p_max << "; pmoy : " << p_moy << endl;
        cout << "iteration " << j <<"; erreur : " << dp_max/p_max << endl;}
// 	iter << j << " " << dp_max/p_max << endl;
	int cel=0;
	double Pav=0;
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
		cel++;
		Pav+=cell->info().p();
	}
	Pav/=cel;
// 	p_av << Pav << endl;
// 	Iterations = j;

        //Display fluxes?
//  bool ref =  Tri.finite_cells_begin()->info().isvisited;
//  for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
//   for (int i =0;i<4;i++) {
//    if (cell->neighbor(i)->info().isvisited == ref) {
//     cout <<"nodes : "<<(Point) cell->info()<< " "<<(Point) cell->neighbor(i)->info()<<", p1/p2 : " <<cell->info().p()<<"/"<<cell->neighbor(i)->info().p()<<", permeability : "<<cell->info().k_norm()[i]<<" flux : "<< cell->info().k_norm()[i]*(cell->info().p()-cell->neighbor(i)->info().p())<<endl;
//    }
//   }
//   cell->info().isvisited = !ref;
//  }
}



double FlowBoundingSphere::Permeameter(double P_Inf, double P_Sup, double Section, double DeltaY, char *file)
{
  RTriangulation& Tri = T[currentTes].Triangulation();
  std::ofstream kFile(file, std::ios::out);
  double Q2=0, Q1=0;
  int cellQ1=0, cellQ2=0;
  double p_out_max=0, p_out_min=100000, p_in_max=0, p_in_min=100000,p_out_moy=0, p_in_moy=0;

  Tesselation::Vector_Cell tmp_cells;
  tmp_cells.resize(10000);
  Tesselation::VCell_iterator cells_it = tmp_cells.begin();

  Tesselation::VCell_iterator cell_up_end = Tri.incident_cells(T[currentTes].vertexHandles[y_max_id],cells_it);
  for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cell_up_end; it++)
  {
    Cell_handle& cell = *it;{for (int j2=0; j2<4; j2++) {if (!cell->neighbor(j2)->info().Pcondition){
    Q1 = Q1 + (cell->neighbor(j2)->info().k_norm())[Tri.mirror_index(cell, j2)]* (cell->neighbor(j2)->info().p()-cell->info().p());
    cellQ1+=1;
    p_out_max = std::max(cell->neighbor(j2)->info().p(), p_out_max);
    p_out_min = std::min(cell->neighbor(j2)->info().p(), p_out_min);
    p_out_moy += cell->neighbor(j2)->info().p();}
  }}}

  Tesselation::VCell_iterator cell_down_end = Tri.incident_cells(T[currentTes].vertexHandles[y_min_id],cells_it);
  for (Tesselation::VCell_iterator it = tmp_cells.begin(); it != cell_down_end; it++)
  {
    Cell_handle& cell = *it;{for (int j2=0; j2<4; j2++) {if (!cell->neighbor(j2)->info().Pcondition){
    Q2 = Q2 + (cell->neighbor(j2)->info().k_norm())[Tri.mirror_index(cell, j2)]* (cell->info().p()-cell->neighbor(j2)->info().p());
    cellQ2+=1;
    p_in_max = std::max(cell->neighbor(j2)->info().p(), p_in_max);
    p_in_min = std::min(cell->neighbor(j2)->info().p(), p_in_min);
    p_in_moy += cell->neighbor(j2)->info().p();}
  }}}

	if (DEBUG_OUT){
	cout << "the maximum superior pressure is = " << p_out_max << " the min is = " << p_in_min << endl;
	cout << "the maximum inferior pressure is = " << p_in_max << " the min is = " << p_out_min << endl;
	cout << "superior average pressure is " << p_out_moy/cellQ1 << endl;
        cout << "inferior average pressure is " << p_in_moy/cellQ2 << endl;
        cout << "celle comunicanti in basso = " << cellQ2 << endl;
        cout << "celle comunicanti in alto = " << cellQ1 << endl;}

        double density = 1;
        double viscosity = 1;
        double gravity = 1;
        double Vdarcy = Q1/Section;
        double GradP = abs(P_Inf-P_Sup) /DeltaY;
        double GradH = GradP/ (density*gravity);
        double Ks= (Vdarcy) /GradH;
        double k= Ks*viscosity/ (density*gravity);

	if (DEBUG_OUT){
        cout << "The incoming average flow rate is = " << Q2 << " m^3/s " << endl;
        cout << "The outgoing average flow rate is = " << Q1 << " m^3/s " << endl;
        cout << "The gradient of charge is = " << GradH << " [-]" << endl;
        cout << "Darcy's velocity is = " << Vdarcy << " m/s" <<endl;

        cout << "The permeability of the sample is = " << k << " m^2" <<endl;}

	kFile << "the maximum superior pressure is = " << p_in_max << " the min is = " << p_in_min << endl;
        kFile << "the maximum inferior pressure is = " << p_out_max << " the min is = " << p_out_min << endl;
        kFile << "superior average pressure is " << p_in_moy/cellQ2 << endl;
        kFile << "inferior average pressure is " << p_out_moy/cellQ1 << endl;
        kFile << "celle comunicanti in basso = " << cellQ1 << endl;
        kFile << "celle comunicanti in basso = " << cellQ1 << endl;
	kFile << "The incoming average flow rate is = " << Q2 << " m^3/s " << endl;
        kFile << "The outgoing average flow rate is = " << Q1 << " m^3/s " << endl;
        kFile << "The gradient of charge is = " << GradH << " [-]" << endl;
        kFile << "Darcy's velocity is = " << Vdarcy << " m/s" <<endl;
        kFile << "The hydraulic conductivity of the sample is = " << Ks << " m/s" <<endl;
        kFile << "The permeability of the sample is = " << k << " m^2" <<endl;
        //   cout << "The Darcy permeability of the sample is = " << k_darcy/0.987e-12 << " darcys" << endl;

	cout << endl << "The hydraulic conductivity of the sample is = " << Ks << " m/s" << endl << endl;
	return Ks;
}

void FlowBoundingSphere::DisplayStatistics()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
        int Zero =0, Inside=0, Fictious=0;
        Finite_cells_iterator cell_end = Tri.finite_cells_end();
        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
                int zeros =0;
                for (int j=0; j!=4; j++) {
                        if ((cell->info().k_norm())[j]==0) {
                                zeros+=1;
                        }
                }
                if (zeros==4) {
                        Zero+=1;
                }
                if (!cell->info().fictious()) {
                        Inside+=1;
                } else {
                        Fictious+=1;
                }
        }

        int fict=0, real=0;
        for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
                if (v->info().isFictious) fict+=1;
                else real+=1;
        }
        long Vertices = Tri.number_of_vertices();
	long Cells = Tri.number_of_finite_cells();
	long Facets = Tri.number_of_finite_facets();
        if(DEBUG_OUT) {cout << "zeros = " << Zero << endl;
	cout << "There are " << Vertices << " vertices, dont " << fict << " fictious et " << real << " reeeeeel" << std::endl;
        cout << "There are " << Cells << " cells " << std::endl;
        cout << "There are " << Facets << " facets " << std::endl;
        cout << "There are " << Inside << " cells INSIDE." << endl;
        cout << "There are " << Fictious << " cells FICTIOUS." << endl;}

	vtk_infinite_vertices = fict;
	vtk_infinite_cells = Fictious;
	num_particles = real;
}

void FlowBoundingSphere::save_vtk_file()
{
  RTriangulation& Tri = T[currentTes].Triangulation();
        static unsigned int number = 0;
        char filename[80];
	mkdir("./VTK", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        sprintf(filename,"./VTK/out_%d.vtk", number++);

        basicVTKwritter vtkfile((unsigned int) Tri.number_of_vertices()-vtk_infinite_vertices, (unsigned int) Tri.number_of_finite_cells()-vtk_infinite_cells);

        vtkfile.open(filename,"test");

        vtkfile.begin_vertices();
        double x,y,z;
        for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
		if (!v->info().isFictious){
		x = (double)(v->point().point()[0]);
                y = (double)(v->point().point()[1]);
                z = (double)(v->point().point()[2]);
                vtkfile.write_point(x,y,z);}
        }
        vtkfile.end_vertices();

        vtkfile.begin_cells();
        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		if (!cell->info().fictious()){vtkfile.write_cell(cell->vertex(0)->info().id()-6, cell->vertex(1)->info().id()-6, cell->vertex(2)->info().id()-6, cell->vertex(3)->info().id()-6);}
        }
        vtkfile.end_cells();

// 	vtkfile.begin_data("Force",POINT_DATA,VECTORS,FLOAT);
// 	for (Finite_vertices_iterator v = T.finite_vertices_begin(); v != T.finite_vertices_end(); ++v)
// 	{if (!v->info().isFictious) vtkfile.write_data((v->info().forces)[0],(v->info().forces)[1],(v->info().forces)[2]);}
// 	vtkfile.end_data();

	vtkfile.begin_data("Pressure",CELL_DATA,SCALARS,FLOAT);
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		if (!cell->info().fictious()){vtkfile.write_data(cell->info().p());}
	}
	vtkfile.end_data();

	vtkfile.begin_data("Velocity",POINT_DATA,VECTORS,FLOAT);
	for (Finite_vertices_iterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v)
	{if (!v->info().isFictious) vtkfile.write_data((v->info().vel())[0],(v->info().vel())[1],(v->info().vel())[2]);}
	vtkfile.end_data();

// 	vtkfile.begin_data("Velocity",CELL_DATA,VECTORS,FLOAT);
// 	for (Finite_cells_iterator cell = T.finite_cells_begin(); cell != T.finite_cells_end(); ++cell) {
// 		if (!cell->info().fictious()){vtkfile.write_data((cell->info().av_vel())[0],(cell->info().av_vel())[1],(cell->info().av_vel())[2]);}
// 	}
// 	vtkfile.end_data();
}

void FlowBoundingSphere::MGPost()
{
	RTriangulation& Tri = T[currentTes].Triangulation();
        Point P;

        ofstream file("mgp.out.001");
        file << "<?xml version=\"1.0\"?>" << endl;
        file << "<mgpost mode=\"3D\">" << endl;
        file << " <state time=\"0\">" << endl;

        Finite_cells_iterator cell_end = Tri.finite_cells_end();

        for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
                if (cell->info().fictious()==0) {
                        double p [3] = {0,0,0};

                        for (int j2=0; j2!=4; j2++) {
                                Vertex_handle v = cell->vertex(j2);
                                for (int i=0; i<3;  i++) {
                                        p[i] += 0.25* v->point().point()[i];
                                }
                        }

                        double pressure =  cell->info().p();
                        double rad = 0.00025;

                        file << "  <body>" << endl;
                        file << "   <SPHER r=\""  <<  rad << "\">" << endl;
                        file << "    <position x=\""  <<  p[0] << "\" y=\"" << p[1] << "\" z=\"" << p[2] << "\"/>" << endl;
                        file << "    <velocity x=\""  <<  pressure << "\" y=\"" << pressure << "\" z=\"" << pressure << "\"/>" << endl;
                        file << "   </SPHER>" << endl;
                        file << "  </body>" << endl;
                }
        }
        file << " </state>" << endl;
        file << "</mgpost>" << endl;
}
#ifdef XVIEW
void FlowBoundingSphere::Dessine_Triangulation(Vue3D &Vue, RTriangulation &T)
{
        double* Segments = NULL;
        long N_seg = New_liste_edges(T, &Segments);
        Vue.Dessine_Segment(Segments, N_seg);
        Delete_liste_edges(&Segments, N_seg);
}
void FlowBoundingSphere::Dessine_Short_Tesselation(Vue3D &Vue, Tesselation &Tes)
{
        if (!Tes.Computed()) Tes.Compute();
        double* Segments = NULL;
        long N_seg = Tes.New_liste_short_edges(&Segments);
        Vue.Dessine_Segment(Segments, N_seg);
        Delete_liste_edges(&Segments, N_seg);
}
#endif
void FlowBoundingSphere::GenerateVoxelFile( )
{
	RTriangulation& Tri = T[currentTes].Triangulation();
        double l = 1;
        int dx = 200;
        double eps = l/dx;

        std::ofstream voxelfile("MATRIX",std::ios::out);
        bool solid=false;

        for (double y=0; y<=l; y+=eps) {
                for (double z=0; z<=l; z+=eps) {
                        for (double x=0; x<=l; x+=eps) {
                                solid=false;

                                for (Finite_vertices_iterator V_it = Tri.finite_vertices_begin(); V_it != Tri.finite_vertices_end(); V_it++) {
                                        double rayon = sqrt(V_it->point().weight());
                                        if ((sqrt(pow((x- (V_it->point()[0])),2) +pow((y- (V_it->point()[1])),2) +pow((z- (V_it->point()[2])),2))) <= rayon) solid=true;
                                }
                                if (solid) voxelfile << 1;
                                else voxelfile << 0;
                        }
                        voxelfile << endl;
                }
        }
}

double FlowBoundingSphere::PressureProfile(char *filename, Real& time, int& intervals)
{
	RTriangulation& Tri = T[currentTes].Triangulation();
	vector<double> Pressures;

	/** CONSOLIDATION CURVES **/
        Cell_handle permeameter;
        int n=0, k=0;
        vector<double> P_ave;
        std::ofstream consFile(filename, std::ios::out);

        double Rx = (x_max-x_min) /intervals;
        double Ry = (y_max-y_min) /intervals;
        double Rz = (z_max-z_min) /intervals;

	for (double Y=y_min; Y<=y_max+Ry/10; Y=Y+Ry) {
                P_ave.push_back(0);
		for (double X=x_min; X<=x_max+Ry/10; X=X+Rx) {
			for (double Z=z_min; Z<=z_max+Ry/10; Z=Z+Rz) {
                                P_ave[k]+=Tri.locate(Point(X, Y, Z))->info().p();
				n++;
                        }
                }
                P_ave[k]/= (n);
                consFile<<k<<" "<<time<<" "<<P_ave[k]<<endl;
                if (k==intervals/2) Pressures.push_back(P_ave[k]);
                n=0; k++;
	}
	return P_ave[intervals/2];
}

void FlowBoundingSphere::mplot (char *filename)
{
	RTriangulation& Tri = T[currentTes].Triangulation();
	std::ofstream plot (filename, std::ios::out);
	Cell_handle permeameter;
	int intervals = 30;
	double Rx = (x_max-x_min) /intervals;
	double Ry = (y_max-y_min) /intervals;
	double Z = (z_max+z_min)/2.0;
	double Press = 0;
	for (double Y=y_min; Y<y_max; Y=Y+Ry) {
		for (double X=x_min; X<x_max; X=X+Rx) {
				permeameter = Tri.locate(Point(X, Y, Z));
				permeameter->info().p()<0? Press=0 : Press=permeameter->info().p();
				plot << Y << " " << X << " " << Press << endl;
		}
	}
}

double FlowBoundingSphere::Sample_Permeability(double& x_Min,double& x_Max ,double& y_Min,double& y_Max,double& z_Min,double& z_Max, string key)
{
        double Section = (x_Max-x_Min) * (z_Max-z_Min);
        double DeltaY = y_Max-y_Min;
        boundary(y_min_id).flowCondition=0;
        boundary(y_max_id).flowCondition=0;
        boundary(y_min_id).value=0;
        boundary(y_max_id).value=1;
	double P_zero = abs((boundary(y_min_id).value-boundary(y_max_id).value)/2);
        Initialize_pressures( P_zero );
	GaussSeidel();

        char *kk;
        kk = (char*) key.c_str();
        return Permeameter(boundary(y_min_id).value, boundary(y_max_id).value, Section, DeltaY, kk);
}

bool FlowBoundingSphere::isInsideSphere(double& x, double& y, double& z)
{
	RTriangulation& Tri = T[currentTes].Triangulation();
        for (Finite_vertices_iterator V_it = Tri.finite_vertices_begin(); V_it != Tri.finite_vertices_end(); V_it++) {
                double rayon = V_it->point().weight();
                if (pow((x- (V_it->point()[0])),2) +pow((y- (V_it->point()[1])),2) +pow((z- (V_it->point()[2])),2)   <= rayon) return true;
        }
        return false;
}

void FlowBoundingSphere::SliceField(char *filename)
{
        /** Pressure field along one cutting plane **/
	RTriangulation& Tri = T[currentTes].Triangulation();
        Cell_handle permeameter;

        std::ofstream consFile(filename,std::ios::out);

        int intervals = 400;
        double Ry = (y_max-y_min) /intervals;
        double Rz = (z_max-z_min) /intervals;
//         cout<<Rx<<" "<<Ry<<" "<<Rz<<" "<<z_max<<" "<<z_min<<" "<<y_max<<" "<<y_min<<" "<<x_max<<" "<<x_min<<endl;

//         for (double X=min(x_min,x_max); X<=max(x_min,x_max); X=X+abs(Rx)) {
	double X=0.5;
                for (double Y=min(y_max,y_min); Y<=max(y_max,y_min); Y=Y+abs(Ry)) {
                        for (double Z=min(z_min,z_max); Z<=max(z_min,z_max); Z=Z+abs(Rz)) {
			  permeameter = Tri.locate(Point(X, Y, Z));
			  consFile << permeameter->info().p() <<" ";
//                                 if (!isInsideSphere(X,Y,Z)) {
//                                         permeameter = Tri.locate(Point(X, Y, Z));
//                                         consFile << permeameter->info().p() <<" ";
//                                         //cout <<"valeur trouvée";
//                                 } else consFile << "Nan ";
                        }
                        consFile << endl;}
                consFile << endl;
//         }
        consFile.close();
}

void FlowBoundingSphere::ComsolField()
{
	//Compute av. velocity first, because in the following permeabilities will be overwritten with "junk" (in fact velocities from comsol)
	Average_Cell_Velocity();

  	RTriangulation& Tri = T[currentTes].Triangulation();
        Cell_handle c;
  	ifstream loadFile("vx_grid_03_07_ns.txt");
	ifstream loadFileY("vy_grid_03_07_ns.txt");
	ifstream loadFileZ("vz_grid_03_07_ns.txt");
	int Nx=100; int Ny=10; int Nz=100;
	std::ofstream consFile("velComp",std::ios::out);

	Finite_cells_iterator cell_end = Tri.finite_cells_end();
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++){
		cell->info().dv()=0;
		cell->info().module_permeability[0]=cell->info().module_permeability[1]=cell->info().module_permeability[2]=0;
	}

	vector<Real> X, Y, Z;
	Real buffer;
	for (int xi=0;xi<Nx;xi++) {loadFile >> buffer; X.push_back(buffer); loadFileY >> buffer; loadFileZ >> buffer;}
	for (int yi=0;yi<Ny;yi++) {loadFile >> buffer; Y.push_back(buffer); loadFileY >> buffer; loadFileZ >> buffer;}
	for (int zi=0;zi<Nz;zi++) {loadFile >> buffer; Z.push_back(buffer); loadFileY >> buffer; loadFileZ >> buffer;}

	Real vx, vy, vz;
	Real meanCmsVel=0; int totCmsPoints = 0;
	for (int zi=0;zi<Nz;zi++)
	  	for (int yi=0;yi<Ny;yi++)
		  	for (int xi=0;xi<Nx;xi++) {
			  	loadFile >> vx; loadFileY >> vy; loadFileZ >> vz;
				if (!isInsideSphere(X[xi], Y[yi], Z[zi]) && vx!=0) {
                                        c = Tri.locate(Point(X[xi], Y[yi], Z[zi]));
                                        c->info().module_permeability[0]+=vx;
					c->info().module_permeability[1]+=vy;
					c->info().module_permeability[2]+=vz;
					c->info().dv()+=1;
					meanCmsVel+=vy; totCmsPoints++;}
	}
	int kk=0;
	Vecteur diff;
	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end && kk<10000; cell++){
		if (cell->info().fictious() || cell->info().dv()<60) continue;
		for (int k=0;k<3;k++) cell->info().module_permeability[k]/=cell->info().dv();
		cerr << cell->info().module_permeability[0]<<" "<< cell->info().module_permeability[1]<<" "<< cell->info().module_permeability[2]<<" "<<cell->info().dv()<<" "<< cell->info().av_vel()<<endl;
		Real m=sqrt(pow(cell->info().module_permeability[0],2)+pow(cell->info().module_permeability[1],2)+pow(cell->info().module_permeability[2],2));
		Vecteur comFlow (cell->info().module_permeability[0],cell->info().module_permeability[1],cell->info().module_permeability[2]);
		Real angle=asin(sqrt(cross_product(comFlow,cell->info().av_vel()).squared_length())/(sqrt(comFlow.squared_length())*sqrt(cell->info().av_vel().squared_length())));
		cerr<<"norms : "<<m<<" vs. "<<sqrt(cell->info().av_vel().squared_length())<<" angle "<<180*angle/3.1415<<endl;
		consFile<<m<<" "<<sqrt(cell->info().av_vel().squared_length())<<" "<<180*angle/3.1415<<endl;
		diff = diff + (comFlow - cell->info().av_vel());
		kk++;
	}
	cerr << "meanCmsVel "<<meanCmsVel/totCmsPoints<<" mean diff "<<diff/kk<<endl;
}

} //namespace CGT

#endif //FLOW_ENGINE

#ifdef LINSOLV
#include "FlowBoundingSphereLinSolv.cpp"
#endif
