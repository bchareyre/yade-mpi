
#ifndef _FLOWBOUNDINGSPHERE_H
#define _FLOWBOUNDINGSPHERE_H


#ifdef XVIEW
	#include "Vue3D.h"
#endif

// #include "Empilement.h"
#include "Operations.h"
// #include "Commandes_Spheres.h"
#include "Timer.h"
#include "Tesselation.h"

#include "basicVTKwritter.hpp"
#include "Timer.h"

#include "stdafx.h"
#include "Deformation.h"
#include "Empilement.h"

namespace CGT{

struct Boundary
{
	Point p;
	Vecteur normal;
	int coordinate;
	bool condition;//condition=1, pressure is imposed // condition=0, flow is imposed
};

class FlowBoundingSphere
{
	public:
// 		FlowBoundingSphere( );
		
		int x_min_id, x_max_id, y_min_id, y_max_id, z_min_id, z_max_id;
		bool currentTes;
// 		double Vsolid_tot, Vtotalissimo, Vporale, Ssolid_tot;
		
		Boundary boundaries [6];
		short id_offset;
// 		Boundary& boundary ( int b ) {return boundaries[b-id_offset];}
		
		void insert ( Real x, Real y, Real z, Real radius, int id );
		void Localize ();
		void Compute_Permeability();
		void AddBoundingPlanes();
		void AddBoundingPlanes ( Real center[3], Real Extents[3],int id );
		void DisplayStatistics();
		void Tesselate();
		void GaussSeidel ( );
		void Compute_Forces ();
		double surface_external_triple_fictious ( Real position[3], Cell_handle cell, Boundary b );
		void Fictious_cells ( );

		Tesselation T [2];
		
		double x_min, x_max, y_min, y_max, z_min, z_max, Rmoy;
		Real Vsolid_tot, Vtotalissimo, Vporale, Ssolid_tot;
		double k_factor; //permeability moltiplicator
		string key; //to give to consolidation files a name with iteration number
		vector<double> Pressures; //for automatic write maximum pressures during consolidation
		bool tess_based_force; //allow the force computation method to be chosen from FlowEngine
		
		double P_SUP, P_INF, P_INS;

		void AddBoundingPlanes ( Tesselation& Tes);
		
		void AddBoundingPlanes ( Tesselation& Tes, double x_Min,double x_Max ,double y_Min,double y_Max,double z_Min,double z_Max );

		void Compute_Action ( );

		void Compute_Action ( int argc, char *argv[ ], char *envp[ ] );

		void Localize ( RTriangulation& Tri );

		void DisplayStatistics ( RTriangulation& Tri );

		void Compute_Forces ( RTriangulation& Tri );

		Vecteur external_force_single_fictious ( Cell_handle cell );
		
		void UpdateVolumes ( RTriangulation& Tri, vector<double>& X,  vector<double>& Y,  vector<double>& Z, vector<double>& X2,  vector<double>& Y2,  vector<double>& Z2 );

		void SpheresFileCreator ();
		
		void Analytical_Consolidation ( );
		
		void Boundary_Conditions ( RTriangulation& Tri );
		
		void Initialize_pressures ( );

		void GaussSeidel ( RTriangulation& Tri );

		void save_vtk_file ( RTriangulation &T );

		void MGPost ( RTriangulation& Tri );
#ifdef XVIEW
		void Dessine_Triangulation ( Vue3D &Vue, RTriangulation &T );

		void Dessine_Short_Tesselation ( Vue3D &Vue, Tesselation &Tes );
#endif
		void Permeameter ( RTriangulation& Tri, double P_Inf, double P_Sup, double Section, double DeltaY, char *file );

		void Sample_Permeability ( RTriangulation& Tri, double x_Min,double x_Max ,double y_Min,double y_Max,double z_Min,double z_Max, string key );

		void Compute_Permeability ( RTriangulation& Tri );
		
		double Compute_HydraulicRadius ( RTriangulation& Tri, Cell_handle cell, int j );

		void PermeameterCurve ( RTriangulation& Tri, char *filename, Real time );

		double dotProduct ( Vecteur x, Vecteur y );

		double crossProduct ( double x[3], double y[3] );

		double surface_solid_facet ( Sphere ST1, Sphere ST2, Sphere ST3 );
		
		double surface_solid_fictious_facet ( Vertex_handle ST1, Vertex_handle ST2, Vertex_handle ST3 );
		
		double surface_external_triple_fictious (Cell_handle cell, Boundary b );
		
		double surface_external_double_fictious ( Cell_handle cell, Boundary b );
		
		double surface_external_single_fictious ( Cell_handle cell, Boundary b );

		void GenerateVoxelFile ( RTriangulation& Tri );

		RTriangulation& Build_Triangulation ( Real x, Real y, Real z, Real radius, unsigned const id );

		void Build_Tessalation ( RTriangulation& Tri );

		double spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 );

		double fast_spherical_triangle_area ( Sphere STA1, Sphere STA2, Sphere STA3, Point PTA1 );

		double spherical_triangle_volume ( Sphere ST1, Sphere ST2, Sphere ST3, Point PT1 );

		bool isInsideSphere ( RTriangulation& Tri, double x, double y, double z );
		
		void SliceField ( RTriangulation& Tri );

		void Interpolate ( Tesselation& Tes, Tesselation& NewTes );
		
		double volume_single_fictious_pore ( Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1 );
		
		double volume_double_fictious_pore ( Vertex_handle SV1, Vertex_handle SV2, Vertex_handle SV3, Point PV1 );
};


} //namespace CGT
#endif
