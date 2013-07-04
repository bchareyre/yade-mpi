/*************************************************************************
*  Copyright (C) 2010 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifdef FLOW_ENGINE


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
#include <omp.h>


namespace CGT
{

using namespace std;

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

PeriodicFlowLinSolv::~PeriodicFlowLinSolv()
{
}

PeriodicFlowLinSolv::PeriodicFlowLinSolv(): LinSolver() {}

int PeriodicFlowLinSolv::SetLinearSystem(Real dt)
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
// 		indices.resize(Tri.number_of_finite_cells()+1);
		///Ordered cells
		orderedCells.clear();
		const Finite_cells_iterator cell_end = Tri.finite_cells_end();
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
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
// 		indices.resize(maxindex+1);
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
// 		gsB.resize(ncols+1);
		T_cells.resize(ncols+1);
		T_nnz=0;}
	for (int kk=0; kk<ncols;kk++) T_b[kk]=0;
	///Ordered cells
	int nIndex=0; Cell_handle neighbour_cell;
	for (int i=0; i<ncols; i++)
	{
		Finite_cells_iterator& cell = orderedCells[i];
		///Non-ordered cells
// 	for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
// 		if (!cell->info().Pcondition && !cell->info().isGhost)
		{
			const int& index=cell->info().index;
			const Cell_Info& cInfo = cell->info();
			if (!isLinearSystemSet) {
				//Add diagonal term
				is[T_nnz] = index;
				js[T_nnz] = index;
				vs[T_nnz] = (cInfo.k_norm())[0]+ (cInfo.k_norm())[1]+ (cInfo.k_norm())[2]+ (cInfo.k_norm())[3];
				if (vs[T_nnz]<0) cerr<<"!!!! WTF !!!"<<endl;
				if (fluidBulkModulus>0) vs[T_nnz] += (1.f/(dt*fluidBulkModulus*cInfo.invVoidVolume()));
				T_nnz++;
			}
			for (int j=0; j<4; j++) {
				neighbour_cell = cell->neighbor(j);
				if (Tri.is_infinite(neighbour_cell)) continue;
				Cell_Info& nInfo = neighbour_cell->info();
				nIndex=nInfo.index;
				if (nIndex==index) {
					cerr<<"ERROR: nIndex==index, the cell is neighbour to itself"<< index<<endl;
					errorCode=3;}

				if (nInfo.Pcondition) T_b[index-1]+=cInfo.k_norm()[j]*nInfo.shiftedP();
				else {
					if (!isLinearSystemSet && index>nIndex) {
						is[T_nnz] = index;
						js[T_nnz] = nIndex;
						vs[T_nnz] = - (cInfo.k_norm())[j];
						if (vs[T_nnz]>0) cerr<<"!!!! WTF2 !!!"<<endl;
						T_nnz++;}
					if (nInfo.isGhost) T_b[index-1]+=cInfo.k_norm()[j]*nInfo.pShift();
				}
			}
		}
	}
	if (!isLinearSystemSet) {
		if (useSolver==1 || useSolver==2){
		#ifdef TAUCS_LIB
			clen.resize(ncols+1);
			T_jn.resize(ncols+1);
			T_A->colptr = &T_jn[0];
			T_ia.resize(T_nnz);
			T_A->rowind = &T_ia[0];
			T_A->flags = (TAUCS_DOUBLE | TAUCS_SYMMETRIC | TAUCS_LOWER);
			T_an.resize(T_nnz);
			T_A->values.d = &T_an[0];
			T_A->n      = ncols;
			T_A->m      = ncols;
			int i,j,k;
			for (j=0; j<ncols; j++) clen[j] = 0;
			for (k=0; k<T_nnz; k++) {
				i = is[k]-1; /* make it 1-based */
				j = js[k]-1; /* make it 1-based */
				(clen[j])++;
			}
			/* now compute column pointers */
			k = 0;
			for (j=0; j<ncols; j++) {
				int tmp;
				tmp =  clen[j];
				clen[j] = (T_A->colptr[j]) = k;
				k += tmp;
			}
			clen[ncols] = (T_A->colptr[ncols]) = k;

			/* now read matrix into data structure */
			for (k=0; k<T_nnz; k++) {
				i = is[k] - 1; /* make it 1-based */
				j = js[k] - 1; /* make it 1-based */
				assert(i < ncols);
				assert(j < ncols);
				(T_A->taucs_values)[clen[j]] = vs[k];
				(T_A->rowind)[clen[j]] = i;
				clen[j] ++;
			}
		#else
			cerr<<"yade compiled without Taucs, FlowEngine.useSolver="<< useSolver <<" not supported"<<endl;
		#endif //TAUCS_LIB
		} else if (useSolver==3){
		#ifdef EIGENSPARSE_LIB
// 			//here the matrix can be exported in in MatrixMarket format for experiments 
// 			static int mn=0;
// 			ofstream file; ofstream file2; ofstream file3;
// 			stringstream ss,ss2,ss3;
// 			ss<<"matrix"<<mn;
// 			ss3<<"matrixf2"<<mn;
// 			ss2<<"matrixf"<<mn++;
// 			file.open(ss.str().c_str());
// 			file2.open(ss2.str().c_str());
// 			file3.open(ss3.str().c_str());
// 			file <<"%%MatrixMarket matrix coordinate real symmetric"<<endl;
// 			file2 <<"%%MatrixMarket matrix coordinate real symmetric"<<endl;
// 			file3 <<"%%MatrixMarket matrix coordinate real symmetric"<<endl;
// 			file <<ncols<<" "<<ncols<<" "<<T_nnz<<" -1"<<endl;
// 			file2 <<ncols<<" "<<ncols<<" "<<T_nnz<<" -1"<<endl;
// 			file3 <<ncols<<" "<<ncols<<" "<<T_nnz<<" -1"<<endl;
			tripletList.clear(); tripletList.resize(T_nnz);
			for(int k=0;k<T_nnz;k++) {
				tripletList[k]=ETriplet(is[k]-1,js[k]-1,vs[k]);
// 				file<<is[k]-1<<" "<<js[k]-1<<" "<<vs[k]<<endl;
// 				if (is[k]==js[k]) file2<<is[k]-1<<" "<<js[k]-1<<" "<<1.0001*vs[k]<<endl;
// 				else file2<<is[k]-1<<" "<<js[k]-1<<" "<<vs[k]<<endl;
// 				if (is[k]==js[k]) file3<<is[k]-1<<" "<<js[k]-1<<" "<<1.00000001*vs[k]<<endl;
// 				else file3<<is[k]-1<<" "<<js[k]-1<<" "<<vs[k]<<endl;
			}
			A.resize(ncols,ncols);
			A.setFromTriplets(tripletList.begin(), tripletList.end());
// 			file << A;
// 			file.close();
		#else
			cerr<<"yade compiled without CHOLMOD, FlowEngine.useSolver="<< useSolver <<" not supported"<<endl;
		#endif
		}
		isLinearSystemSet=true;
	}
	return ncols;
}



/// For Gauss Seidel, we need the full matrix

int PeriodicFlowLinSolv::SetLinearSystemFullGS(Real dt)
{
	//WARNING : boundary conditions (Pcondition, p values) must have been set for a correct definition
	RTriangulation& Tri = T[currentTes].Triangulation();
	int n_cells=Tri.number_of_finite_cells();

	if (!isFullLinearSystemGSSet){
		T_cells.clear();
		T_index=0;//FIXME : no need to clear if we don't re-triangulate
		T_nnz=0;
		ncols=0;
		const Finite_cells_iterator cell_end = Tri.finite_cells_end();
		orderedCells.clear();
		T_cells.resize(n_cells+1);
		for (Finite_cells_iterator cell = Tri.finite_cells_begin(); cell != cell_end; cell++) {
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
			Cell_handle& cell = T_cells[i];
			///Non-ordered cells
			if (!cell->info().Pcondition && !cell->info().isGhost) {
				//Add diagonal term
				fullAvalues[cell->info().index][4] = 1.f/((cell->info().k_norm())[0]+ (cell->info().k_norm())[1]+ (cell->info().k_norm())[2]+ (cell->info().k_norm())[3] + (fluidBulkModulus>0? 1.f/(dt*fluidBulkModulus*cell->info().invVoidVolume()):0));
				//DUMP
// 				cout<< cell->info().index<<" "<< cell->info().index<<" "<<fullAvalues[cell->info().index][4] <<endl;
				T_nnz++;
				for (int j=0; j<4; j++) {
					Cell_handle neighbour_cell = cell->neighbor(j);
					const Cell_Info& nInfo = neighbour_cell->info();
					Cell_Info& cInfo = cell->info();
					if (Tri.is_infinite(neighbour_cell)) {
						fullAvalues[cInfo.index][j] = 0;
						//We point to the pressure of the adjacent cell. If the cell is ghost, then it has the index of the real one, and then the pointer is correct
						fullAcolumns[cInfo.index][j] = &gsP[0];
						continue;}
					if (!nInfo.Pcondition) {
						++T_nnz;
						fullAvalues[cInfo.index][j] = (cInfo.k_norm())[j];
						fullAcolumns[cInfo.index][j] = &gsP[nInfo.index];
						//DUMP
// 						cout<< cInfo.index<<" "<< nInfo.index<<" "<<fullAvalues[cInfo.index][j] <<endl;
						//if the adjacent cell is ghost, we account for the pressure shift in the RHS
						if (nInfo.isGhost){
							gsB[cInfo.index]+=cInfo.k_norm()[j]*nInfo.pShift();
						}
					} else {
						fullAvalues[cInfo.index][j] = 0;
						fullAcolumns[cInfo.index][j] = &gsP[0];
						gsB[cInfo.index]+=cInfo.k_norm()[j]*nInfo.shiftedP();
					}
				}
			}
		}
	} else for (int i=1; i<=ncols; i++)
	{
		Cell_handle& cell = T_cells[i];
		///Non-ordered cells
		if (!cell->info().Pcondition && !cell->info().isGhost) {
			for (int j=0; j<4; j++) {
				Cell_handle neighbour_cell = cell->neighbor(j);
				const Cell_Info& nInfo = neighbour_cell->info();
				Cell_Info& cInfo = cell->info();
				if (!nInfo.Pcondition) {
					if (nInfo.isGhost) gsB[cInfo.index]+=cInfo.k_norm()[j]*nInfo.pShift();
				} else gsB[cInfo.index]+=cInfo.k_norm()[j]*nInfo.shiftedP();
			}
		}
	}
	isFullLinearSystemGSSet=true;
	return ncols;
}

} //namespace CGT

#endif //FLOW_ENGINE