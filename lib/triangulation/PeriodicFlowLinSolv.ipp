/*************************************************************************
*  Copyright (C) 2010 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef FLOW_ENGINE


#if CGAL_VERSION_NR < CGAL_VERSION_NUMBER(4,11,0)
	#include "CGAL/constructions/constructions_on_weighted_points_cartesian_3.h"
#endif
#include <CGAL/Width_3.h>
#include <iostream>
#include <fstream>
#include <new>
#include <utility>
#include "vector"
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef YADE_OPENMP
  #include <omp.h>
#endif

namespace CGT
{

#ifdef PARDISO
#ifdef AIX
#define F77_FUNC(func)  func
#else
#define F77_FUNC(func)  func ## _
#endif
/* PARDISO prototype. */
extern  "C" int F77_FUNC(pardisoinit)
    (void *, int *, int *, int *, double *, int *);

extern  "C" int F77_FUNC(pardiso)
    (void *, int *, int *, int *, int *, int *,
     double *, int *, int *, int *, int *, int *,
     int *, double *, double *, int *, double *);
#endif
template<class _Tesselation>
PeriodicFlowLinSolv<_Tesselation>::~PeriodicFlowLinSolv()
{
}
template<class _Tesselation>
PeriodicFlowLinSolv<_Tesselation>::PeriodicFlowLinSolv(): BaseFlowSolver() {}

template<class _Tesselation>
int PeriodicFlowLinSolv<_Tesselation>::setLinearSystem(Real dt)
{
//WARNING : boundary conditions (Pcondition, p values) must have been set for a correct definition of
	RTriangulation& Tri = T[currentTes].Triangulation();
	vector<int> clen;
	vector<int> is;
	vector<int> js;
	vector<double> vs;
	if (!areCellsOrdered) {
		T_nnz=0;
		ncols=0;
		T_cells.clear();
		T_index=0;
		unsigned int maxindex = 0;
		//FIXME: this is way too large since many cells will be ghosts
		T_cells.resize(Tri.number_of_finite_cells()+1);
		///Ordered cells
		orderedCells.clear();
		const FiniteCellsIterator cellEnd = Tri.finite_cells_end();
		for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
			if (cell->info().Pcondition || cell->info().isGhost) continue;
			orderedCells.push_back(cell);
// 			T_cells[++ncols]=cell;
// 			indices[cell->info().index]=ncols;
			++ncols;
			T_cells[cell->info().index]=cell;
			maxindex=max(maxindex,cell->info().index);
			}
// 		spatial_sort(orderedCells.begin(),orderedCells.end(), CellTraits_for_spatial_sort());//FIXME: ordering will not work with the new "indices", which needs reordering to
		T_cells.resize(ncols+1);
		isLinearSystemSet=false;
		areCellsOrdered=true;
	}
	if (!isLinearSystemSet) {
		int n = 3*(ncols+1);//number of non-zero in triangular matrix
		is.resize(n);
		js.resize(n);
		vs.resize(n);
		T_x.resize(ncols);
		T_b.resize(ncols);
		T_bv.resize(ncols);
		bodv.resize(ncols);
		xodv.resize(ncols);
		T_cells.resize(ncols+1);
		T_nnz=0;}
	for (int kk=0; kk<ncols;kk++) T_b[kk]=0;
	///Ordered cells
	int nIndex=0; CellHandle neighbourCell;
	for (int i=0; i<ncols; i++)
	{
		FiniteCellsIterator& cell = orderedCells[i];
		///Non-ordered cells
		{
			const int& index=cell->info().index;
			const CellInfo& cInfo = cell->info();
			if (!isLinearSystemSet) {
				//Add diagonal term
				is[T_nnz] = index;
				js[T_nnz] = index;
				vs[T_nnz] = (cInfo.kNorm())[0]+ (cInfo.kNorm())[1]+ (cInfo.kNorm())[2]+ (cInfo.kNorm())[3];
				if (vs[T_nnz]<0) cerr<<"!!!! WTF !!!"<<endl;
				if (fluidBulkModulus>0) vs[T_nnz] += (1.f/(dt*fluidBulkModulus*cInfo.invVoidVolume()));
				T_nnz++;
			}
			for (int j=0; j<4; j++) {
				neighbourCell = cell->neighbor(j);
				if (Tri.is_infinite(neighbourCell)) continue;
				CellInfo& nInfo = neighbourCell->info();
				nIndex=nInfo.index;
				if (nIndex==index) {
					cerr<<"ERROR: nIndex==index, the cell is neighbour to itself"<< index<<endl;
					errorCode=3;}

				if (nInfo.Pcondition) T_b[index-1]+=cInfo.kNorm()[j]*nInfo.shiftedP();
				else {
					if (!isLinearSystemSet && index>nIndex) {
						is[T_nnz] = index;
						js[T_nnz] = nIndex;
						vs[T_nnz] = - (cInfo.kNorm())[j];
						if (vs[T_nnz]>0) cerr<<"!!!! WTF2 !!!"<<endl;
						T_nnz++;}
					if (nInfo.isGhost) T_b[index-1]+=cInfo.kNorm()[j]*nInfo.pShift();
				}
			}
		}
	}
	if (!isLinearSystemSet) {
		if (useSolver>0){
		#ifdef CHOLMOD_LIBS
			tripletList.clear(); tripletList.resize(T_nnz);
			for(int k=0;k<T_nnz;k++) {
				tripletList[k]=ETriplet(is[k]-1,js[k]-1,vs[k]);
			}
			A.resize(ncols,ncols);
			A.setFromTriplets(tripletList.begin(), tripletList.end());
		#else
			cerr<<"yade compiled without CHOLMOD, FlowEngine.useSolver="<< useSolver <<">0 not supported"<<endl;
		#endif
		}
		isLinearSystemSet=true;
	}
	return ncols;
}



/// For Gauss Seidel, we need the full matrix
template<class _Tesselation>
int PeriodicFlowLinSolv<_Tesselation>::setLinearSystemFullGS(Real dt)
{
	//WARNING : boundary conditions (Pcondition, p values) must have been set for a correct definition
	RTriangulation& Tri = T[currentTes].Triangulation();
	int n_cells=Tri.number_of_finite_cells();

	if (!isFullLinearSystemGSSet){
		T_cells.clear();
		T_index=0;//FIXME : no need to clear if we don't re-triangulate
		T_nnz=0;
		ncols=0;
		const FiniteCellsIterator cellEnd = Tri.finite_cells_end();
		orderedCells.clear();
		T_cells.resize(n_cells+1);
		for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
			if (cell->info().Pcondition || cell->info().isGhost) continue;
			++ncols;
			T_cells[cell->info().index]=cell;
		}
// 		spatial_sort(orderedCells.begin(),orderedCells.end(), CellTraits_for_spatial_sort());
		gsP.resize(ncols+1);
		gsB.resize(ncols+1);
		T_b.resize(ncols+1);
		gsdV.resize(ncols+1);
		fullAcolumns.resize(ncols+1);
		fullAvalues.resize(ncols+1);
		T_cells.resize(ncols+1);
		for (int k=0; k<=ncols;k++) {
			fullAcolumns[k].resize(4);
			fullAvalues[k].resize(5);
			gsdV[k]=0;
		}
		gsP[0]=0;
		areCellsOrdered=true;
	}
	for (int k=0; k<=ncols;k++) gsB[k]=0;
 	if (!isFullLinearSystemGSSet){
		///Ordered cells
		for (int i=1; i<=ncols; i++)
		{
			CellHandle& cell = T_cells[i];
			///Non-ordered cells
			if (!cell->info().Pcondition && !cell->info().isGhost) {
				//Add diagonal term
				fullAvalues[cell->info().index][4] = 1.f/((cell->info().kNorm())[0]+ (cell->info().kNorm())[1]+ (cell->info().kNorm())[2]+ (cell->info().kNorm())[3] + (fluidBulkModulus>0? 1.f/(dt*fluidBulkModulus*cell->info().invVoidVolume()):0));
				//DUMP
				T_nnz++;
				for (int j=0; j<4; j++) {
					CellHandle neighbourCell = cell->neighbor(j);
					const CellInfo& nInfo = neighbourCell->info();
					CellInfo& cInfo = cell->info();
					if (Tri.is_infinite(neighbourCell)) {
						fullAvalues[cInfo.index][j] = 0;
						//We point to the pressure of the adjacent cell. If the cell is ghost, then it has the index of the real one, and then the pointer is correct
						fullAcolumns[cInfo.index][j] = &gsP[0];
						continue;}
					if (!nInfo.Pcondition) {
						++T_nnz;
						fullAvalues[cInfo.index][j] = (cInfo.kNorm())[j];
						fullAcolumns[cInfo.index][j] = &gsP[nInfo.index];
						//DUMP
						//if the adjacent cell is ghost, we account for the pressure shift in the RHS
						if (nInfo.isGhost){
							gsB[cInfo.index]+=cInfo.kNorm()[j]*nInfo.pShift();
						}
					} else {
						fullAvalues[cInfo.index][j] = 0;
						fullAcolumns[cInfo.index][j] = &gsP[0];
						gsB[cInfo.index]+=cInfo.kNorm()[j]*nInfo.shiftedP();
					}
				}
			}
		}
	} else for (int i=1; i<=ncols; i++)
	{
		CellHandle& cell = T_cells[i];
		///Non-ordered cells
		if (!cell->info().Pcondition && !cell->info().isGhost) {
			for (int j=0; j<4; j++) {
				CellHandle neighbourCell = cell->neighbor(j);
				const CellInfo& nInfo = neighbourCell->info();
				CellInfo& cInfo = cell->info();
				if (!nInfo.Pcondition) {
					if (nInfo.isGhost) gsB[cInfo.index]+=cInfo.kNorm()[j]*nInfo.pShift();
				} else gsB[cInfo.index]+=cInfo.kNorm()[j]*nInfo.shiftedP();
			}
		}
	}
	isFullLinearSystemGSSet=true;
	return ncols;
}

} //namespace CGT

#endif //FLOW_ENGINE
