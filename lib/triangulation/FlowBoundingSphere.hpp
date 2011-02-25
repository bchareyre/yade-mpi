/*************************************************************************
*  Copyright (C) 2010 by Emanuele Catalano <catalano@grenoble-inp.fr>    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE

#ifndef _FLOWBOUNDINGSPHERE_H
#define _FLOWBOUNDINGSPHERE_H

#include "Operations.h"
#include "Timer.h"
#include "Tesselation.h"
#include "basicVTKwritter.hpp"
#include "Timer.h"
#include "stdafx.h"
#include "Empilement.h"
#include "Network.hpp"

#ifdef XVIEW
#include "Vue3D.h" //FIXME implicit dependencies will look for this class (out of tree) even ifndef XVIEW
#endif



using namespace std;



namespace CGT{

class FlowBoundingSphere : public Network
{
	public:
		virtual ~FlowBoundingSphere();
 		FlowBoundingSphere();

		bool SLIP_ON_LATERALS;
		double TOLERANCE;
		double RELAX;
		double ks; //Hydraulic Conductivity
		bool meanK_LIMIT, meanK_STAT, distance_correction;
		bool OUTPUT_BOUDARIES_RADII;
		bool noCache;//flag for checking if cached values cell->unitForceVectors have been defined
		vector<pair<Point,Real> > imposedP;

		void initNewTri () {noCache=true; /*isLinearSystemSet=false; areCellsOrdered=false;*/}//set flags after retriangulation

		bool computeAllCells;//exececute computeHydraulicRadius for all facets and all spheres (double cpu time but needed for now in order to define crossSections correctly)
		double K_opt_factor;
		int Iterations;

		bool RAVERAGE;
		int walls_id[6];

		void mplot ( char *filename);
		void Localize();

		void Compute_Permeability();
		virtual void GaussSeidel ( );
		virtual void ResetNetwork();

		void Fictious_cells ( );

		double k_factor; //permeability moltiplicator
		std::string key; //to give to consolidation files a name with iteration number
		std::vector<double> Pressures; //for automatic write maximum pressures during consolidation
		bool tess_based_force; //allow the force computation method to be chosen from FlowEngine
		Real minPermLength; //min branch length for Poiseuille

		double P_SUP, P_INF, P_INS, VISCOSITY;

		Tesselation& Compute_Action ( );
		Tesselation& Compute_Action ( int argc, char *argv[ ], char *envp[ ] );
		Tesselation& LoadPositions(int argc, char *argv[ ], char *envp[ ]);
		void SpheresFileCreator ();
		void DisplayStatistics();
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

		void GenerateVoxelFile ( );

		RTriangulation& Build_Triangulation ( Real x, Real y, Real z, Real radius, unsigned const id );

		void Build_Tessalation ( RTriangulation& Tri );

		bool isInsideSphere ( double& x, double& y, double& z );

		void SliceField (char *filename);
		void ComsolField();

		void Interpolate ( Tesselation& Tes, Tesselation& NewTes );
		void Average_Cell_Velocity();
		void Average_Fluid_Velocity();
		void ApplySinusoidalPressure(RTriangulation& Tri, double Amplitude, double Average_Pressure, double load_intervals);
		bool isOnSolid  (double X, double Y, double Z);
		void Measure_Pore_Pressure(double Wall_up_y, double Wall_down_y);
		
		//Solver?
		int useSolver;//(0 : GaussSeidel, 1 : TAUCS, 2 : PARDISO)

};

} //namespace CGT

#ifdef LINSOLV
#include "FlowBoundingSphereLinSolv.hpp"
#endif


#endif //FLOW_ENGINE

#endif
