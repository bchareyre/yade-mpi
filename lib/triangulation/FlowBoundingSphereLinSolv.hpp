/*************************************************************************
*  Copyright (C) 2010 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef FLOW_ENGINE
#pragma once

#define CHOLMOD_LIBS //comment this if CHOLMOD is not available
// #define TAUCS_LIB //comment this if TAUCS lib is not available, it will disable PARDISO lib as well

#ifdef CHOLMOD_LIBS
	#include <Eigen/Sparse>
	#include <Eigen/SparseCore>
	#include <Eigen/CholmodSupport>
	#include <cholmod.h>
#endif

#ifdef TAUCS_LIB
#define TAUCS_CORE_DOUBLE
#include <complex> //THIS ONE MUST ABSOLUTELY BE INCLUDED BEFORE TAUCS.H!
#include <stdlib.h>
#include <float.h>
//#include <time.h>
extern "C" {
#include "taucs.h"
}
#endif

#include "FlowBoundingSphere.hpp"

///_____ Declaration ____

namespace CGT {

template<class _Tesselation, class FlowType=FlowBoundingSphere<_Tesselation> >
class FlowBoundingSphereLinSolv : public FlowType
{
public:
	DECLARE_TESSELATION_TYPES(FlowType)
	typedef typename FlowType::Tesselation	Tesselation;
	using FlowType::useSolver;
	using FlowType::multithread;
	using FlowType::T;
	using FlowType::currentTes;
	using FlowType::boundary;
	using FlowType::yMinId;
	using FlowType::yMaxId;
	using FlowType::debugOut;
	using FlowType::tolerance;
	using FlowType::relax;
	using FlowType::fluidBulkModulus;
	using FlowType::reApplyBoundaryConditions;
	using FlowType::pressureChanged;
	using FlowType::computedOnce;
	using FlowType::resetNetwork;
	using FlowType::tesselation;
	using FlowType::resetRHS;
	using FlowType::factorizeOnly;  // used for backgroundAction()

	//! TAUCS DECs
	vector<FiniteCellsIterator> orderedCells;
	bool isLinearSystemSet;
	bool isFullLinearSystemGSSet;
	bool areCellsOrdered;//true when orderedCells is filled, turn it false after retriangulation
	bool updatedRHS;
	
	#ifdef CHOLMOD_LIBS
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
	#ifdef SUITESPARSE_VERSION_4
	// cholmod direct solver (useSolver=4)
	cholmod_factor* L;
	cholmod_sparse* Achol;
	cholmod_common com;
	bool factorExists;
	void add_T_entry(cholmod_triplet* T, long r, long c, double x)
	{
		size_t k = T->nnz;
		((long*)T->i)[k] = r;
		((long*)T->j)[k] = c;
		((double*)T->x)[k] = x;
		T->nnz++;
	}
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
	vector<CellHandle> T_cells;//(size)
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
	virtual int setLinearSystem(Real dt);
	void vectorizedGaussSeidel(Real dt);
	virtual int setLinearSystemFullGS(Real dt);
	
	int taucsSolveTest();
	int taucsSolve(Real dt);
	int pardisoSolveTest();
	int pardisoSolve(Real dt);
	int eigenSolve(Real dt);
	int cholmodSolve(Real dt);
	
	void copyGsToCells();
	void copyCellsToGs(Real dt);
	
	void copyLinToCells();
	void copyCellsToLin(Real dt);
	void swapFwd (double* v, int i);
	void swapFwd (int* v, int i);
	void sortV(int k1, int k2, int* is, double* ds);

	virtual void gaussSeidel (Real dt) {
		switch (useSolver) {
		case 0:
			vectorizedGaussSeidel(dt);
			break;
		case 1:
			taucsSolve(dt);
			break;
		case 2:
			pardisoSolve(dt);
			break;
		case 3:
			eigenSolve(dt);
			break;
		case 4:
			cholmodSolve(dt);
			break;
		}
		computedOnce=true;
	}
	virtual void resetLinearSystem();
	virtual void resetRHS() {updatedRHS=false;};
};

} //namespace CGT


///_____ Implementation ____

#include "FlowBoundingSphereLinSolv.ipp"

#endif
