/*************************************************************************
*  Copyright (C) 2010 by Emanuele Catalano <catalano@grenoble-inp.fr>    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef _FLOWBOUNDINGSPHERE_H
#define _FLOWBOUNDINGSPHERE_H

#include "Operations.h"
#include "Timer.h"
#include "Tesselation.h"
#include "basicVTKwritter.hpp"
#include "Timer.h"
#include "stdafx.h"
#include "Empilement.h"
#include "Network.h"

#ifdef XVIEW
#include "Vue3D.h" //FIXME implicit dependencies will look for this class (out of tree) even ifndef XVIEW
#endif

#ifdef FLOW_ENGINE

using namespace std;

namespace CGT{

class FlowBoundingSphere : public Network
{
	public:
		virtual ~FlowBoundingSphere();
 		FlowBoundingSphere();
		
// 		int x_min_id, x_max_id, y_min_id, y_max_id, z_min_id, z_max_id;
// 		int* boundsIds [6];
// 		bool currentTes;
		bool SLIP_ON_LATERALS;
		double TOLERANCE;
		double RELAX;
		double ks; //Hydraulic Conductivity
		bool meanK_LIMIT, meanK_STAT, distance_correction;
		bool DEBUG_OUT;
		bool noCache;//flag for checking if cached values cell->unitForceVectors have been defined
		bool computeAllCells;//exececute computeHydraulicRadius for all facets and all spheres (double cpu time but needed for now in order to define crossSections correctly)
		double K_opt_factor;
		int Iterations;
		bool RAVERAGE;
		
// 		Boundary boundaries [6];
		int walls_id[6];
// 		short id_offset;
//  		Boundary& boundary (int b) {return boundaries[b-id_offset];}
		
		void mplot ( char *filename);
		void Localize ();

		void Compute_Permeability();
		
		double Vpore, Ssolid;

		void GaussSeidel ( );
// 		void Compute_Forces ();
		void Fictious_cells ( );

// 		Tesselation T [2];
		
// 		double x_min, x_max, y_min, y_max, z_min, z_max, Rmoy;
// 		Real Vsolid_tot, Vtotalissimo, Vporale, Ssolid_tot;
		double k_factor; //permeability moltiplicator
		std::string key; //to give to consolidation files a name with iteration number
		std::vector<double> Pressures; //for automatic write maximum pressures during consolidation
		bool tess_based_force; //allow the force computation method to be chosen from FlowEngine
		Real minPermLength; //min branch length for Poiseuille
		
		double P_SUP, P_INF, P_INS;
		
// 		void AddBoundingPlanes ( Tesselation& Tes, double x_Min,double x_Max ,double y_Min,double y_Max,double z_Min,double z_Max );
// 		void AddBoundingPlanes(bool yade);
// 		void AddBoundingPlanes();
// 		void AddBoundingPlanes(Real center[3], Real Extents[3], int id);
		
		Tesselation& Compute_Action ( );
		Tesselation& Compute_Action ( int argc, char *argv[ ], char *envp[ ] );
// 		Vecteur external_force_single_fictious ( Cell_handle cell );
		void SpheresFileCreator ();
// 		void Analytical_Consolidation ( );
		
// 		void Boundary_Conditions ( RTriangulation& Tri );
		void Initialize_pressures ( double P_zero );
		/// Define forces using the same averaging volumes as for permeability
		void ComputeTetrahedralForces();
		/// Define forces spliting drag and buoyancy terms
		void ComputeFacetForces();
		void ComputeFacetForcesWithCache();
		void save_vtk_file ( );
		void MGPost ( );
#ifdef XVIEW
		void Dessine_Triangulation ( Vue3D &Vue, RTriangulation &T );
		void Dessine_Short_Tesselation ( Vue3D &Vue, Tesselation &Tes );
#endif
		double Permeameter ( double P_Inf, double P_Sup, double Section, double DeltaY, char *file );
		double Sample_Permeability( double& x_Min,double& x_Max ,double& y_Min,double& y_Max,double& z_Min,double& z_Max, string key);
		double Compute_HydraulicRadius (Cell_handle cell, int j );
		double PressureProfile ( char *filename, Real& time, int& intervals );

		double dotProduct ( Vecteur x, Vecteur y );
		double Compute_EffectiveRadius(Cell_handle cell, int j);
		double Compute_EquivalentRadius(Cell_handle cell, int j);

// 		double crossProduct ( double x[3], double y[3] );

// 		double surface_solid_facet ( Sphere ST1, Sphere ST2, Sphere ST3 );
// 		Vecteur surface_double_fictious_facet ( Vertex_handle fSV1, Vertex_handle fSV2, Vertex_handle SV3 );
// 		Vecteur surface_single_fictious_facet ( Vertex_handle fSV1, Vertex_handle SV2, Vertex_handle SV3 );
		
// 		double surface_solid_double_fictious_facet ( Vertex_handle ST1, Vertex_handle ST2, Vertex_handle ST3 );
		
// 		double surface_external_triple_fictious (Cell_handle cell, Boundary b );
// 		double surface_external_triple_fictious ( Real position[3], Cell_handle cell, Boundary b );		
// 		double surface_external_double_fictious ( Cell_handle cell, Boundary b );
		
// 		double surface_external_single_fictious ( Cell_handle cell, Boundary b );

		void GenerateVoxelFile ( );

		RTriangulation& Build_Triangulation ( Real x, Real y, Real z, Real radius, unsigned const id );

		void Build_Tessalation ( RTriangulation& Tri );

// 		double spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 );
		
// 		double fast_spherical_triangle_area ( const Sphere& STA1, const Point& STA2, const Point& STA3, const Point& PTA1 );
// 		Real solid_angle ( const Point& STA1, const Point& STA2, const Point& STA3, const Point& PTA1 );
// 		double spherical_triangle_volume ( const Sphere& ST1, const Point& PT1, const Point& PT2, const Point& PT3 );
// 		Real fast_solid_angle ( const Point& STA1, const Point& PTA1, const Point& PTA2, const Point& PTA3 );
		
		bool isInsideSphere ( double& x, double& y, double& z );
		
		void SliceField ();
		void ComsolField();

		void Interpolate ( Tesselation& Tes, Tesselation& NewTes );
		
// 		double volume_single_fictious_pore ( Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1 );
		//Fast version, assign surface of facet for future forces calculations (pointing from PV2 to PV1)
// 		double volume_single_fictious_pore ( const Vertex_handle& SV1, const Vertex_handle& SV2, const Vertex_handle& SV3, const Point& PV1,  const Point& PV2, Vecteur& facetSurface);
// 		double volume_double_fictious_pore ( Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1 );
		//Fast version, assign surface of facet for future forces calculations (pointing from PV2 to PV1)
// 		double volume_double_fictious_pore (Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point& PV1, Point& PV2, Vecteur& facetSurface);

// 		double PoreVolume (RTriangulation& Tri, Cell_handle cell);
		int Average_Cell_Velocity(int id_sphere, RTriangulation& Tri);
		void Average_Cell_Velocity();
		void Average_Grain_Velocity();
		void vtk_average_cell_velocity(RTriangulation &T, int id_sphere, int num_cells);
		void ApplySinusoidalPressure(RTriangulation& Tri, double Pressure, double load_intervals);
		
// 		double surface_external_triple_fictious(Real position[3], Cell_handle cell, Boundary b);
// 		double surface_external_double_fictious(Cell_handle cell, Boundary b);
// 		double surface_external_single_fictious(Cell_handle cell, Boundary b);

};

} //namespace CGT
#endif //FLOW_ENGINE

#endif
