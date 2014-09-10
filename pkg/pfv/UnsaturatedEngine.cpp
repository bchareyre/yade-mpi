/*************************************************************************
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*  Copyright (C) 2012 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE

//keep this #ifdef for commited versions unless you really have stable version that should be compiled by default
//it will save compilation time for everyone else
//when you want it compiled, you can pass -DDFNFLOW to cmake, or just uncomment the following line
#define UNSATURATED_FLOW
#ifdef UNSATURATED_FLOW

#include "FlowEngine_UnsaturatedEngineT.hpp"

/// We can add data to the Info types by inheritance
class UnsatCellInfo : public FlowCellInfo_UnsaturatedEngineT {
  	public:
  	bool isWaterReservoir;
	bool isAirReservoir;
	double capillaryCellVolume;//std::abs(cell.volume) - std::abs(cell.solid.volume)
	std::vector<double> poreRadius;//pore throat radius for drainage
	double solidLine [4][4];//the length of intersecting line between sphere and facet. [i][j] is for sphere facet "i" and sphere facetVertices[i][j]. Last component for 1/sumLines in the facet.
	double trapCapP;//for calculating the pressure of trapped phase, cell.pressureTrapped = pressureAir - trapCapP.
	int windowsID;//a temp cell info for experiment comparison
	UnsatCellInfo (void)
	{
		poreRadius.resize(4, 0);
		isWaterReservoir = true; isAirReservoir = false; capillaryCellVolume = 0;	  
		for (int k=0; k<4;k++) for (int l=0; l<3;l++) solidLine[k][l]=0;
		trapCapP = 0;
		windowsID = 0;
	}
};

class UnsatVertexInfo : public FlowVertexInfo_UnsaturatedEngineT {
//	add later;  
public:
// 	UnsatVertexInfo (void)
};

typedef TemplateFlowEngine_UnsaturatedEngineT<UnsatCellInfo,UnsatVertexInfo> UnsaturatedEngineT;
REGISTER_SERIALIZABLE(UnsaturatedEngineT);
YADE_PLUGIN((UnsaturatedEngineT));

class UnsaturatedEngine : public UnsaturatedEngineT
{
		double totalCellVolume;
	protected:
		void testFunction();		

	public :
		void initializeReservoirs();///only used for determining first entry pressure
		void initializeCellIndex();
		void updatePoreRadius();
		void updateTotalCellVolume();
		void updateVolumeCapillaryCell();
		double computeCellInterfacialArea(CellHandle cell, int j, double rC);
		double computeEffPoreRadius(CellHandle cell, int j);
		double computeEffPoreRadiusFine(CellHandle cell, int j);
		double bisection(CellHandle cell, int j, double a, double b);
		double computeTriRadian(double a, double b, double c);
		double computeDeltaForce(CellHandle cell,int j, double rC);		

		void computeSolidLine();
		void computeFacetPoreForcesWithCache(bool onlyCache=false);	
		void computeCapillaryForce() {computeFacetPoreForcesWithCache(false);}
		
		
		void invade();
		///functions can be shared by two modes
		void invadeSingleCell(CellHandle cell, double pressure);
		void updatePressure();
		double getMinEntryValue();
		double getSaturation();
		double getSpecificInterfacialArea();

		void invade1();
		void updateReservoirs1();
		void initWaterReservoirBound();
		void initAirReservoirBound();
		void waterReservoirRecursion(CellHandle cell);
		void airReservoirRecursion(CellHandle cell);
		void checkTrap(double pressure);

		void invade2();
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
		double getWindowsSaturation(int i);
		bool checknoCache() {return solver->noCache;}
		
		double getRMin(CellHandle cell, int j);
		double getRMax(CellHandle cell, int j);
		void checkRCompare();
				
		virtual ~UnsaturatedEngine();

		virtual void action();
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(UnsaturatedEngine,UnsaturatedEngineT,"Preliminary version engine of a drainage model for unsaturated soils. Note:Air reservoir is on the top; water reservoir is on the bottom.",
					((bool, isPhaseTrapped, true,,"Activate invade mode. If True, the wetting phase can be trapped, activate invade mode 1; if false, the wetting phase cann't be trapped, activate invade mode 2."))
					((double,gasPressure,0,,"Invasion pressure"))
					((double,surfaceTension,0.0728,,"Water Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))

					((bool, computeForceActivated, true,,"Activate capillary force computation. WARNING: turning off means capillary force is not computed at all, but the drainage can still work."))
					((bool, isInvadeBoundary, true,,"Invade from boundaries."))
					((int, windowsNo, 10,, "Number of genrated windows(or zoomed samples)."))
					,,,
					.def("saveVtk",&UnsaturatedEngine::saveVtk,(boost::python::arg("folder")="./VTK"),"Save pressure field in vtk format. Specify a folder name for output.")
					.def("savePhaseVtk",&UnsaturatedEngine::savePhaseVtk,(boost::python::arg("folder")="./phaseVtk"),"Save W-phase and Nw-phase in vtk format. W-phase=1, NW-phase=0 (also can be seen as saturation of single cell.). Specify a folder name for output.")
					.def("getMinEntryValue",&UnsaturatedEngine::getMinEntryValue,"get the minimum air entry pressure for the next invade step.")
					.def("getSaturation",&UnsaturatedEngine::getSaturation,"get saturation.")
					.def("getSpecificInterfacialArea",&UnsaturatedEngine::getSpecificInterfacialArea,"get specific interfacial area (defined as the amount of fluid-fluid interfacial area per unit volume pf the porous medium).")
					.def("invade",&UnsaturatedEngine::invade,"Run the drainage invasion.")
					.def("computeCapillaryForce",&UnsaturatedEngine::computeCapillaryForce,"Compute capillary force. ")

					.def("checkCellsConnection",&UnsaturatedEngine::checkCellsConnection,"Check cell connections.")
					.def("checkEntryCapillaryPressure",&UnsaturatedEngine::checkEntryCapillaryPressure,"Check entry capillary pressure between neighbor cells.")
					.def("checkLatticeNodeY",&UnsaturatedEngine::checkLatticeNodeY,(boost::python::arg("y")),"Check the slice of lattice nodes for yNormal(y). 0: out of sphere; 1: inside of sphere.")
					.def("checkReservoirInfo",&UnsaturatedEngine::checkReservoirInfo,(boost::python::arg("boundN")),"Check reservoir cells(N=2,3) states and export to 'waterReservoirBoundInfo.txt' and 'airReservoirBoundInfo.txt'.")
					.def("checkBoundingCellsInfo",&UnsaturatedEngine::checkBoundingCellsInfo,"Check boundary cells (without reservoirs) states and export to 'boundInfo.txt'.")
					
					.def("testFunction",&UnsaturatedEngine::testFunction,"The playground for Chao's experiments.")
					.def("checknoCache",&UnsaturatedEngine::checknoCache,"check noCache. (temp func.)")
					.def("getWindowsSaturation",&UnsaturatedEngine::getWindowsSaturation,(boost::python::arg("windowsID")), "get saturation of windowsID")
					.def("initializeCellWindowsID",&UnsaturatedEngine::initializeCellWindowsID,"Initialize cell windows index. A temp function for comparison with experiments, will delete soon")
					.def("checkRCompare",&UnsaturatedEngine::checkRCompare,"debug R RMin RMax.")
					)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(UnsaturatedEngine);
YADE_PLUGIN((UnsaturatedEngine));

UnsaturatedEngine::~UnsaturatedEngine(){}

/*void UnsaturatedEngine::testFunction()
{
	cout<<"This is UnsaturatedEngine test program"<<endl;
	RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
	if (tri.number_of_vertices()==0) {
		cout<< "triangulation is empty: building a new one" << endl;
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(bndCondValue[2],*solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
		initializeReservoirs();
		initializeCellIndex();//initialize cell index
		updatePoreRadius();//save all pore radii before invade
		updateTotalCellVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
		updateVolumeCapillaryCell();//save capillary volume of all cells, for fast calculating saturation
		computeSolidLine();//save cell->info().solidLine[j][y]
	}
	solver->noCache = true;
}*/
void UnsaturatedEngine::testFunction()
{
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(bndCondValue[2],*solver);//create a triangulation and initialize pressure in the elements (connecting with W-reservoir), everything will be contained in "solver"
		initializeReservoirs();
		initializeCellIndex();//initialize cell index
		updatePoreRadius();//save all pore radii before invade
		updateTotalCellVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
		updateVolumeCapillaryCell();//save capillary volume of all cells, for fast calculating saturation
		computeSolidLine();//save cell->info().solidLine[j][y]
		solver->noCache = true;
}

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
	initializeReservoirs();
        initializeCellIndex();//initialize cell index
        updatePoreRadius();//save all pore radii before invade
        updateTotalCellVolume();//save total volume of porous medium, considering different invading boundaries condition (isInvadeBoundary==True or False), aiming to calculate specific interfacial area.
        updateVolumeCapillaryCell();//save capillary volume of all cells, for calculating saturation
        computeSolidLine();//save cell->info().solidLine[j][y]
        solver->noCache = true;
    }
    ///compute invade
    if (pressureForce) { invade();}
    
    ///compute force
    if(computeForceActivated){
    computeCapillaryForce();
    Vector3r force;
    FiniteVerticesIterator vertices_end = solver->T[solver->currentTes].Triangulation().finite_vertices_end();
    for ( FiniteVerticesIterator V_it = solver->T[solver->currentTes].Triangulation().finite_vertices_begin(); V_it !=  vertices_end; V_it++ ) {
        force = pressureForce ? Vector3r ( V_it->info().forces[0],V_it->info().forces[1],V_it->info().forces[2] ): Vector3r(0,0,0);
        scene->forces.addForce ( V_it->info().id(), force); }}
*/}

void UnsaturatedEngine::initializeCellIndex()
{
    int k=0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        cell->info().index=k++;}
}

void UnsaturatedEngine::updatePoreRadius()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    CellHandle neighbourCell;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        for (int j=0; j<4; j++) {
            neighbourCell = cell->neighbor(j);
            if (!tri.is_infinite(neighbourCell)) {
                cell->info().poreRadius[j]=computeEffPoreRadius(cell, j);
                neighbourCell->info().poreRadius[tri.mirror_index(cell, j)]= cell->info().poreRadius[j];}}}
}

void UnsaturatedEngine::updateTotalCellVolume()
{
    initializeVolumes(*solver);
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    totalCellVolume=0;
    
    if(isInvadeBoundary==true) {
        for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;//NOTE:reservoirs cells should not be included in totalCellVolume
            totalCellVolume = totalCellVolume + std::abs( cell->info().volume() );}}
    else {
        for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;//NOTE:reservoirs cells should not be included in totalCellVolume
            if (cell->info().isFictious) continue;
            totalCellVolume = totalCellVolume + std::abs( cell->info().volume() );}}
}

void UnsaturatedEngine::updateVolumeCapillaryCell()
{
    initializeVolumes(*solver);
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    CellHandle neighbourCell;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        cell->info().capillaryCellVolume = std::abs( cell->info().volume() ) - solver->volumeSolidPore(cell);
    }
}

void UnsaturatedEngine::initializeReservoirs()
{
    initWaterReservoirBound();
    initAirReservoirBound();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().p()==bndCondValue[2]) {cell->info().isWaterReservoir=true;cell->info().isAirReservoir=false;}
      if (cell->info().p()==bndCondValue[3]) {cell->info().isAirReservoir=true;cell->info().isWaterReservoir=false;}
    }       
}

void UnsaturatedEngine::updatePressure()
{
    boundaryConditions(*solver);
    solver->pressureChanged=true;
    solver->reApplyBoundaryConditions();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().isWaterReservoir==true) {cell->info().p()=bndCondValue[2];}
      if (cell->info().isAirReservoir==true) {cell->info().p()=bndCondValue[3];}
      if (isPhaseTrapped) {
	if ( (cell->info().isWaterReservoir==false)&&(cell->info().isAirReservoir==false) ) {cell->info().p()=bndCondValue[3]-cell->info().trapCapP;}
      }
    } 
}

///boundingCells[2] always connect W-reservoir. 
void UnsaturatedEngine::initWaterReservoirBound()
{
    if (solver->boundingCells[2].size()==0) {cerr<<"ERROR! set bndCondIsPressure[2] true. boundingCells.size=0!";}
    else {
        for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
            if ((*it)->info().index == 0) continue;
            (*it)->info().isWaterReservoir = true;
            (*it)->info().isAirReservoir = false;}}
}
///boundingCells[3] always connect NW-reservoir
void UnsaturatedEngine::initAirReservoirBound()
{
    if (solver->boundingCells[3].size()==0) {cerr<<"ERROR! set bndCondIsPressure[3] true. boundingCells.size=0!";}
    else {
        for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
            if((*it)->info().index == 0) continue;
            (*it)->info().isAirReservoir = true;
            (*it)->info().isWaterReservoir = false;}}
}

void UnsaturatedEngine::invade()
{
    if (isPhaseTrapped) invade1();
    else invade2();
}

///mode1 and mode2 can share the same invadeSingleCell()
void UnsaturatedEngine::invadeSingleCell(CellHandle cell, double pressure)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;//FIXME:defensive
        if ( (nCell->info().isFictious) && (!isInvadeBoundary) )continue;
        if (nCell->info().p() == bndCondValue[2]) {
            double nCellP = surfaceTension/cell->info().poreRadius[facet];
            if (pressure-nCell->info().p() > nCellP) {
                nCell->info().p() = pressure;
                invadeSingleCell(nCell, pressure);}}}
}

///invade mode 1: withTrap
void UnsaturatedEngine::invade1()
{
    if(solver->debugOut) {cout<<"----start invade1----"<<endl;}

    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    if(solver->debugOut) {cout<<"----invade1.updatePressure----"<<endl;}
    
    ///invadeSingleCell by Pressure difference, only change Pressure.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().p() == bndCondValue[3])
            invadeSingleCell(cell,cell->info().p());
    }
    if(solver->debugOut) {cout<<"----invade1.invadeSingleCell----"<<endl;}
    
    ///update W, NW reservoirInfo according Pressure, trapped W-phase is marked by isWaterReservoir=False&&isAirReservoir=False.
    updateReservoirs1();
    if(solver->debugOut) {cout<<"----invade1.update W, NW reservoirInfo----"<<endl;}
    
    ///search new trapped W-phase, assign trapCapP for trapped W-phase
    checkTrap(bndCondValue[3]-bndCondValue[2]);
    if(solver->debugOut) {cout<<"----invade1.checkTrap----"<<endl;}

    ///update trapped W-phase Pressure
    FiniteCellsIterator ncellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator ncell = tri.finite_cells_begin(); ncell != ncellEnd; ncell++ ) {
        if( (ncell->info().isWaterReservoir) || (ncell->info().isAirReservoir) ) continue;
        ncell->info().p() = bndCondValue[3] - ncell->info().trapCapP;
    }
    if(solver->debugOut) {cout<<"----invade1.update trapped W-phase Pressure----"<<endl;}
    
}

///search trapped W-phase, define trapCapP=Pn-Pw.
void UnsaturatedEngine::checkTrap(double pressure)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if( (cell->info().isWaterReservoir) || (cell->info().isAirReservoir) ) continue;
      if(cell->info().p()!= bndCondValue[2]) continue;
      cell->info().trapCapP=pressure;
    }
}

void UnsaturatedEngine::updateReservoirs1()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        cell->info().isWaterReservoir = false;
        cell->info().isAirReservoir = false;
    }
    if(solver->debugOut) {cout<<"----updateReservoirs1.initial----"<<endl;}

    initWaterReservoirBound();
    if(solver->debugOut) {cout<<"----updateReservoirs1.initWaterReservoirBound----"<<endl;}
    initAirReservoirBound();
    if(solver->debugOut) {cout<<"----updateReservoirs1.initAirReservoirBound----"<<endl;}
    
    for (FlowSolver::VCellIterator it = solver->boundingCells[2].begin(); it != solver->boundingCells[2].end(); it++) {
        if ((*it)->info().index == 0) continue;
        waterReservoirRecursion(*it);
    }
    if(solver->debugOut) {cout<<"----updateReservoirs1.waterReservoirRecursion----"<<endl;}
    
    for (FlowSolver::VCellIterator it = solver->boundingCells[3].begin(); it != solver->boundingCells[3].end(); it++) {
        if ((*it)->info().index == 0) continue;
        airReservoirRecursion(*it);
    }
    if(solver->debugOut) {cout<<"----updateReservoirs1.airReservoirRecursion----"<<endl;}
}

void UnsaturatedEngine::waterReservoirRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;
	if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().p() != bndCondValue[2]) continue;
        if (nCell->info().isWaterReservoir==true) continue;
        nCell->info().isWaterReservoir = true;
	nCell->info().isAirReservoir = false;
        waterReservoirRecursion(nCell);
    }
}

void UnsaturatedEngine::airReservoirRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        CellHandle nCell = cell->neighbor(facet);
        if (solver->T[solver->currentTes].Triangulation().is_infinite(nCell)) continue;
        if (nCell->info().Pcondition) continue;
	if ( (nCell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (nCell->info().p() != bndCondValue[3]) continue;
        if (nCell->info().isAirReservoir==true) continue;
        nCell->info().isAirReservoir = true;
	nCell->info().isWaterReservoir = false;
        airReservoirRecursion(nCell);
    }
}

///invade mode 2: withoutTrap
void UnsaturatedEngine::invade2()
{
    ///update Pw, Pn according to reservoirInfo.
    updatePressure();
    ///invadeSingleCell by Pressure difference, only change Pressure.
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().p() == bndCondValue[3])
            invadeSingleCell(cell,cell->info().p());
    }
    ///update W, NW reservoirInfo according Pressure
    updateReservoirs2();
}

void UnsaturatedEngine::updateReservoirs2()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().p()==bndCondValue[2]) {cell->info().isWaterReservoir=true; cell->info().isAirReservoir=false;}
        else if (cell->info().p()==bndCondValue[3]) {cell->info().isAirReservoir=true; cell->info().isWaterReservoir=false;}
        else {cerr<<"invade mode2: updateReservoir Error!"<<endl;}
    }
}

double UnsaturatedEngine::getMinEntryValue()
{
    double nextEntry = 1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().isAirReservoir == true) {
            for (int facet=0; facet<4; facet ++) {
                if (tri.is_infinite(cell->neighbor(facet))) continue;
                if (cell->neighbor(facet)->info().Pcondition) continue;
                if ( (cell->neighbor(facet)->info().isFictious) && (!isInvadeBoundary) ) continue;
                if ( cell->neighbor(facet)->info().isWaterReservoir == true ) {
                    double nCellP = surfaceTension/cell->info().poreRadius[facet];
                    nextEntry = std::min(nextEntry,nCellP);}}}}
                    
    if (nextEntry==1e50) {
        cout << "End drainage !" << endl;
        return nextEntry=0;
    }
    else return nextEntry;
}

double UnsaturatedEngine::getSaturation()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    double capillaryVolume = 0.0; //total capillary volume
    double airVolume = 0.0; 	//air volume
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isInvadeBoundary) ) continue;
        capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
        if (cell->info().p()==bndCondValue[3]) {
            airVolume = airVolume + cell->info().capillaryCellVolume;
        }
    }
    double saturation = 1 - airVolume/capillaryVolume;
    return saturation;
}

double UnsaturatedEngine::getSpecificInterfacialArea()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    double interfacialArea=0;

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
//             if (cell->info().Pcondition==true) continue;//NOTE:reservoirs cells interfacialArea should not be included.
            if(cell->info().isFictious) continue;
            if (cell->info().isAirReservoir==true) {
                for (int facet = 0; facet < 4; facet ++) {
                    if (tri.is_infinite(cell->neighbor(facet))) continue;
                    if (cell->neighbor(facet)->info().Pcondition==true) continue;
                    if ( (cell->neighbor(facet)->info().isFictious) && (!isInvadeBoundary) ) continue;
                    if (cell->neighbor(facet)->info().isAirReservoir==false)
                        interfacialArea = interfacialArea + computeCellInterfacialArea(cell, facet, cell->info().poreRadius[facet]);}}}
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

double UnsaturatedEngine::computeEffPoreRadius(CellHandle cell, int j)
{
    double rInscribe = std::abs(solver->computeEffectiveRadius(cell, j));
    CellHandle cellh = CellHandle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
    double r;
    if(facetNFictious==0) {r=computeEffPoreRadiusFine(cell,j);}
    else r=rInscribe;    
    return r;
}

double UnsaturatedEngine::computeEffPoreRadiusFine(CellHandle cell, int j)
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
    double rmax = std::abs(solver->computeEffectiveRadius(cell, j));
//     if(rmin>rmax) { cerr<<"WARNING! rmin>rmax. rmin="<<rmin<<" ,rmax="<<rmax<<endl; }
    
    double deltaForceRMin = computeDeltaForce(cell,j,rmin);
    double deltaForceRMax = computeDeltaForce(cell,j,rmax);
    double effPoreRadius;
    
    if(deltaForceRMin>deltaForceRMax) {
      effPoreRadius=rmax;
      //cerr<<"ERROR! deltaForceRMin>deltaForceRMax. cellID: "<<cell->info().index<<"; deltaForceRMin="<<deltaForceRMin<<"; deltaForceRMax="<<deltaForceRMax<<". posA="<<pos[0]<<"; posB="<<pos[1]<<"; posC="<< pos[2]<<"; rA="<< r[0]<<"; rB="<< r[1]<<"; rC="<<r[2]<<endl;
    }
    else if(deltaForceRMax<0) {
      effPoreRadius=rmax;
      //cerr<<"deltaForceRMax<0. cellID: "<<cell->info().index<<"; deltaForceRMin="<<deltaForceRMin<<"; deltaForceRMax="<<deltaForceRMax<<". posA="<<pos[0]<<"; posB="<<pos[1]<<"; posC="<< pos[2]<<"; rA="<< r[0]<<"; rB="<< r[1]<<"; rC="<<r[2]<<endl;
    }
    else if(deltaForceRMin>0) {
      effPoreRadius=rmin;
      //cerr<<"0";      
    }
    else {
      effPoreRadius=bisection(cell,j,rmin,rmax);
      //cerr<<"1";
    }
    return effPoreRadius;
}
double UnsaturatedEngine::bisection(CellHandle cell, int j, double a, double b)
{
    double m = 0.5*(a+b);
    if (std::abs(b-a)>std::abs((solver->computeEffectiveRadius(cell, j)*1.0e-6))) {
        if ( computeDeltaForce(cell,j,m) * computeDeltaForce(cell,j,a) < 0 ) {
            b = m;
            return bisection(cell,j,a,b);}
        else {
            a = m;
            return bisection(cell,j,a,b);}}
    else return m;
}

//calculate radian with law of cosines. (solve $\alpha$)
double UnsaturatedEngine::computeTriRadian(double a, double b, double c)
{   
  double cosAlpha = (pow(b,2) + pow(c,2) - pow(a,2))/(2*b*c);
  if (cosAlpha>1.0) {cosAlpha=1.0;} if (cosAlpha<-1.0) {cosAlpha=-1.0;}
  double alpha = acos(cosAlpha);
  return alpha;
}

double UnsaturatedEngine::computeDeltaForce(CellHandle cell,int j, double rC)
{
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
    
    double lNW = (rad[0][0]+rad[1][1]+rad[2][2])*rC;
    double lNS = (rad[3][0]-rad[1][0]-rad[2][0])*r[0] + (rad[3][1]-rad[2][1]-rad[0][1])*r[1] + (rad[3][2]-rad[1][2]-rad[0][2])*r[2] ;
    double lInterface=lNW+lNS;
    
    double sW0=0.5*rRc[1]*rRc[2]*sin(rad[0][0])-0.5*rad[0][0]*pow(rC,2)-0.5*rad[0][1]*pow(r[1],2)-0.5*rad[0][2]*pow(r[2],2) ;
    double sW1=0.5*rRc[2]*rRc[0]*sin(rad[1][1])-0.5*rad[1][1]*pow(rC,2)-0.5*rad[1][2]*pow(r[2],2)-0.5*rad[1][0]*pow(r[0],2) ;
    double sW2=0.5*rRc[0]*rRc[1]*sin(rad[2][2])-0.5*rad[2][2]*pow(rC,2)-0.5*rad[2][0]*pow(r[0],2)-0.5*rad[2][1]*pow(r[1],2) ;
    double sW=sW0+sW1+sW2;
    double sVoid=sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
    double sInterface=sVoid-sW;

    double deltaF = lInterface - sInterface/rC;//deltaF=surfaceTension*(perimeterPore - areaPore/rCap)
    return deltaF;
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

	vtkfile.begin_data("Phase",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(!cell->info().isAirReservoir);}
	}
	vtkfile.end_data();
}

// ------------------for checking----------
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
            file << cell->info().index << "	" <<facet<< "	" <<getRMin(cell,facet) << "	" << computeEffPoreRadius(cell,facet)<< "	" <<getRMax(cell,facet) << endl;
        }
    }
    file.close();

    file.open("rMinEqualR.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd1 = tri.finite_cells_end();
    for ( FiniteCellsIterator cell1 = tri.finite_cells_begin(); cell1 != cellEnd1; cell1++ ) {
        for (int facet=0; facet<4; facet++) {
            if(getRMin(cell1,facet)==computeEffPoreRadius(cell1,facet))
                file << cell1->info().index << "	" <<facet<< "	" <<getRMin(cell1,facet) << "	" << computeEffPoreRadius(cell1,facet)<< "	" <<getRMax(cell1,facet) << endl;
        }
    }
    file.close();

    file.open("rMaxEqualR.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd2 = tri.finite_cells_end();
    for ( FiniteCellsIterator cell2 = tri.finite_cells_begin(); cell2 != cellEnd2; cell2++ ) {
        for (int facet=0; facet<4; facet++) {
            if(getRMax(cell2,facet)==computeEffPoreRadius(cell2,facet))
                file << cell2->info().index << "	" <<facet<< "	" <<getRMin(cell2,facet) << "	" << computeEffPoreRadius(cell2,facet)<< "	" <<getRMax(cell2,facet) << endl;
        }
    }
    file.close();

    file.open("rFine.txt");
    file<<"cellID	"<<"j	"<<"rmin	"<<"r	"<<"rmax	"<<endl;
    FiniteCellsIterator cellEnd3 = tri.finite_cells_end();
    for ( FiniteCellsIterator cell3 = tri.finite_cells_begin(); cell3 != cellEnd3; cell3++ ) {
      for (int facet=0; facet<4; facet++) {
	if( (getRMax(cell3,facet)>computeEffPoreRadius(cell3,facet)) && (getRMin(cell3,facet)<computeEffPoreRadius(cell3,facet)) ) {
            file << cell3->info().index << "	" <<facet<< "	" <<getRMin(cell3,facet) << "	" << computeEffPoreRadius(cell3,facet)<< "	" <<getRMax(cell3,facet) << endl;
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
            file << cell4->info().index << "	" <<facet<< "	" <<getRMin(cell4,facet) << "	" << computeEffPoreRadius(cell4,facet)<< "	" <<getRMax(cell4,facet) << endl;
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
    file << "#List of Connections \n";
    file << "cell" << " " << "neighborCell" << " " << "entryCapillaryPressure" << endl;
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        for (int facet=0; facet<4; facet++) {
            file << cell->info().index << " " <<cell->neighbor(facet)->info().index << " " << surfaceTension/cell->info().poreRadius[facet] << endl;
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
    file << "CellID" << "	CellPressure" << "	isAirReservoir" << "	isWaterReservoir" <<endl;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        if (cell->info().index==0) continue;
        if ((cell->info().isFictious==true)&&(cell->info().Pcondition==false)) {
            file << cell->info().index <<" "<<cell->info().p()<<" "<<cell->info().isAirReservoir<<" "<<cell->info().isWaterReservoir<<endl;
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
            file << "CellID" << "	CellPressure" << "	isAirReservoir" << "	isWaterReservoir" <<endl;
            for (FlowSolver::VCellIterator it = solver->boundingCells[boundN].begin() ; it != solver->boundingCells[boundN].end(); it++) {
                if ((*it)->info().index == 0) continue;
                file << (*it)->info().index <<" "<<(*it)->info().p()<<" "<<(*it)->info().isAirReservoir<<" "<<(*it)->info().isWaterReservoir<<endl;
            }
            file.close();
        }
        else if (boundN==3) {
            ofstream file;
            file.open("airReservoirBoundInfo.txt");
            file << "#Checking the air reservoir cells state\n";
            file << "CellID"<<"	CellPressure"<<"	isAirReservoir"<<"	isWaterReservoir"<<endl;
            for (FlowSolver::VCellIterator it = solver->boundingCells[boundN].begin(); it != solver->boundingCells[boundN].end(); it++) {
                if ((*it)->info().index == 0) continue;
                file << (*it)->info().index <<" "<<(*it)->info().p()<<" "<<(*it)->info().isAirReservoir<<" "<<(*it)->info().isWaterReservoir<<endl;
            }
            file.close();
        }
        else {
            cerr<<"This is not a reservoir boundary. Please set boundN to be 2(waterReservoirBound) or 3(airReservoirBound)."<<endl;
        }
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

double UnsaturatedEngine::getWindowsSaturation(int i)
{
    double capillaryVolume = 0.0; //total capillary volume
    double airVolume = 0.0; 	//air volume
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (tri.is_infinite(cell)) continue;
        if (cell->info().Pcondition) continue;
        if ( (cell->info().isFictious) && (!isInvadeBoundary) ) continue;
        if (cell->info().windowsID != i) continue;
        capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
        if (cell->info().p()==bndCondValue[3]) {
            airVolume = airVolume + cell->info().capillaryCellVolume;
        }
    }
    double saturation = 1 - airVolume/capillaryVolume;
    return saturation;
}
//--------------end of comparison with experiment----------------------------

///compute forces
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
}

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

#endif //UNSATURATED_FLOW
#endif //FLOW_ENGINE