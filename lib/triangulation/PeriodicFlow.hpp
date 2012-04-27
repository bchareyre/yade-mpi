#ifdef FLOW_ENGINE

#ifndef _PERIFLOWBOUNDINGSPHERE_H
#define _PERIFLOWBOUNDINGSPHERE_H

#include<yade/lib/triangulation/FlowBoundingSphere.hpp>//include after #define XVIEW
// #include "Timer.h"
// #include "PeriodicTesselation.h"
// #include "basicVTKwritter.hpp"
// #include "Timer.h"
// #include "Network.hpp"
// 
// #ifdef XVIEW
// #include "Vue3D.h" //FIXME implicit dependencies will look for this class (out of tree) even ifndef XVIEW
// #endif


namespace CGT{

	typedef CGT::FlowBoundingSphere<PeriFlowTesselation> PeriodicFlowBoundingSphere;
	class PeriodicFlow : public PeriodicFlowBoundingSphere
	{
		public:
// 		typedef PeriFlowTesselation 				Tesselation;
// 		typedef Tesselation::RTriangulation			RTriangulation;
// 		typedef RTriangulation::Vertex_handle                   Vertex_handle;
// 		typedef RTriangulation::Finite_cells_iterator		Finite_cells_iterator;
// 		typedef RTriangulation::Cell_handle			Cell_handle;
// 		typedef RTriangulation::Edge_iterator			Edge_iterator;
		
// 		using PeriodicFlowBoundingSphere::Tri;
			
		void Interpolate(Tesselation& Tes, Tesselation& NewTes);
		void ComputeFacetForcesWithCache();
		void Compute_Permeability();
		void Initialize_pressures( double P_zero );
		void GaussSeidel(Real dt=0);
		void DisplayStatistics();
		void Average_Relative_Cell_Velocity();
		void ComputeEdgesSurfaces();
	};
}

#ifdef LINSOLV
#include "PeriodicFlowLinSolv.hpp"
#endif


#endif //FLOW_ENGINE

#endif
