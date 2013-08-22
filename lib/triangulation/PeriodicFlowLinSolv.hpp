/*************************************************************************
*  Copyright (C) 2010 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include "FlowBoundingSphere.hpp"

#ifdef FLOW_ENGINE

using namespace std;

namespace CGT {

typedef FlowBoundingSphereLinSolv<PeriodicFlow> LinSolver;

class PeriodicFlowLinSolv : public LinSolver
{
public:
	typedef PeriodicFlow	FlowType;
	vector<int> indices;//redirection vector containing the rank of cell so that T_cells[indices[cell->info().index]]=cell

	virtual ~PeriodicFlowLinSolv();
	PeriodicFlowLinSolv();

	///Linear system solve
	virtual int SetLinearSystem(Real dt=0);
	virtual int SetLinearSystemFullGS(Real dt=0);
};

} //namespace CGTF

#endif //FLOW_ENGINE
