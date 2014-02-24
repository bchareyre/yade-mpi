#ifdef FLOW_ENGINE

#pragma once

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
		void interpolate(Tesselation& Tes, Tesselation& NewTes);
		void computeFacetForcesWithCache(bool onlyCache=false);
		void computePermeability();
		void gaussSeidel(Real dt=0);
		void displayStatistics();
		void computeEdgesSurfaces();
		double boundaryFlux(unsigned int boundaryId);
		#ifdef EIGENSPARSE_LIB
		//Eigen's sparse matrix for forces computation
// 		Eigen::SparseMatrix<double> FIntegral;
// 		Eigen::SparseMatrix<double> PshiftsInts;
// 		Eigen::SparseMatrix<double> FRHS;
// 		Eigen::VectorXd forces;
		#endif
	};
}

#ifdef LINSOLV
#include "PeriodicFlowLinSolv.hpp"
#endif


#endif //FLOW_ENGINE
