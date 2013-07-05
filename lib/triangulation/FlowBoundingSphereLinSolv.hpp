/*************************************************************************
*  Copyright (C) 2010 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef FLOW_ENGINE
#pragma once

#define EIGENSPARSE_LIB //comment this if CHOLMOD is not available
// #define TAUCS_LIB //comment this if TAUCS lib is not available, it will disable PARDISO lib as well

#ifdef EIGENSPARSE_LIB
	#include <eigen3/Eigen/Sparse>
	#include <eigen3/Eigen/SparseCore>
	#include <eigen3/Eigen/CholmodSupport>
#endif
#ifdef TAUCS_LIB
#define TAUCS_CORE_DOUBLE
#include <complex> //THIS ONE MUST ABSOLUTELY BE INCLUDED BEFORE TAUCS.H!
#include <stdlib.h>
#include <float.h>
extern "C" {
#include "taucs.h"
}
#endif

#include "FlowBoundingSphere.hpp"

///_____ Declaration ____

using namespace std;

namespace CGT {

template<class FlowType>
class FlowBoundingSphereLinSolv : public FlowType
{
public:
	DECLARE_TESSELATION_TYPES(FlowType)
	typedef typename FlowType::Tesselation	Tesselation;
	using FlowType::useSolver;
	using FlowType::T;
	using FlowType::currentTes;
	using FlowType::boundary;
	using FlowType::y_min_id;
	using FlowType::y_max_id;
	using FlowType::DEBUG_OUT;
	using FlowType::TOLERANCE;
	using FlowType::RELAX;
	using FlowType::fluidBulkModulus;
	using FlowType::reApplyBoundaryConditions;
	using FlowType::pressureChanged;
	using FlowType::computedOnce;

	//! TAUCS DECs
	vector<Finite_cells_iterator> orderedCells;
	bool isLinearSystemSet;
	bool isFullLinearSystemGSSet;
	bool areCellsOrdered;//true when orderedCells is filled, turn it false after retriangulation

	#ifdef EIGENSPARSE_LIB
	//Eigen's sparse matrix and solver
	Eigen::SparseMatrix<double> A;
	typedef Eigen::Triplet<double> ETriplet;
	std::vector<ETriplet> tripletList;//The list of non-zero components in Eigen sparse matrix
	Eigen::CholmodDecomposition<Eigen::SparseMatrix<double>, Eigen::Lower > eSolver;
	bool factorizedEigenSolver;
	void exportMatrix(const char* filename) {ofstream f; f.open(filename); f<<A; f.close();};
	void exportTriplets(const char* filename) {ofstream f; f.open(filename);
		for (int k=0; k<A.outerSize(); ++k)
		  	for (Eigen::SparseMatrix<double>::InnerIterator it(A,k); it; ++it) f<< it.row()<<" "<< it.col()<<" "<<it.value()<<endl; f.close();};
	//Multi-threading seems to work fine for Cholesky decomposition, but it fails for the solve phase in which -j1 is the fastest,
	//here we specify both thread numbers independently
	int numFactorizeThreads;
	int numSolveThreads;
	#endif

	#ifdef TAUCS_LIB
	taucs_ccs_matrix SystemMatrix;
	taucs_ccs_matrix* T_A;
	taucs_ccs_matrix* Fccs;
	void* F;//The taucs factor
	#endif

	
	int T_nnz;
	int ncols;
	int T_size;

	double pTime1, pTime2;
	int pTimeInt, pTime1N, pTime2N;

	double ZERO;
	vector<double> T_an;//(size*5);
	vector<int> T_jn;//(size+1);
	vector<int> T_ia;//(size*5);
	vector<double> T_f;//(size); // right-hand size vector object
	vector<Cell_handle> T_cells;//(size)
	int T_index;

	vector<double> T_b;
	vector<double> T_bv;
	vector <double> T_x, P_x;
	vector <double> bodv;
	vector <double> xodv;
	int*         perm;
	int*         invperm;
	bool pardisoInitialized;
	//! END TAUCS DECs


	//! Pardiso
	int*    ia;
	int*    ja;
	double*  a;
	int nnz;
	int mtype;        /* Real symmetric positive def. matrix */
	double* b;
	double* x;// the unknown vector to solve Ax=b
	int      nrhs;          /* Number of right hand sides. */
	void *pt[64];
	int      iparm[64];
	double   dparm[64];
	int      maxfct, mnum, phase, error, msglvl, solver;
	int      num_procs;
	char    *var;
	int      i;
	double   ddum;              /* Double dummy */
	int      idum;              /* Integer dummy. */
	//! end pardiso

	/// EXTERNAL_GS part
	vector<vector<double> > fullAvalues;//contains Kij's and 1/(sum Kij) in 5th value (for use in GuaussSeidel)
	vector<vector<double*> > fullAcolumns;//contains columns numbers
	vector<double> gsP;//a vector of pressures
	vector<double> gsdV;//a vector of dV
	vector<double> gsB;//a vector of dV

public:
	virtual ~FlowBoundingSphereLinSolv();
	FlowBoundingSphereLinSolv();

	///Linear system solve
	virtual int SetLinearSystem(Real dt);
	void VectorizedGaussSeidel(Real dt);
	virtual int SetLinearSystemFullGS(Real dt);
	
	int TaucsSolveTest();
	int TaucsSolve(Real dt);
	int PardisoSolveTest();
	int PardisoSolve(Real dt);
	int eigenSolve(Real dt);
	
	void CopyGsToCells();
	void CopyCellsToGs(Real dt);
	
	void CopyLinToCells();
	void CopyCellsToLin(Real dt);
	void swap_fwd (double* v, int i);
	void swap_fwd (int* v, int i);
	void sort_v(int k1, int k2, int* is, double* ds);

	virtual void GaussSeidel (Real dt) {
		switch (useSolver) {
		case 0:
			VectorizedGaussSeidel(dt);
			break;
		case 1:
			TaucsSolve(dt);
			break;
		case 2:
			PardisoSolve(dt);
			break;
		case 3:
			eigenSolve(dt);
			break;
		}
		computedOnce=true;
	}
	virtual void ResetNetwork();
};

} //namespace CGT


///_____ Implementation ____

#include "FlowBoundingSphereLinSolv.ipp"

#endif
