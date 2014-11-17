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
// 		void initialReservoirs();///only used for determining first entry pressure
// 		void initializeCellIndex();
// 		void initializeReservoirs();
		void computeTotalPoresVolume();
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
// 		void updataPoreSaturation();

		void invasion1();
		void updateReservoirs1();
// 		void initialWResBound();
// 		void initialNWResBound();
		void WResRecursion(CellHandle cell);
		void NWResRecursion(CellHandle cell);
		void checkTrap(double pressure);

		void invasion2();
		void updateReservoirs2();

		void saveVtk(const char* folder) {bool initT=solver->noCache; solver->noCache=false; solver->saveVtk(folder); solver->noCache=initT;}
		void savePhaseVtk(const char* folder);

		//record and test functions
		void checkCellsConnection();
		void checkEntryCapillaryPressure();
		void checkLatticeNodeY(double y); 
		void checkReservoirInfo(int boundN);
		void checkBoundingCellsInfo();
		//temp functions
		void initializeCellWindowsID();
		double getWindowsSaturation(int i, bool isSideBoundaryIncluded=false);
		bool checknoCache() {return solver->noCache;}
		double getInvadeDepth();
		double getSubdomainSaturation(Vector3r pos, double radius);
		
		double getRMin(CellHandle cell, int j);
		double getRMax(CellHandle cell, int j);
		void checkRCompare();
		void checkPoreRadius();
		void printSomething();
				
		virtual ~UnsaturatedEngine();

		virtual void action();
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(UnsaturatedEngine,TwoPhaseFlowEngine,"Preliminary version engine of a drainage model for unsaturated soils. Note:Air reservoir is on the top; water reservoir is on the bottom.",

					((bool, computeForceActivated, true,,"Activate capillary force computation. WARNING: turning off means capillary force is not computed at all, but the drainage can still work."))
					((bool, isInvadeBoundary, true,,"Invasion side boundary condition. If True, pores of side boundary can be invaded; if False, the pore throats connecting side boundary are closed, those pores are excluded in saturation calculation."))
					((int, windowsNo, 10,, "Number of genrated windows(or zoomed samples)."))
// 					((bool, drainageFirst, true,,"If true, activate drainage first (initial saturated), then imbibition; if false, activate imbibition first (initial unsaturated), then drainage."))
					,,,
					.def("saveVtk",&UnsaturatedEngine::saveVtk,(boost::python::arg("folder")="./VTK"),"Save pressure field in vtk format. Specify a folder name for output.")
					.def("savePhaseVtk",&UnsaturatedEngine::savePhaseVtk,(boost::python::arg("folder")="./phaseVtk"),"Save phases information in vtk format. W-phase=1, NW-phase=0, which can also be seen as saturation of local pore. Specify a folder name for output.")
					.def("getMinDrainagePc",&UnsaturatedEngine::getMinDrainagePc,"Get the minimum entry capillary pressure for the next drainage step.")
					.def("getMaxImbibitionPc",&UnsaturatedEngine::getMaxImbibitionPc,"Get the maximum entry capillary pressure for the next imbibition step.")
					.def("getSaturation",&UnsaturatedEngine::getSaturation,(boost::python::arg("isSideBoundaryIncluded")),"Get saturation of entire packing. If isSideBoundaryIncluded=false (default), the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
					.def("getSpecificInterfacialArea",&UnsaturatedEngine::getSpecificInterfacialArea,"get specific interfacial area (defined as the amount of fluid-fluid interfacial area per unit volume pf the porous medium).")
					.def("invasion",&UnsaturatedEngine::invasion,"Run the drainage invasion.")
					.def("computeCapillaryForce",&UnsaturatedEngine::computeCapillaryForce,"Compute capillary force. ")

					.def("checkCellsConnection",&UnsaturatedEngine::checkCellsConnection,"Check cell connections.")
					.def("checkEntryCapillaryPressure",&UnsaturatedEngine::checkEntryCapillaryPressure,"Check entry capillary pressure between neighbor cells.")
					.def("checkLatticeNodeY",&UnsaturatedEngine::checkLatticeNodeY,(boost::python::arg("y")),"Check the slice of lattice nodes for yNormal(y). 0: out of sphere; 1: inside of sphere.")
					.def("checkReservoirInfo",&UnsaturatedEngine::checkReservoirInfo,(boost::python::arg("boundN")),"Check reservoir cells(N=2,3) states and export to 'waterReservoirBoundInfo.txt' and 'airReservoirBoundInfo.txt'.")
					.def("checkBoundingCellsInfo",&UnsaturatedEngine::checkBoundingCellsInfo,"Check boundary cells (without reservoirs) states and export to 'boundInfo.txt'.")
					.def("getInvadeDepth",&UnsaturatedEngine::getInvadeDepth,"Get NW-phase invasion depth. (the distance from NW-reservoir to front of NW-W interface.)")
					.def("getSubdomainSaturation",&UnsaturatedEngine::getSubdomainSaturation,(boost::python::arg("pos"),boost::python::arg("radius")),"Get saturation of spherical subdomain defined by (pos, radius). The subdomain exclude boundary pores.")
					
// 					.def("initialization",&UnsaturatedEngine::initialization,"Initialize drainage setup. Build network, compute pore geometry info and initialize reservoir boundary conditions. ")
					.def("checknoCache",&UnsaturatedEngine::checknoCache,"check noCache. (temp func.)")
					.def("getWindowsSaturation",&UnsaturatedEngine::getWindowsSaturation,(boost::python::arg("windowsID"),boost::python::arg("isSideBoundaryIncluded")), "get saturation of subdomain with windowsID. If isSideBoundaryIncluded=false (default), the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
					.def("initializeCellWindowsID",&UnsaturatedEngine::initializeCellWindowsID,"Initialize cell windows index. A temp function for comparison with experiments, will delete soon")
					.def("checkRCompare",&UnsaturatedEngine::checkRCompare,"debug R RMin RMax.")
					.def("checkPoreRadius",&UnsaturatedEngine::checkPoreRadius,"debug pore throat radius and pore body radius.")
					.def("printSomething",&UnsaturatedEngine::printSomething,"print debug.")
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
/*
void UnsaturatedEngine::initialDrainage()
{
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(bndCondValue[2],*solver);//create a triangulation and initialize pressure in the elements (connecting with W-reservoir), everything will be contained in "solver"
// 		initializeCellIndex();//initialize cell index
		computePoreThroatRadius();//save all pore radii before drainage
		computeTotalPoresVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
		computePoreBodyVolume();//save capillary volume of all cells, for fast calculating saturation
		computeSolidLine();//save cell->info().solidLine[j][y]
		initialReservoirs();
		solver->noCache = true;
}*/
/*
void UnsaturatedEngine::initialization()
{
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(0.0,*solver);//create a triangulation and initialize pressure in the elements (connecting with W-reservoir), everything will be contained in "solver"
// 		initializeCellIndex();//initialize cell index
		computePoreThroatRadius();//save pore throat radius before drainage
		computePoreBodyRadius();//save pore body radius before imbibition
		computeTotalPoresVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
		computePoreBodyVolume();//save capillary volume of all cells, for fast calculating saturation
		computeSolidLine();//save cell->info().solidLine[j][y]
		initializeReservoirs();//initial pressure, reservoir flags and local pore saturation
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

void UnsaturatedEngine::computeTotalPoresVolume()
{
    initializeVolumes(*solver);
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    totalCellVolume=0;
    
    if(isInvadeBoundary==true) {
        for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
            if (cell->info().Pcondition) continue;//NOTE:reservoirs cells should not be included in totalCellVolume
            totalCellVolume = totalCellVolume + std::abs( cell->info().volume() );}}
    else {
        for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
            if (cell->info().Pcondition) continue;//NOTE:reservoirs cells should not be included in totalCellVolume
            if (cell->info().isFictious) continue;
            totalCellVolume = totalCellVolume + std::abs( cell->info().volume() );}}
}
/*
void UnsaturatedEngine::initializeReservoirs()
{
    boundaryConditions(*solver);
    solver->pressureChanged=true;
    solver->reApplyBoundaryConditions();
    ///keep boundingCells[2] as W-reservoir.
    for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
        (*it)->info().isWRes = true;
        (*it)->info().isNWRes = false;
        (*it)->info().saturation=1.0;
    }
    ///keep boundingCells[3] as NW-reservoir.
    for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
        (*it)->info().isNWRes = true;
        (*it)->info().isWRes = false;
        (*it)->info().saturation=0.0;
    }

    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    ///if we start from drainage
    if(drainageFirst)
    {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().Pcondition) continue;
	    cell->info().p()=bndCondValue[2];
            cell->info().isWRes = true;
            cell->info().isNWRes= false;
            cell->info().saturation=1.0;
        }
    }
    ///if we start from imbibition
    if(!drainageFirst)
    {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().Pcondition) continue;
	    cell->info().p()=bndCondValue[3];
            cell->info().isWRes = false;
            cell->info().isNWRes= true;
            cell->info().saturation=0.0;
        }
    }
    if(solver->debugOut) {cout<<"----initializeReservoirs----"<<endl;}    
}*/
/*
void UnsaturatedEngine::initialReservoirs()
{
    initialWResBound();
    initialNWResBound();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().p()==bndCondValue[2]) {cell->info().isWRes=true;cell->info().isNWRes=false;}
      if (cell->info().p()==bndCondValue[3]) {cell->info().isNWRes=true;cell->info().isWRes=false;}
    }       
}*/

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
      }
    } 
}
/*
void UnsaturatedEngine::updataPoreSaturation()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if( (cell->info().isWRes) || (cell->info().isTrapW) ) {cell->info().saturation=1.0;}
      else if( (cell->info().isNWRes) || (cell->info().isTrapNW) ) {cell->info().saturation=0.0;}
      else cerr<<"updataPoreSaturation Error! check pore flag."<<endl;
    }  
}*/
/*
///boundingCells[2] always connect W-reservoir. 
void UnsaturatedEngine::initialWResBound()
{
        for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
            (*it)->info().isWRes = true;
            (*it)->info().isNWRes = false;}
}
///boundingCells[3] always connect NW-reservoir
void UnsaturatedEngine::initialNWResBound()
{
        for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
            (*it)->info().isNWRes = true;
            (*it)->info().isWRes = false;}
}*/

void UnsaturatedEngine::invasion()
{
    if (isPhaseTrapped) invasion1();
    else invasion2();
}

///mode1 and mode2 can share the same invasionSingleCell(), invasionSingleCell() ONLY change neighbor pressure and neighbor saturation, independent of reservoirInfo.
void UnsaturatedEngine::invasionSingleCell(CellHandle cell/*, double pressure*/)
{
    double localPressure=cell->info().p();
    double localSaturation=cell->info().saturation;
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;//FIXME:defensive
        if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;

// 	if ( (nCell->info().saturation==localSaturation) && (nCell->info().p()==localPressure) ) continue;
	if ( (nCell->info().saturation==localSaturation) && (nCell->info().p() != localPressure) && ((nCell->info().isTrapNW)||(nCell->info().isTrapW)) ) {
	  nCell->info().p() = localPressure;
	  if(solver->debugOut) {cerr<<"merge trapped phase"<<endl;}
	  invasionSingleCell(nCell);} ///here we merge trapped phase back to reservoir 
	else if ( (nCell->info().saturation>localSaturation) ) {
	  double nPcThroat=surfaceTension/cell->info().poreThroatRadius[facet];
	  double nPcBody=surfaceTension/nCell->info().poreBodyRadius;
	  if( (localPressure-nCell->info().p()>nPcThroat) && (localPressure-nCell->info().p()>nPcBody) ) {
// 	  if( localPressure-nCell->info().p()>nPcThroat ) {
	    nCell->info().p() = localPressure;
	    nCell->info().saturation=localSaturation;
	    if(solver->debugOut) {cerr<<"drainage"<<endl;}
	    invasionSingleCell(nCell);
	  }
	}
	else if ( (nCell->info().saturation<localSaturation) ) {
// 	  double nPcThroat=surfaceTension/nCell->info().poreThroatRadius[facet];
	  double nPcBody=surfaceTension/nCell->info().poreBodyRadius;
	  if( (nCell->info().p()-localPressure<nPcBody) && (nCell->info().p()-localPressure<surfaceTension/nCell->info().poreThroatRadius[0]) && (nCell->info().p()-localPressure<surfaceTension/nCell->info().poreThroatRadius[1]) && (nCell->info().p()-localPressure<surfaceTension/nCell->info().poreThroatRadius[2]) && (nCell->info().p()-localPressure<surfaceTension/nCell->info().poreThroatRadius[3]) ) {
	    nCell->info().p() = localPressure;
	    nCell->info().saturation=localSaturation;
	    if(solver->debugOut) {cerr<<"imbibition"<<endl;}
	    invasionSingleCell(nCell);
	  }	  
	}
	else continue;
    }
}
///invasion mode 1: withTrap
void UnsaturatedEngine::invasion1()
{
    if(solver->debugOut) {cout<<"----start drainage1----"<<endl;}

    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    if(solver->debugOut) {cout<<"----invasion1.updatePressure----"<<endl;}

//     ///drainageSingleCell by Pressure difference, only change Pressure.    
//     for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
//       invasionSingleCell((*it));
//     } 
//     for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
//       invasionSingleCell((*it));
//     } 
//     if(solver->debugOut) {cout<<"----invasion1.invasionSingleCell----"<<endl;}
    
    ///drainageSingleCell by Pressure difference, only change Pressure.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().isWRes)
            invasionSingleCell(cell);
    }
    ///drainageSingleCell by Pressure difference, only change Pressure.
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().isNWRes)
            invasionSingleCell(cell);
    }

    if(solver->debugOut) {cout<<"----invasion1.invasionSingleCell----"<<endl;}

    ///update W, NW reservoirInfo according Pressure, trapped W-phase is marked by isWRes=False&&isNWRes=False.
    updateReservoirs1();
    if(solver->debugOut) {cout<<"----invasion1.update W, NW reservoirInfo----"<<endl;}
    
    ///search new trapped W-phase/NW-phase, assign trapCapP for trapped W-phase
    checkTrap(bndCondValue[3]-bndCondValue[2]);
    if(solver->debugOut) {cout<<"----invasion1.checkWTrap----"<<endl;}

    ///update trapped W-phase/NW-phase Pressure
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
 	if ( cell->info().isTrapW ) {cell->info().p()=bndCondValue[3]-cell->info().trapCapP;}
	if ( cell->info().isTrapNW) {cell->info().p()=bndCondValue[2]+cell->info().trapCapP;}
   }
    if(solver->debugOut) {cout<<"----invasion1.update trapped W-phase/NW-phase Pressure----"<<endl;}
    
//     ///update local pore saturation
//     updataPoreSaturation();
//     if(solver->debugOut) {cout<<"----drainage1.update local pore saturation----"<<endl;}    
}
/*
///mode1 and mode2 can share the same drainageSingleCell(), drainageSingleCell() ONLY change pressure, independent of reservoirInfo.
void UnsaturatedEngine::drainageSingleCell(CellHandle cell, double pressure)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;//FIXME:defensive
        if ( (nCell->info().isFictious) && (!isInvadeBoundary) )continue;
        if (nCell->info().p() == bndCondValue[2]) {
            double nCellP = surfaceTension/cell->info().poreThroatRadius[facet];
            if (pressure-nCell->info().p() > nCellP) {
                nCell->info().p() = pressure;
                drainageSingleCell(nCell, pressure);}}}
}*/
/*
///drainage mode 1: withTrap
void UnsaturatedEngine::drainage1()
{
    if(solver->debugOut) {cout<<"----start drainage1----"<<endl;}

    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    if(solver->debugOut) {cout<<"----drainage1.updatePressure----"<<endl;}
    
    ///drainageSingleCell by Pressure difference, only change Pressure.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().p() == bndCondValue[3])
            drainageSingleCell(cell,cell->info().p());
    }
    if(solver->debugOut) {cout<<"----drainage1.drainageSingleCell----"<<endl;}
    
    ///update W, NW reservoirInfo according Pressure, trapped W-phase is marked by isWRes=False&&isNWRes=False.
    updateReservoirs1();
    if(solver->debugOut) {cout<<"----drainage1.update W, NW reservoirInfo----"<<endl;}
    
    ///search new trapped W-phase, assign trapCapP for trapped W-phase
    checkWTrap(bndCondValue[3]-bndCondValue[2]);
    if(solver->debugOut) {cout<<"----drainage1.checkWTrap----"<<endl;}

    ///update trapped W-phase Pressure
    FiniteCellsIterator nCellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator nCell = tri.finite_cells_begin(); nCell != nCellEnd; nCell++ ) {
        if(nCell->info().isTrapW)
	  nCell->info().p() = bndCondValue[3] - nCell->info().trapCapP;
    }
    if(solver->debugOut) {cout<<"----drainage1.update trapped W-phase Pressure----"<<endl;}
    
    ///update local pore saturation
    updataPoreSaturation();
    if(solver->debugOut) {cout<<"----drainage1.update local pore saturation----"<<endl;}    
}*/

///search trapped W-phase or NW-phase, define trapCapP=Pn-Pw. assign isTrapW/isTrapNW info.
void UnsaturatedEngine::checkTrap(double pressure)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
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
// 
//     initialWResBound();
//     initialNWResBound();
    
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
        if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().p() != bndCondValue[2]) continue;
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
        if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().p() != bndCondValue[3]) continue;
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
    
    ///drainageSingleCell by Pressure difference, only change Pressure.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().isWRes)
            invasionSingleCell(cell);
    }
    ///drainageSingleCell by Pressure difference, only change Pressure.
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().isNWRes)
            invasionSingleCell(cell);
    }

    if(solver->debugOut) {cout<<"----invasion2.invasionSingleCell----"<<endl;}
    
    ///update W, NW reservoirInfo according Pressure
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
                if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
                if ( nCell->info().isWRes == true ) {
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
                if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
                if ( nCell->info().isNWRes == true ) {
//                     double nCellP = std::min( (surfaceTension/nCell->info().poreBodyRadius), (surfaceTension/cell->info().poreThroatRadius[facet]));
                    double nCellP = std::min( (surfaceTension/nCell->info().poreBodyRadius), std::min( std::min( std::min( (surfaceTension/nCell->info().poreThroatRadius[0]),(surfaceTension/nCell->info().poreThroatRadius[1]) ), (surfaceTension/nCell->info().poreThroatRadius[2]) ),(surfaceTension/nCell->info().poreThroatRadius[3])) );
                    nextEntry = std::max(nextEntry,nCellP);}}}}
                    
    if (nextEntry==-1e50) {
        cout << "End imbibition !" << endl;
        return nextEntry=0;
    }
    else return nextEntry;
}

/*
double UnsaturatedEngine::getSaturation(bool isSideBoundaryIncluded)
{
    if( (!isInvadeBoundary) && (isSideBoundaryIncluded)) cerr<<"In isInvadeBoundary=false drainage, isSideBoundaryIncluded can't set true."<<endl;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    double poresVolume = 0.0; //total pores volume
    double nwVolume = 0.0; 	//NW-phase volume
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isSideBoundaryIncluded) ) continue;
        poresVolume = poresVolume + cell->info().poreBodyVolume;
        if (cell->info().p()==bndCondValue[3]) {
            nwVolume = nwVolume + cell->info().poreBodyVolume;
        }
    }
    double saturation = 1 - nwVolume/poresVolume;
    return saturation;
}*/

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

void UnsaturatedEngine::savePhaseVtk(const char* folder)
{
// 	RTriangulation& Tri = T[solver->noCache?(!currentTes):currentTes].Triangulation();
	RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
        static unsigned int number = 0;
        char filename[80];
	mkdir(folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        sprintf(filename,"%s/out_%d.vtk",folder,number++);
	int firstReal=-1;

	//count fictious vertices and cells
	solver->vtkInfiniteVertices=solver->vtkInfiniteCells=0;
 	FiniteCellsIterator cellEnd = Tri.finite_cells_end();
        for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (!isDrawable) solver->vtkInfiniteCells+=1;
	}
	for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
                if (!v->info().isReal()) solver->vtkInfiniteVertices+=1;
                else if (firstReal==-1) firstReal=solver->vtkInfiniteVertices;}

        basicVTKwritter vtkfile((unsigned int) Tri.number_of_vertices()-solver->vtkInfiniteVertices, (unsigned int) Tri.number_of_finite_cells()-solver->vtkInfiniteCells);

        vtkfile.open(filename,"test");

        vtkfile.begin_vertices();
        double x,y,z;
        for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
		if (v->info().isReal()){
		x = (double)(v->point().point()[0]);
                y = (double)(v->point().point()[1]);
                z = (double)(v->point().point()[2]);
                vtkfile.write_point(x,y,z);}
        }
        vtkfile.end_vertices();

        vtkfile.begin_cells();
        for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
        	if (isDrawable){vtkfile.write_cell(cell->vertex(0)->info().id()-firstReal, cell->vertex(1)->info().id()-firstReal, cell->vertex(2)->info().id()-firstReal, cell->vertex(3)->info().id()-firstReal);}
        }
        vtkfile.end_cells();

	vtkfile.begin_data("Saturation",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().saturation);}
	}
	vtkfile.end_data();
}

// ------------------for checking----------
void UnsaturatedEngine::checkPoreRadius()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("checkPoreRadius.txt");
    file<<"cellID "<<"poreThroatRadius[0] "<<"poreThroatRadius[1] "<<"poreThroatRadius[2] "<<"poreThroatRadius[3] "<<"poreBodyRadius"<<endl;
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            file << cell->info().index << "	" <<cell->info().poreThroatRadius[0]<< "	"<<cell->info().poreThroatRadius[1]<< "	"<<cell->info().poreThroatRadius[2]<< "	"<<cell->info().poreThroatRadius[3]<< "	"<<cell->info().poreBodyRadius<< endl;
    }
    file.close();  
    file.open("checkPoreThroatRadiusCompareBodyRadius.txt");
    file<<"cellID "<<"poreThroatRadius[0] "<<"poreThroatRadius[1] "<<"poreThroatRadius[2] "<<"poreThroatRadius[3] "<<"poreBodyRadius"<<endl;
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if( (cell->info().poreBodyRadius<cell->info().poreThroatRadius[0]) || (cell->info().poreBodyRadius<cell->info().poreThroatRadius[1]) || (cell->info().poreBodyRadius<cell->info().poreThroatRadius[2]) || (cell->info().poreBodyRadius<cell->info().poreThroatRadius[3]) )	
            file << cell->info().index << "	" <<cell->info().poreThroatRadius[0]<< "	"<<cell->info().poreThroatRadius[1]<< "	"<<cell->info().poreThroatRadius[2]<< "	"<<cell->info().poreThroatRadius[3]<< "	"<<cell->info().poreBodyRadius<< endl;
    }
    file.close();
    file.open("checkPoreThroatCircleRadiusCompareBodyRadius.txt");
    file<<"cellID "<<"poreThroatRadius[0] "<<"poreThroatRadius[1] "<<"poreThroatRadius[2] "<<"poreThroatRadius[3] "<<"poreBodyRadius"<<endl;
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if( (cell->info().poreBodyRadius<solver->computeEffectiveRadius(cell,0)) || (cell->info().poreBodyRadius<solver->computeEffectiveRadius(cell,1)) || (cell->info().poreBodyRadius<solver->computeEffectiveRadius(cell,2)) || (cell->info().poreBodyRadius<solver->computeEffectiveRadius(cell,3)) )	
            file << cell->info().index << "	" <<solver->computeEffectiveRadius(cell,0)<< "	"<<solver->computeEffectiveRadius(cell,1)<< "	"<<solver->computeEffectiveRadius(cell,2)<< "	"<<solver->computeEffectiveRadius(cell,3)<< "	"<<cell->info().poreBodyRadius<< endl;
    }
    file.close();  
    file.open("checkPoreBodyRadiusNegative.txt");
    file<<"cellID "<<"poreThroatRadius[0] "<<"poreThroatRadius[1] "<<"poreThroatRadius[2] "<<"poreThroatRadius[3] "<<"poreBodyRadius"<<endl;
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if(  (cell->info().poreBodyRadius<0) )	
            file << cell->info().index << "	" <<solver->computeEffectiveRadius(cell,0)<< "	"<<solver->computeEffectiveRadius(cell,1)<< "	"<<solver->computeEffectiveRadius(cell,2)<< "	"<<solver->computeEffectiveRadius(cell,3)<< "	"<<cell->info().poreBodyRadius<< endl;
    }
    file.close();  

}
double UnsaturatedEngine::getRMin(CellHandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;
    Vector3r pos[3]; //solid pos
    double r[3]; //solid radius
    double e[3]; //edges of triangulation
    double g[3]; //gap radius between solid
    
    for (int i=0; i<3; i++) {
      pos[i] = makeVector3r(cell->vertex(facetVertices[j][i])->point().point());
      r[i] = sqrt(cell->vertex(facetVertices[j][i])->point().weight());
    }
    
    e[0] = (pos[1]-pos[2]).norm();
    e[1] = (pos[2]-pos[0]).norm();
    e[2] = (pos[1]-pos[0]).norm();
    g[0] = ((e[0]-r[1]-r[2])>0) ? 0.5*(e[0]-r[1]-r[2]):0 ;
    g[1] = ((e[1]-r[2]-r[0])>0) ? 0.5*(e[1]-r[2]-r[0]):0 ;
    g[2] = ((e[2]-r[0]-r[1])>0) ? 0.5*(e[2]-r[0]-r[1]):0 ;
    
    double rmin= (std::max(g[0],std::max(g[1],g[2]))==0) ? 1.0e-10:std::max(g[0],std::max(g[1],g[2])) ;
    return rmin;
}
double UnsaturatedEngine::getRMax(CellHandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) {return 0;cerr<<"tri.is_infinite(cell->neighbor(j)"<<endl;}
    double rmax = std::abs(solver->computeEffectiveRadius(cell, j));//rmin>rmax ?
    return rmax;
}
void UnsaturatedEngine::checkRCompare()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("rCompareSum.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        for (int facet=0; facet<4; facet++) {
            file << cell->info().index << "	" <<facet<< "	" <<getRMin(cell,facet) << "	" << computeEffPoreThroatRadius(cell,facet)<< "	" <<getRMax(cell,facet) << endl;
        }
    }
    file.close();

    file.open("rMinEqualR.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd1 = tri.finite_cells_end();
    for ( FiniteCellsIterator cell1 = tri.finite_cells_begin(); cell1 != cellEnd1; cell1++ ) {
        for (int facet=0; facet<4; facet++) {
            if(getRMin(cell1,facet)==computeEffPoreThroatRadius(cell1,facet))
                file << cell1->info().index << "	" <<facet<< "	" <<getRMin(cell1,facet) << "	" << computeEffPoreThroatRadius(cell1,facet)<< "	" <<getRMax(cell1,facet) << endl;
        }
    }
    file.close();

    file.open("rMaxEqualR.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd2 = tri.finite_cells_end();
    for ( FiniteCellsIterator cell2 = tri.finite_cells_begin(); cell2 != cellEnd2; cell2++ ) {
        for (int facet=0; facet<4; facet++) {
            if(getRMax(cell2,facet)==computeEffPoreThroatRadius(cell2,facet))
                file << cell2->info().index << "	" <<facet<< "	" <<getRMin(cell2,facet) << "	" << computeEffPoreThroatRadius(cell2,facet)<< "	" <<getRMax(cell2,facet) << endl;
        }
    }
    file.close();

    file.open("rFine.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd3 = tri.finite_cells_end();
    for ( FiniteCellsIterator cell3 = tri.finite_cells_begin(); cell3 != cellEnd3; cell3++ ) {
      for (int facet=0; facet<4; facet++) {
	if( (getRMax(cell3,facet)>computeEffPoreThroatRadius(cell3,facet)) && (getRMin(cell3,facet)<computeEffPoreThroatRadius(cell3,facet)) ) {
            file << cell3->info().index << "	" <<facet<< "	" <<getRMin(cell3,facet) << "	" << computeEffPoreThroatRadius(cell3,facet)<< "	" <<getRMax(cell3,facet) << endl;
        }
      }
    }
    file.close();

    file.open("rBug.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd4 = tri.finite_cells_end();
    for ( FiniteCellsIterator cell4 = tri.finite_cells_begin(); cell4 != cellEnd4; cell4++ ) {
      for(int facet=0; facet<4; facet++) {
        if(getRMax(cell4,facet)<getRMin(cell4,facet)) {
            file << cell4->info().index << "	" <<facet<< "	" <<getRMin(cell4,facet) << "	" << computeEffPoreThroatRadius(cell4,facet)<< "	" <<getRMax(cell4,facet) << endl;
        }	
      }
    }
    file.close();
}

void UnsaturatedEngine::checkCellsConnection()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("cellsConnection.txt");
    file << "cell" << " " << "neighborCells" << endl;
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        file << cell->info().index << " " <<cell->neighbor(0)->info().index << " " << cell->neighbor(1)->info().index << " " << cell->neighbor(2)->info().index << " " << cell->neighbor(3)->info().index << endl;
    }
    file.close();
}

void UnsaturatedEngine::checkEntryCapillaryPressure()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("entryCapillaryPressure.txt");
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        for (int facet=0; facet<4; facet++) {
            file <<surfaceTension/cell->info().poreThroatRadius[facet] << endl;
        }
    }
    file.close();

    file.open("entryCapillaryPressureNoBound.txt");
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
    if (cell->info().isFictious) continue;
      for (int facet=0; facet<4; facet++) {
            file <<surfaceTension/cell->info().poreThroatRadius[facet] << endl;
        }
    }
    file.close();

}

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

void UnsaturatedEngine::checkBoundingCellsInfo()
{
    ofstream file;
    file.open("boundInfo.txt");
    file << "#Checking the boundingCells states\n";
    file << "CellID" << "	CellPressure" << "	isNWRes" << "	isWRes" <<endl;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if ((cell->info().isFictious==true)&&(cell->info().Pcondition==false)) {
            file << cell->info().index <<" "<<cell->info().p()<<" "<<cell->info().isNWRes<<" "<<cell->info().isWRes<<endl;
        }
    }
}

void UnsaturatedEngine::checkReservoirInfo(int boundN)
{
    if(solver->boundingCells[boundN].size()==0) {
        cerr << "please set corresponding bndCondIsPressure[bound] to be true ."<< endl;
    }
    else {
        if (boundN==2) {
            ofstream file;
            file.open("waterReservoirBoundInfo.txt");
            file << "#Checking the water reservoir cells states\n";
            file << "CellID" << "	CellPressure" << "	isNWRes" << "	isWRes" <<endl;
            for (FlowSolver::VCellIterator it = solver->boundingCells[boundN].begin() ; it != solver->boundingCells[boundN].end(); it++) {
                file << (*it)->info().index <<" "<<(*it)->info().p()<<" "<<(*it)->info().isNWRes<<" "<<(*it)->info().isWRes<<endl;
            }
            file.close();
        }
        else if (boundN==3) {
            ofstream file;
            file.open("airReservoirBoundInfo.txt");
            file << "#Checking the air reservoir cells state\n";
            file << "CellID"<<"	CellPressure"<<"	isNWRes"<<"	isWRes"<<endl;
            for (FlowSolver::VCellIterator it = solver->boundingCells[boundN].begin(); it != solver->boundingCells[boundN].end(); it++) {
                file << (*it)->info().index <<" "<<(*it)->info().p()<<" "<<(*it)->info().isNWRes<<" "<<(*it)->info().isWRes<<endl;
            }
            file.close();
        }
        else {
            cerr<<"This is not a reservoir boundary. Please set boundN to be 2(waterReservoirBound) or 3(airReservoirBound)."<<endl;
        }
    }
}

void UnsaturatedEngine::printSomething()
{
      RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if ((cell->info().isFictious==true)&&(cell->info().Pcondition==false)) {
cerr<<cell->info().saturation<<endl;        }
    }

}

//----------temp functions for comparison with experiment-----------------------
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
    
double UnsaturatedEngine::getInvadeDepth()
{
    double depth=0.0;
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

double UnsaturatedEngine::getSubdomainSaturation(Vector3r pos, double radius)
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
/*
void UnsaturatedEngine::computeSolidLine()
{
    RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = Tri.finite_cells_end();
    for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
        for(int j=0; j<4; j++) {
            solver->lineSolidPore(cell, j);
        }
    }
    if(solver->debugOut) {cout<<"----computeSolidLine-----."<<endl;}
}*/

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

#endif //TWOPHASEFLOW
#endif //FLOW_ENGINE
