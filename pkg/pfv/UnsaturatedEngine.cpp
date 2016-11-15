/*************************************************************************
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*  Copyright (C) 2012 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE
#include "TwoPhaseFlowEngine.hpp"

//keep this #ifdef for commited versions unless you really have stable version that should be compiled by default
//it will save compilation time for everyone else
//when you want it compiled, you can pass -DTWOPHASEFLOW to cmake, or just uncomment the following line
#ifdef TWOPHASEFLOW

class UnsaturatedEngine : public TwoPhaseFlowEngine
{
		double totalCellVolume;
	protected:
// 		void initialization();		

	public :
		double computeCellInterfacialArea(CellHandle cell, int j, double rC);

// 		void computeSolidLine();
		void computeFacetPoreForcesWithCache(bool onlyCache=false);	
		void computeCapillaryForce() {computeFacetPoreForcesWithCache(false);}
		
		
		void invasion();
		///functions can be shared by two modes
		void invasionSingleCell(CellHandle cell);
		void updatePressure();
		double getMinDrainagePc();
		double getMaxImbibitionPc();
		double getSaturation(bool isSideBoundaryIncluded=false);
		double getSpecificInterfacialArea();

		void invasion1();
		void updateReservoirs1();
		void WResRecursion(CellHandle cell);
		void NWResRecursion(CellHandle cell);
		void checkTrap(double pressure);

		void invasion2();
		void updateReservoirs2();

		void saveVtk(const char* folder) {bool initT=solver->noCache; solver->noCache=false; solver->saveVtk(folder); solver->noCache=initT;}

		//record and test functions
		void checkLatticeNodeY(double y);
		//temporary functions
		void initializeCellWindowsID();
		double getWindowsSaturation(int i, bool isSideBoundaryIncluded=false);
		bool checknoCache() {return solver->noCache;}
		double getInvadeDepth();
		double getSphericalSubdomainSaturation(Vector3r pos, double radius);
		double getCuboidSubdomainSaturation(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded);
		double getCuboidSubdomainPorosity(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded);

		void printSomething();
		boost::python::list getPotentialPendularSpheresPair() {
			RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
			boost::python::list bridgeIds;
			FiniteEdgesIterator ed_it = Tri.finite_edges_begin();
			for ( ; ed_it!=Tri.finite_edges_end(); ed_it++ ) {
			  if (detectBridge(ed_it)==true) {
			    const VertexInfo& vi1=(ed_it->first)->vertex(ed_it->second)->info();
			    const VertexInfo& vi2=(ed_it->first)->vertex(ed_it->third)->info();
			    const int& id1 = vi1.id();
			    const int& id2 = vi2.id();
			    bridgeIds.append(boost::python::make_tuple(id1,id2));}}
			    return bridgeIds;}
  		bool detectBridge(RTriangulation::Finite_edges_iterator& edge);
				
		virtual ~UnsaturatedEngine();

		virtual void action();
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(UnsaturatedEngine,TwoPhaseFlowEngine,"Preliminary version engine of a drainage model for unsaturated soils. Note:Air reservoir is on the top; water reservoir is on the bottom.",

		((bool, computeForceActivated, true,,"Activate capillary force computation. WARNING: turning off means capillary force is not computed at all, but the drainage can still work."))
		((int, windowsNo, 10,, "Number of genrated windows(or zoomed samples)."))
		((bool, isDrainageActivated, true,, "Activates drainage."))
		((bool, isImbibitionActivated, false,, "Activates imbibition."))
					,,,
		.def("saveVtk",&UnsaturatedEngine::saveVtk,(boost::python::arg("folder")="./VTK"),"Save pressure field in vtk format. Specify a folder name for output.")
		.def("getMinDrainagePc",&UnsaturatedEngine::getMinDrainagePc,"Get the minimum entry capillary pressure for the next drainage step.")
		.def("getMaxImbibitionPc",&UnsaturatedEngine::getMaxImbibitionPc,"Get the maximum entry capillary pressure for the next imbibition step.")
		.def("getSaturation",&UnsaturatedEngine::getSaturation,(boost::python::arg("isSideBoundaryIncluded")),"Get saturation of entire packing. If isSideBoundaryIncluded=false (default), the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
		.def("getSpecificInterfacialArea",&UnsaturatedEngine::getSpecificInterfacialArea,"get specific interfacial area (defined as the amount of fluid-fluid interfacial area per unit volume pf the porous medium).")
		.def("invasion",&UnsaturatedEngine::invasion,"Run the drainage invasion.")
		.def("computeCapillaryForce",&UnsaturatedEngine::computeCapillaryForce,"Compute capillary force. ")
		.def("checkLatticeNodeY",&UnsaturatedEngine::checkLatticeNodeY,(boost::python::arg("y")),"Check the slice of lattice nodes for yNormal(y). 0: out of sphere; 1: inside of sphere.")
		.def("getInvadeDepth",&UnsaturatedEngine::getInvadeDepth,"Get NW-phase invasion depth. (the distance from NW-reservoir to front of NW-W interface.)")
		.def("getSphericalSubdomainSaturation",&UnsaturatedEngine::getSphericalSubdomainSaturation,(boost::python::arg("pos"),boost::python::arg("radius")),"Get saturation of spherical subdomain defined by (pos, radius). The subdomain exclude boundary pores.")
		.def("getCuboidSubdomainSaturation",&UnsaturatedEngine::getCuboidSubdomainSaturation,(boost::python::arg("pos1"),boost::python::arg("pos2"),boost::python::arg("isSideBoundaryIncluded")),"Get saturation of cuboid subdomain defined by (pos1,pos2). If isSideBoundaryIncluded=false, the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
		.def("getCuboidSubdomainPorosity",&UnsaturatedEngine::getCuboidSubdomainPorosity,(boost::python::arg("pos1"),boost::python::arg("pos2"),boost::python::arg("isSideBoundaryIncluded")),"Get the porosity of cuboid subdomain defined by (pos1,pos2). If isSideBoundaryIncluded=false, the pores of side boundary are excluded in porosity calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in porosity calculating.")
		.def("checknoCache",&UnsaturatedEngine::checknoCache,"check noCache. (temporary function.)")
		.def("getWindowsSaturation",&UnsaturatedEngine::getWindowsSaturation,(boost::python::arg("windowsID"),boost::python::arg("isSideBoundaryIncluded")), "get saturation of subdomain with windowsID. If isSideBoundaryIncluded=false (default), the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
		.def("initializeCellWindowsID",&UnsaturatedEngine::initializeCellWindowsID,"Initialize cell windows index. A temporary function for comparison with experiments, will delete soon")
		.def("printSomething",&UnsaturatedEngine::printSomething,"print debug.")
		.def("getPotentialPendularSpheresPair",&UnsaturatedEngine::getPotentialPendularSpheresPair,"Get the list of sphere ID pairs of potential pendular liquid bridge.")
		)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(UnsaturatedEngine);
YADE_PLUGIN((UnsaturatedEngine));

UnsaturatedEngine::~UnsaturatedEngine(){}

/*void UnsaturatedEngine::initialDrainage()
{
	cout<<"This is UnsaturatedEngine test program"<<endl;
	RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
	if (tri.number_of_vertices()==0) {
		cout<< "triangulation is empty: building a new one" << endl;
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(bndCondValue[2],*solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
		initialReservoirs();
		initializeCellIndex();//initialize cell index
		computePoreThroatRadius();//save all pore radii before drainage
		computeTotalPoresVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
		computePoreBodyVolume();//save capillary volume of all cells, for fast calculating saturation
		computeSolidLine();//save cell->info().solidLine[j][y]
	}
	solver->noCache = true;
}*/

void UnsaturatedEngine::action()
{
/*
  //the drainage is in quasi-static regime, so it can work outside Omega.  
    if ( !isActivated ) return;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if ( (tri.number_of_vertices()==0) || (updateTriangulation) ) {
        cout<< "triangulation is empty: building a new one" << endl;
        scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
        setPositionsBuffer(true);//copy sphere positions in a buffer...
        buildTriangulation(bndCondValue[2],*solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
	initialReservoirs();
        initializeCellIndex();//initialize cell index
        computePoreThroatRadius();//save all pore radii before drainage
        computeTotalPoresVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
        computePoreBodyVolume();//save capillary volume of all cells, for calculating saturation
        computeSolidLine();//save cell->info().solidLine[j][y]
        solver->noCache = true;
    }
    ///compute drainage
    if (pressureForce) { drainage();}
    
    ///compute force
    if(computeForceActivated){
    computeCapillaryForce();
    Vector3r force;
    FiniteVerticesIterator vertices_end = solver->T[solver->currentTes].Triangulation().finite_vertices_end();
    for ( FiniteVerticesIterator V_it = solver->T[solver->currentTes].Triangulation().finite_vertices_begin(); V_it !=  vertices_end; V_it++ ) {
        force = pressureForce ? Vector3r ( V_it->info().forces[0],V_it->info().forces[1],V_it->info().forces[2] ): Vector3r(0,0,0);
        scene->forces.addForce ( V_it->info().id(), force); }}
*/}

void UnsaturatedEngine::updatePressure()
{
    boundaryConditions(*solver);
    solver->pressureChanged=true;
    solver->reApplyBoundaryConditions();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().isWRes==true) {cell->info().p()=bndCondValue[2];}
      if (cell->info().isNWRes==true) {cell->info().p()=bndCondValue[3];}
      if (isPhaseTrapped) {
	if ( cell->info().isTrapW ) {cell->info().p()=bndCondValue[3]-cell->info().trapCapP;}
	if ( cell->info().isTrapNW) {cell->info().p()=bndCondValue[2]+cell->info().trapCapP;}
	//check cell reservoir info.
	if ( !cell->info().isWRes && !cell->info().isNWRes && !cell->info().isTrapW && !cell->info().isTrapNW ) {cerr<<"ERROR! NOT FIND Cell Info!";}
// 	{cell->info().p()=bndCondValue[2]; if (isInvadeBoundary) cerr<<"Something wrong in updatePressure.(isInvadeBoundary)";}
      }
    } 
}

void UnsaturatedEngine::invasion()
{
    if (isPhaseTrapped) invasion1();
    else invasion2();
}

///mode1 and mode2 can share the same invasionSingleCell(), invasionSingleCell() ONLY change neighbor pressure and neighbor saturation, independent of reservoirInfo.
void UnsaturatedEngine::invasionSingleCell(CellHandle cell)
{
    double localPressure=cell->info().p();
    double localSaturation=cell->info().saturation;
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;//FIXME:defensive
//         if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
	if (cell->info().poreThroatRadius[facet]<0) continue;

	if ( (nCell->info().saturation==localSaturation) && (nCell->info().p() != localPressure) && ((nCell->info().isTrapNW)||(nCell->info().isTrapW)) ) {
	  nCell->info().p() = localPressure;
	  if(solver->debugOut) {cerr<<"merge trapped phase"<<endl;}
	  invasionSingleCell(nCell);} ///here we merge trapped phase back to reservoir 
	else if ( (nCell->info().saturation>localSaturation) ) {
	  double nPcThroat=surfaceTension/cell->info().poreThroatRadius[facet];
	  double nPcBody=surfaceTension/nCell->info().poreBodyRadius;
	  if( (localPressure-nCell->info().p()>nPcThroat) && (localPressure-nCell->info().p()>nPcBody) ) {
	    nCell->info().p() = localPressure;
	    nCell->info().saturation=localSaturation;
	    nCell->info().hasInterface=false;
	    if(solver->debugOut) {cerr<<"drainage"<<endl;}
	    if (recursiveInvasion) invasionSingleCell(nCell);
	  }
////FIXME:Introduce cell.hasInterface	  
// 	  else if( (localPressure-nCell->info().p()>nPcThroat) && (localPressure-nCell->info().p()<nPcBody) && (cell->info().hasInterface==false) && (nCell->info().hasInterface==false) ) {
// 	    if(solver->debugOut) {cerr<<"invasion paused into pore interface "<<endl;}
// 	    nCell->info().hasInterface=true;
// 	  }
// 	  else continue;
	}
	else if ( (nCell->info().saturation<localSaturation) ) {
	  double nPcThroat=surfaceTension/cell->info().poreThroatRadius[facet];
	  double nPcBody=surfaceTension/nCell->info().poreBodyRadius;
	  if( (nCell->info().p()-localPressure<nPcBody) && (nCell->info().p()-localPressure<nPcThroat) ) {
	    nCell->info().p() = localPressure;
	    nCell->info().saturation=localSaturation;
	    if(solver->debugOut) {cerr<<"imbibition"<<endl;}
	    if (recursiveInvasion) invasionSingleCell(nCell);
	  }
//// FIXME:Introduce cell.hasInterface	  
// 	  else if ( (nCell->info().p()-localPressure<nPcBody) && (nCell->info().p()-localPressure>nPcThroat) /*&& (cell->info().hasInterface==false) && (nCell->info().hasInterface==false)*/ ) {
// 	    nCell->info().p() = localPressure;
// 	    nCell->info().saturation=localSaturation;
// 	    if(solver->debugOut) {cerr<<"imbibition paused pore interface"<<endl;}
// 	    nCell->info().hasInterface=true;
// 	  }
// 	  else continue;
	}
	else continue;
    }
}
///invasion mode 1: withTrap
void UnsaturatedEngine::invasion1()
{
    if(solver->debugOut) {cout<<"----start invasion1----"<<endl;}

    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    if(solver->debugOut) {cout<<"----invasion1.updatePressure----"<<endl;}
    
    ///invasionSingleCell by Pressure difference, change Pressure and Saturation.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    if(isDrainageActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isNWRes)
                invasionSingleCell(cell);
        }
    }
    if(isImbibitionActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isWRes)
                invasionSingleCell(cell);
        }
    }
    if(solver->debugOut) {cout<<"----invasion1.invasionSingleCell----"<<endl;}

    ///update W, NW reservoirInfo according to cell->info().saturation
    updateReservoirs1();
    if(solver->debugOut) {cout<<"----invasion1.update W, NW reservoirInfo----"<<endl;}
    
    ///search new trapped W-phase/NW-phase, assign trapCapP, isTrapW/isTrapNW flag for new trapped phases. But at this moment, the new trapped W/NW cells.P= W/NW-Res.P. They will be updated in next updatePressure() func.
    checkTrap(bndCondValue[3]-bndCondValue[2]);
    if(solver->debugOut) {cout<<"----invasion1.checkWTrap----"<<endl;}

    ///update trapped W-phase/NW-phase Pressure //FIXME: is this necessary?
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
 	if ( cell->info().isTrapW ) {cell->info().p()=bndCondValue[3]-cell->info().trapCapP;}
	if ( cell->info().isTrapNW) {cell->info().p()=bndCondValue[2]+cell->info().trapCapP;}
   }
    if(solver->debugOut) {cout<<"----invasion1.update trapped W-phase/NW-phase Pressure----"<<endl;}
    
    if(isCellLabelActivated) updateCellLabel();
    if(solver->debugOut) {cout<<"----update cell labels----"<<endl;}
}

///search trapped W-phase or NW-phase, define trapCapP=Pn-Pw. assign isTrapW/isTrapNW info.
void UnsaturatedEngine::checkTrap(double pressure)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
//       if( (cell->info().isFictious) && (!cell->info().Pcondition) && (!isInvadeBoundary) ) continue;
      if( (cell->info().isWRes) || (cell->info().isNWRes) || (cell->info().isTrapW) || (cell->info().isTrapNW) ) continue;
      cell->info().trapCapP=pressure;
      if(cell->info().saturation==1.0) cell->info().isTrapW=true;
      if(cell->info().saturation==0.0) cell->info().isTrapNW=true;
    }
}

void UnsaturatedEngine::updateReservoirs1()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if(cell->info().Pcondition) continue;
        cell->info().isWRes = false;
        cell->info().isNWRes = false;
    }
    
    for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
        if ((*it)==NULL) continue;
        WResRecursion(*it);
    }
    
    for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
        if ((*it)==NULL) continue;
        NWResRecursion(*it);
    }
}

void UnsaturatedEngine::WResRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;
//         if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().saturation != 1.0) continue;
        if (nCell->info().isWRes==true) continue;
        nCell->info().isWRes = true;
        nCell->info().isNWRes = false;
        nCell->info().isTrapW = false;
	nCell->info().trapCapP=0.0;	
        WResRecursion(nCell);
    }
}

void UnsaturatedEngine::NWResRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;
//         if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().saturation != 0.0) continue;
        if (nCell->info().isNWRes==true) continue;
        nCell->info().isNWRes = true;
        nCell->info().isWRes = false;
        nCell->info().isTrapNW = false;
	nCell->info().trapCapP=0.0;	
        NWResRecursion(nCell);
    }
}

///invasion mode 2: withoutTrap
void UnsaturatedEngine::invasion2()
{
    if(solver->debugOut) {cout<<"----start invasion2----"<<endl;}

    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    if(solver->debugOut) {cout<<"----invasion2.updatePressure----"<<endl;}
    
    ///drainageSingleCell by Pressure difference, change Pressure and Saturation.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    if(isDrainageActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isNWRes)
                invasionSingleCell(cell);
        }
    }
    ///drainageSingleCell by Pressure difference, change Pressure and Saturation.
    if(isImbibitionActivated) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if(cell->info().isWRes)
                invasionSingleCell(cell);
        }
    }

    if(solver->debugOut) {cout<<"----invasion2.invasionSingleCell----"<<endl;}
    
    ///update W, NW reservoirInfo according to Pressure
    updateReservoirs2();
    if(solver->debugOut) {cout<<"----drainage2.update W, NW reservoirInfo----"<<endl;}    
    
}

void UnsaturatedEngine::updateReservoirs2()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().p()==bndCondValue[2]) {cell->info().isWRes=true; cell->info().isNWRes=false;}
        else if (cell->info().p()==bndCondValue[3]) {cell->info().isNWRes=true; cell->info().isWRes=false;}
        else {cerr<<"drainage mode2: updateReservoir Error!"<<endl;}
    }
}

double UnsaturatedEngine::getMinDrainagePc()
{
    double nextEntry = 1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().isNWRes == true) {
            for (int facet=0; facet<4; facet ++) {
	      CellHandle nCell = cell->neighbor(facet);
	      if (tri.is_infinite(nCell)) continue;
                if (nCell->info().Pcondition) continue;
//                 if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
                if ( nCell->info().isWRes == true && cell->info().poreThroatRadius[facet]>0) {
                    double nCellP = std::max( (surfaceTension/cell->info().poreThroatRadius[facet]),(surfaceTension/nCell->info().poreBodyRadius) );
//                     double nCellP = surfaceTension/cell->info().poreThroatRadius[facet];
                    nextEntry = std::min(nextEntry,nCellP);}}}}
                    
    if (nextEntry==1e50) {
        cout << "End drainage !" << endl;
        return nextEntry=0;
    }
    else return nextEntry;
}

double UnsaturatedEngine::getMaxImbibitionPc()
{
    double nextEntry = -1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().isWRes == true) {
            for (int facet=0; facet<4; facet ++) {
 	      CellHandle nCell = cell->neighbor(facet);
               if (tri.is_infinite(nCell)) continue;
                if (nCell->info().Pcondition) continue;
//                 if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
                if ( nCell->info().isNWRes == true && cell->info().poreThroatRadius[facet]>0) {
                    double nCellP = std::min( (surfaceTension/nCell->info().poreBodyRadius), (surfaceTension/cell->info().poreThroatRadius[facet]));
                    nextEntry = std::max(nextEntry,nCellP);}}}}
                    
    if (nextEntry==-1e50) {
        cout << "End imbibition !" << endl;
        return nextEntry=0;
    }
    else return nextEntry;
}

double UnsaturatedEngine::getSaturation(bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    double poresVolume = 0.0; //total pores volume
    double wVolume = 0.0; 	//NW-phase volume
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
        poresVolume = poresVolume + cell->info().poreBodyVolume;
        if (cell->info().saturation>0.0) {
            wVolume = wVolume + cell->info().poreBodyVolume * cell->info().saturation;
        }
    }
    return wVolume/poresVolume;
}

double UnsaturatedEngine::getSpecificInterfacialArea()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    double interfacialArea=0;

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
//             if (cell->info().Pcondition==true) continue;//NOTE:reservoirs cells interfacialArea should not be included.
            if(cell->info().isFictious) continue;
            if (cell->info().isNWRes==true) {
                for (int facet = 0; facet < 4; facet ++) {
                    if (tri.is_infinite(cell->neighbor(facet))) continue;
                    if (cell->neighbor(facet)->info().Pcondition==true) continue;
                    if ( (cell->neighbor(facet)->info().isFictious) && (!isInvadeBoundary) ) continue;
                    if (cell->neighbor(facet)->info().isNWRes==false)
                        interfacialArea = interfacialArea + computeCellInterfacialArea(cell, facet, cell->info().poreThroatRadius[facet]);}}}
//     cerr<<"InterArea:"<<interfacialArea<<"  totalCellVolume:"<<totalCellVolume<<endl;
    return interfacialArea/totalCellVolume;
}

double UnsaturatedEngine::computeCellInterfacialArea(CellHandle cell, int j, double rC)
{
    double rInscribe = std::abs(solver->computeEffectiveRadius(cell, j));  
    CellHandle cellh = CellHandle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
    
    if(facetNFictious==0) {
        RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
        if (tri.is_infinite(cell->neighbor(j))) return 0;

        Vector3r pos[3]; //solid pos
        double r[3]; //solid radius
        double rRc[3]; //r[i] + rC (rC: capillary radius)
        double e[3]; //edges of triangulation
        double rad[4][3]; //angle in radian

        for (int i=0; i<3; i++) {
            pos[i] = makeVector3r(cell->vertex(facetVertices[j][i])->point().point());
            r[i] = sqrt(cell->vertex(facetVertices[j][i])->point().weight());
            rRc[i] = r[i]+rC;
        }

        e[0] = (pos[1]-pos[2]).norm();
        e[1] = (pos[2]-pos[0]).norm();
        e[2] = (pos[1]-pos[0]).norm();

        rad[3][0]=acos(((pos[1]-pos[0]).dot(pos[2]-pos[0]))/(e[2]*e[1]));
        rad[3][1]=acos(((pos[2]-pos[1]).dot(pos[0]-pos[1]))/(e[0]*e[2]));
        rad[3][2]=acos(((pos[0]-pos[2]).dot(pos[1]-pos[2]))/(e[1]*e[0]));

        rad[0][0]=computeTriRadian(e[0],rRc[1],rRc[2]);
        rad[0][1]=computeTriRadian(rRc[2],e[0],rRc[1]);
        rad[0][2]=computeTriRadian(rRc[1],rRc[2],e[0]);

        rad[1][0]=computeTriRadian(rRc[2],e[1],rRc[0]);
        rad[1][1]=computeTriRadian(e[1],rRc[0],rRc[2]);
        rad[1][2]=computeTriRadian(rRc[0],rRc[2],e[1]);

        rad[2][0]=computeTriRadian(rRc[1],e[2],rRc[0]);
        rad[2][1]=computeTriRadian(rRc[0],rRc[1],e[2]);
        rad[2][2]=computeTriRadian(e[2],rRc[0],rRc[1]);

        double sW0=0.5*rRc[1]*rRc[2]*sin(rad[0][0])-0.5*rad[0][0]*pow(rC,2)-0.5*rad[0][1]*pow(r[1],2)-0.5*rad[0][2]*pow(r[2],2) ;
        double sW1=0.5*rRc[2]*rRc[0]*sin(rad[1][1])-0.5*rad[1][1]*pow(rC,2)-0.5*rad[1][2]*pow(r[2],2)-0.5*rad[1][0]*pow(r[0],2) ;
        double sW2=0.5*rRc[0]*rRc[1]*sin(rad[2][2])-0.5*rad[2][2]*pow(rC,2)-0.5*rad[2][0]*pow(r[0],2)-0.5*rad[2][1]*pow(r[1],2) ;
        double sW=sW0+sW1+sW2;
        double sVoid=sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
        double sInterface=sVoid-sW;
	return sInterface;
    }
    else {
        return Mathr::PI*pow(rInscribe,2);
    }
}

// ------------------for checking----------
void UnsaturatedEngine::checkLatticeNodeY(double y)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if((y<solver->yMin)||(y>solver->yMax)) {
        cerr<<"y is out of range! "<<"pleas set y between "<<solver->yMin<<" and "<<solver->yMax<<endl;}
    else {
        int N=100;// the default Node number for each slice is 100X100
        ofstream file;
        std::ostringstream fileNameStream(".txt");
        fileNameStream << "LatticeNodeY_"<< y;
        std::string fileName = fileNameStream.str();
        file.open(fileName.c_str());
//     file << "#Slice Of LatticeNodes: 0: out of sphere; 1: inside of sphere  \n";
        double deltaX = (solver->xMax-solver->xMin)/N;
        double deltaZ = (solver->zMax-solver->zMin)/N;
        for (int j=0; j<N+1; j++) {
            for (int k=0; k<N+1; k++) {
                double x=solver->xMin+j*deltaX;
                double z=solver->zMin+k*deltaZ;
                int M=0;
                Vector3r LatticeNode = Vector3r(x,y,z);
                for (FiniteVerticesIterator V_it = tri.finite_vertices_begin(); V_it != tri.finite_vertices_end(); V_it++) {
                    if(V_it->info().isFictious) continue;
                    Vector3r SphereCenter = makeVector3r(V_it->point().point());
                    if ((LatticeNode-SphereCenter).squaredNorm() < V_it->point().weight()) {
                        M=1;
                        break;}}
                file << M;}
            file << "\n";}
        file.close();}
}

void UnsaturatedEngine::printSomething()
{

    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    FiniteEdgesIterator ed_it;
    for ( FiniteEdgesIterator ed_it = Tri.finite_edges_begin(); ed_it!=Tri.finite_edges_end();ed_it++ )
    {
      const VertexInfo& vi1=(ed_it->first)->vertex(ed_it->second)->info();
      const VertexInfo& vi2=(ed_it->first)->vertex(ed_it->third)->info();
      const int& id1 = vi1.id();
      const int& id2 = vi2.id();
      cerr<<id1<<" "<<id2<<endl;}
}

bool UnsaturatedEngine::detectBridge(RTriangulation::Finite_edges_iterator& edge)
{
	bool dryBridgeExist=true;
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	RTriangulation::Cell_circulator cell1 = Tri.incident_cells(*edge);
	RTriangulation::Cell_circulator cell0 = cell1++;
	if(cell0->info().saturation==1) {dryBridgeExist=false; return dryBridgeExist;}
	else {
	while (cell1!=cell0) {
	  if(cell1->info().saturation==1) {dryBridgeExist=false;break;}
	  else cell1++;}
	  return dryBridgeExist;
	}
}

//----------temporary functions for comparison with experiment-----------------------
void UnsaturatedEngine::initializeCellWindowsID()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        for (int i=1; i<(windowsNo+1); i++) {
            if ( (cell->info()[1]>(solver->yMin+(i-1)*(solver->yMax-solver->yMin)/windowsNo) ) && (cell->info()[1] < (solver->yMin+i*(solver->yMax-solver->yMin)/windowsNo)) )
            {cell->info().windowsID=i; break;}
        }
    }
}

double UnsaturatedEngine::getWindowsSaturation(int i, bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    double poresVolume = 0.0; //total pores volume
    double wVolume = 0.0; 	//W-phase volume
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
        if (cell->info().windowsID != i) continue;
        poresVolume = poresVolume + cell->info().poreBodyVolume;
        if (cell->info().saturation>0.0) {
            wVolume = wVolume + cell->info().poreBodyVolume * cell->info().saturation;
        }
    }
    return wVolume/poresVolume;
}

double UnsaturatedEngine::getCuboidSubdomainSaturation(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    double poresVolume = 0.0; //total pores volume
    double wVolume = 0.0; 	//W-phase volume
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
	if ( ((pos1[0]-cell->info()[0])*(pos2[0]-cell->info()[0])<0) && ((pos1[1]-cell->info()[1])*(pos2[1]-cell->info()[1])<0) && ((pos1[2]-cell->info()[2])*(pos2[2]-cell->info()[2])<0) ) {
	  poresVolume = poresVolume + cell->info().poreBodyVolume;
	  if (cell->info().saturation>0.0) {
	    wVolume = wVolume + cell->info().poreBodyVolume * cell->info().saturation;
	  }
	}
    }
    return wVolume/poresVolume;
}

double UnsaturatedEngine::getCuboidSubdomainPorosity(Vector3r pos1, Vector3r pos2, bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    double totalCellVolume = 0.0; 
    double totalVoidVolume = 0.0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
	if ( ((pos1[0]-cell->info()[0])*(pos2[0]-cell->info()[0])<0) && ((pos1[1]-cell->info()[1])*(pos2[1]-cell->info()[1])<0) && ((pos1[2]-cell->info()[2])*(pos2[2]-cell->info()[2])<0) ) {
	  totalCellVolume = totalCellVolume + std::abs( cell->info().volume() );
	  totalVoidVolume = totalVoidVolume + cell->info().poreBodyVolume;
	}
    }
    if(totalVoidVolume==0 || totalCellVolume==0) cerr<<"subdomain too small!"<<endl;
    return totalVoidVolume/totalCellVolume;
}

double UnsaturatedEngine::getInvadeDepth()
{
    double yPosMax=-1e50;
    double yPosMin=1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().isNWRes) {
	  yPosMax=std::max(yPosMax,cell->info()[1]);
	  yPosMin=std::min(yPosMin,cell->info()[1]);
        }
    }
    return std::abs(yPosMax-yPosMin);
}

double UnsaturatedEngine::getSphericalSubdomainSaturation(Vector3r pos, double radius)
{
    double poresVolume=0.0;
    double wVolume=0.0;
    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = Tri.finite_cells_end();
    for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
        Vector3r cellPos = makeVector3r(cell->info());
        double dist=(pos-cellPos).norm();
        if(dist>radius) continue;
        if(cell->info().isFictious) {
	  cerr<<"The radius of subdomain is too large, or the center of subdomain is out of packing. Please reset subdomain again."<<endl;
	  return -1;
        }
        poresVolume=poresVolume+cell->info().poreBodyVolume;
        if(cell->info().saturation>0.0) {
            wVolume=wVolume+cell->info().poreBodyVolume * cell->info().saturation;
        }
    }
    return wVolume/poresVolume;
}

//--------------end of comparison with experiment----------------------------

///compute forces
void UnsaturatedEngine::computeFacetPoreForcesWithCache(bool onlyCache)
{
    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    CVector nullVect(0,0,0);
    //reset forces
    if (!onlyCache) for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) v->info().forces=nullVect;
// 	#ifdef parallel_forces
// 	if (solver->noCache) {
// 		solver->perVertexUnitForce.clear(); solver->perVertexPressure.clear();
// 		solver->perVertexUnitForce.resize(solver->T[solver->currentTes].maxId+1);
// 		solver->perVertexPressure.resize(solver->T[solver->currentTes].maxId+1);}
// 	#endif
// 	CellHandle neighbourCell;
// 	VertexHandle mirrorVertex;
    CVector tempVect;
    //FIXME : Ema, be carefull with this (noCache), it needs to be turned true after retriangulation
    if (solver->noCache) {//WARNING:all currentTes must be solver->T[solver->currentTes], should NOT be solver->T[currentTes]
        for (FlowSolver::VCellIterator cellIt=solver->T[solver->currentTes].cellHandles.begin(); cellIt!=solver->T[solver->currentTes].cellHandles.end(); cellIt++) {
            CellHandle& cell = *cellIt;
            //reset cache
            for (int k=0; k<4; k++) cell->info().unitForceVectors[k]=nullVect;

            for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j))) {
                    const CVector& Surfk = cell->info().facetSurfaces[j];
                    //FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
                    //The ratio void surface / facet surface
                    Real area = sqrt(Surfk.squared_length());
                    if (area<=0) cerr <<"AREA <= 0!! AREA="<<area<<endl;
                    CVector facetNormal = Surfk/area;
                    const std::vector<CVector>& crossSections = cell->info().facetSphereCrossSections;
                    CVector fluidSurfk = cell->info().facetSurfaces[j]*cell->info().facetFluidSurfacesRatio[j];
                    /// handle fictious vertex since we can get the projected surface easily here
                    if (cell->vertex(j)->info().isFictious) {
                        Real projSurf=std::abs(Surfk[solver->boundary(cell->vertex(j)->info().id()).coordinate]);
                        tempVect=-projSurf*solver->boundary(cell->vertex(j)->info().id()).normal;
                        cell->vertex(j)->info().forces = cell->vertex(j)->info().forces+tempVect*cell->info().p();
                        //define the cached value for later use with cache*p
                        cell->info().unitForceVectors[j]=cell->info().unitForceVectors[j]+ tempVect;
                    }
                    /// Apply weighted forces f_k=sqRad_k/sumSqRad*f
                    CVector facetUnitForce = -fluidSurfk*cell->info().solidLine[j][3];
                    CVector facetForce = cell->info().p()*facetUnitForce;

                    for (int y=0; y<3; y++) {
                        cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces + facetForce*cell->info().solidLine[j][y];
                        //add to cached value
                        cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]+facetUnitForce*cell->info().solidLine[j][y];
                        //uncomment to get total force / comment to get only pore tension forces
                        if (!cell->vertex(facetVertices[j][y])->info().isFictious) {
                            cell->vertex(facetVertices[j][y])->info().forces = cell->vertex(facetVertices[j][y])->info().forces -facetNormal*cell->info().p()*crossSections[j][y];
                            //add to cached value
                            cell->info().unitForceVectors[facetVertices[j][y]]=cell->info().unitForceVectors[facetVertices[j][y]]-facetNormal*crossSections[j][y];
                        }
                    }
// 	#ifdef parallel_forces
// 	solver->perVertexUnitForce[cell->vertex(j)->info().id()].push_back(&(cell->info().unitForceVectors[j]));
// 	solver->perVertexPressure[cell->vertex(j)->info().id()].push_back(&(cell->info().p()));
// 	#endif
                }
        }
        solver->noCache=false;//cache should always be defined after execution of this function
    }
    if (onlyCache) return;

//     else {//use cached values when triangulation doesn't change
// 		#ifndef parallel_forces
    for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); cell++) {
        for (int yy=0; yy<4; yy++) cell->vertex(yy)->info().forces = cell->vertex(yy)->info().forces + cell->info().unitForceVectors[yy]*cell->info().p();
    }

//  		#else
// 		#pragma omp parallel for num_threads(ompThreads)
// 		for (int vn=0; vn<= solver->T[solver->currentTes].maxId; vn++) {
// 			VertexHandle& v = solver->T[solver->currentTes].vertexHandles[vn];
// 			const int& id =  v->info().id();
// 			CVector tf (0,0,0);
// 			int k=0;
// 			for (vector<const Real*>::iterator c = solver->perVertexPressure[id].begin(); c != solver->perVertexPressure[id].end(); c++)
// 				tf = tf + (*(solver->perVertexUnitForce[id][k++]))*(**c);
// 			v->info().forces = tf;
// 		}
// 		#endif
//     }
    if (solver->debugOut) {
        CVector totalForce = nullVect;
        for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v)	{
            if (!v->info().isFictious) totalForce = totalForce + v->info().forces;
            else if (solver->boundary(v->info().id()).flowCondition==1) totalForce = totalForce + v->info().forces;
        }
        cout << "totalForce = "<< totalForce << endl;
    }
}

//#########################################################
//         CONVECTIVE DRYING EXTENSION
//#########################################################

class PhaseCluster : public Serializable
{
  		double totalCellVolume;
	public :

				
		virtual ~PhaseCluster();
		vector<TwoPhaseFlowEngine::CellHandle> pores;
		TwoPhaseFlowEngine::RTriangulation* tri;

		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(PhaseCluster,Serializable,"Preliminary.",
		((int,label,-1,,"Unique label of this cluster, should be reflected in pores of this cluster."))
		((double,volume,0,,"cumulated volume of all pores."))
		((double,entryPc,0,,"smallest entry capillary pressure."))
		((int,entryPore,0,,"the pore of the cluster incident to the throat with smallest entry Pc."))
		((double,interfacialArea,0,,"interfacial area of the cluster"))
					,,,
		)
};

REGISTER_SERIALIZABLE(PhaseCluster);
YADE_PLUGIN((PhaseCluster));

PhaseCluster::~PhaseCluster(){}


class DryingEngine : public UnsaturatedEngine
{
	public :
		virtual ~DryingEngine();
		vector<shared_ptr<PhaseCluster> > clusters;
		
		boost::python::list pyClusters() {
			boost::python::list ret;
			for(vector<shared_ptr<PhaseCluster> >::iterator it=clusters.begin(); it!=clusters.end(); ++it) ret.append(*it);
			return ret;}
	
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DryingEngine,UnsaturatedEngine,"Extended TwoPhaseFlowEngine for application to convective drying.",
// 		((shared_ptr<PhaseCluster> , cluster,new PhaseCluster,,"The list of clusters"))
					,,,
		.def("getClusters",&DryingEngine::pyClusters/*,(boost::python::arg("folder")="./VTK")*/,"Save pressure field in vtk format. Specify a folder name for output.")
		)
		DECLARE_LOGGER;
};

DryingEngine::~DryingEngine(){};

REGISTER_SERIALIZABLE(DryingEngine);
YADE_PLUGIN((DryingEngine));



#endif //TWOPHASEFLOW
#endif //FLOW_ENGINE
