 
/*************************************************************************
*  Copyright (C) 2014 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2013 by T. Sweijen (T.sweijen@uu.nl)                    *
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

// This is an example of how to derive a new FlowEngine with additional data and possibly completely new behaviour.
// Every functions of the base engine can be overloaded, and new functions can be added

//keep this #ifdef as long as you don't really want to realize a final version publicly, it will save compilation time for everyone else
//when you want it compiled, you can pass -DDFNFLOW to cmake, or just uncomment the following line
#define TWOPHASEFLOW
#ifdef TWOPHASEFLOW

#include "TwoPhaseFlowEngine.hpp"

YADE_PLUGIN((TwoPhaseFlowEngineT));
YADE_PLUGIN((TwoPhaseFlowEngine));

void TwoPhaseFlowEngine::fancyFunction(Real what) {std::cerr<<"yes, I'm a new function"<<std::endl;}

void TwoPhaseFlowEngine::initializeCellIndex()
{
    int k=0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        cell->info().index=k++;}
}

void TwoPhaseFlowEngine::computePoreBodyVolume()
{
    initializeVolumes(*solver);
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    CellHandle neighbourCell;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        cell->info().poreBodyVolume = std::abs( cell->info().volume() ) - solver->volumeSolidPore(cell);
    }
}

#endif //TwoPhaseFLOW
 
