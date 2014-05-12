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
#define TEMPLATE_FLOW_NAME UnsaturatedEngineT
#include <yade/pkg/pfv/FlowEngine.hpp>

/// We can add data to the Info types by inheritance
class UnsatCellInfo : public FlowCellInfo {
  	public:
  	bool isWaterReservoir;
	bool isAirReservoir;
	Real capillaryCellVolume;//abs(cell.volume) - abs(cell.solid.volume)
	std::vector<double> poreRadius;//pore throat radius for drainage
	double solidLine [4][4];//the length of intersecting line between sphere and facet. [i][j] is for sphere facet "i" and sphere facetVertices[i][j]. Last component for 1/sumLines in the facet.
	double trapCapP;//for calculating the pressure of trapped phase, cell.pressureTrapped = pressureAir - trapCapP.
	int windowsID;//a temp cell info for experiment comparison
	UnsatCellInfo (void)
	{
		poreRadius.resize(4, 0);
		isWaterReservoir = false; isAirReservoir = false; capillaryCellVolume = 0;	  
		for (int k=0; k<4;k++) for (int l=0; l<3;l++) solidLine[k][l]=0;
		trapCapP = 0;
		windowsID = 0;
	}
};

class UnsatVertexInfo : public FlowVertexInfo {
//	add later;  
public:
// 	UnsatVertexInfo (void)
};

typedef TemplateFlowEngine<UnsatCellInfo,UnsatVertexInfo> UnsaturatedEngineT;
REGISTER_SERIALIZABLE(UnsaturatedEngineT);
YADE_PLUGIN((UnsaturatedEngineT));

class UnsaturatedEngine : public UnsaturatedEngineT
{
	protected:
		void testFunction();
		

	public :
		void initializeCellIndex();
		void updatePoreRadius();
		void updateVolumeCapillaryCell();		
		double computeEffPoreRadius(CellHandle cell, int j);
		double computeEffPoreRadiusFine(CellHandle cell, int j);
		double bisection(CellHandle cell, int j, double a, double b);
		double computeDeltaForce(CellHandle cell,int j, double rCap);
		void computeSolidLine();
		void computeFacetPoreForcesWithCache(bool onlyCache=false);	
		void computeCapillaryForce() {computeFacetPoreForcesWithCache(false);}
		
		void invade();
		Real getMinEntryValue();
		Real getSaturation();

		void invadeSingleCell1(CellHandle cell, double pressure);
		void invade1();
		void checkTrap(double pressure);//check trapped phase, define trapCapP.	
		Real getMinEntryValue1();
		void updatePressure();
		void updatePressureReservoir();
		void initReservoirBound();
		void initWaterReservoirBound();
		void updateWaterReservoir();
		void waterReservoirRecursion(CellHandle cell);
		void initAirReservoirBound();
		void updateAirReservoir();
		void airReservoirRecursion(CellHandle cell);
		Real getSaturation1();

		void invadeSingleCell2(CellHandle cell, double pressure);
		void invade2();
		void updatePressure2();
		Real getMinEntryValue2();
		Real getSaturation2();	
		
		//record and test functions
		void checkCellsConnection();
		void checkEntryCapillaryPressure();
		void checkLatticeNodeY(double y); 
		void checkReservoirInfo(int boundN);
		void checkBoundingCellsInfo();
		void saveVtk(const char* folder) {bool initT=solver->noCache; solver->noCache=false; solver->saveVtk(folder); solver->noCache=initT;}
		//temp functions
		void initializeCellWindowsID();
		Real getWindowsSaturation(int windowsID);
		Real getWindowsSaturation1(int i);
		Real getWindowsSaturation2(int i);
		double getRadiusMin(CellHandle cell, int j);
		void debugTemp();
				
		virtual ~UnsaturatedEngine();

		virtual void action();
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(UnsaturatedEngine,UnsaturatedEngineT,"Preliminary version engine of a drainage model for unsaturated soils. Note:Air reservoir is on the top; water reservoir is on the bottom.",
					((bool, isPhaseTrapped, true,,"Activate invade mode. If True, the wetting phase can be trapped, activate invade mode 1; if false, the wetting phase cann't be trapped, activate invade mode 2."))
					((double,gasPressure,0,,"Invasion pressure"))
					((double,surfaceTension,0.0728,,"Water Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))

					((bool, computeForceActivated, true,,"Activate capillary force computation. WARNING: turning off means capillary force is not computed at all, but the drainage can still work."))
					((bool, invadeBoundary, false,,"Invade from boundaries."))
					((int, windowsNo, 10,, "Number of genrated windows/(zoomed samples)."))
					,,,
					 
					.def("saveVtk",&UnsaturatedEngine::saveVtk,(python::arg("folder")="./VTK"),"Save pressure field in vtk format. Specify a folder name for output.")
					.def("testFunction",&UnsaturatedEngine::testFunction,"The playground for Chao's experiments.")
					.def("getSaturation",&UnsaturatedEngine::getSaturation,"get saturation.")
					.def("getWindowsSaturation",&UnsaturatedEngine::getWindowsSaturation,(python::arg("windowsID")), "get saturation of windowsID")
					.def("getMinEntryValue",&UnsaturatedEngine::getMinEntryValue,"get the minimum air entry pressure for the next invade step.")
					.def("checkCellsConnection",&UnsaturatedEngine::checkCellsConnection,"Check cell connections.")
					.def("checkEntryCapillaryPressure",&UnsaturatedEngine::checkEntryCapillaryPressure,"Check entry capillary pressure between neighbor cells.")
					.def("checkLatticeNodeY",&UnsaturatedEngine::checkLatticeNodeY,(python::arg("y")),"Check the slice of lattice nodes for yNormal(y). 0: out of sphere; 1: inside of sphere.")
					.def("checkReservoirInfo",&UnsaturatedEngine::checkReservoirInfo,(python::arg("boundN")),"Check reservoir cells(N=2,3) statement and export to 'waterReservoirBoundInfo.txt' and 'airReservoirBoundInfo.txt'.")
					.def("checkBoundingCellsInfo",&UnsaturatedEngine::checkBoundingCellsInfo,"Check boundary cells (without reservoirs) statement and export to 'boundInfo.txt'.")
					.def("debugTemp",&UnsaturatedEngine::debugTemp,"debug temp file.(temporary)")
					.def("initializeCellWindowsID",&UnsaturatedEngine::initializeCellWindowsID,"Initialize cell windows index. A temp function for comparison with experiments, will delete soon")
					.def("invade",&UnsaturatedEngine::invade,"Run the drainage invasion.")
					.def("computeCapillaryForce",&UnsaturatedEngine::computeCapillaryForce,"Compute capillary force. ")
					)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(UnsaturatedEngine);
YADE_PLUGIN((UnsaturatedEngine));

UnsaturatedEngine::~UnsaturatedEngine(){}

void UnsaturatedEngine::testFunction()
{
	cout<<"This is UnsaturatedEngine test program"<<endl;
	RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
	if (tri.number_of_vertices()==0) {
		cout<< "triangulation is empty: building a new one" << endl;
		scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
		setPositionsBuffer(true);//copy sphere positions in a buffer...
		buildTriangulation(pZero,*solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
		initializeCellIndex();//initialize cell index
		updatePoreRadius();//save all pore radii before invade
		updateVolumeCapillaryCell();//save capillary volume of all cells, for fast calculating saturation
		computeSolidLine();//save cell->info().solidLine[j][y]
	}
	solver->noCache = true;
}

void UnsaturatedEngine::action()
{
    if ( !isActivated ) return;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if ( (tri.number_of_vertices()==0) || (updateTriangulation) ) {
        cout<< "triangulation is empty: building a new one" << endl;
        scene = Omega::instance().getScene().get();//here define the pointer to Yade's scene
        setPositionsBuffer(true);//copy sphere positions in a buffer...
        buildTriangulation(pZero,*solver);//create a triangulation and initialize pressure in the elements, everything will be contained in "solver"
        initializeCellIndex();//initialize cell index
        updatePoreRadius();//save all pore radii before invade
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
}

void UnsaturatedEngine::initializeCellIndex()
{
    int k=0;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        cell->info().index=k++;
    }
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
                neighbourCell->info().poreRadius[tri.mirror_index(cell, j)]= cell->info().poreRadius[j];
            }
        }
    }
}

void UnsaturatedEngine::updateVolumeCapillaryCell()
{
    initializeVolumes(*solver);
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    CellHandle neighbourCell;
    for (FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++) {
        cell->info().capillaryCellVolume = abs( cell->info().volume() ) - solver->volumeSolidPore(cell);
    }
}

void UnsaturatedEngine::invade()
{
    if (isPhaseTrapped) invade1();
    else invade2();
}

Real UnsaturatedEngine::getMinEntryValue()
{
    if (isPhaseTrapped) return getMinEntryValue1();
    else return getMinEntryValue2();
}

Real UnsaturatedEngine::getSaturation()
{
    if (isPhaseTrapped) return getSaturation1();
    else return getSaturation2();
}

///invade mode 1. update phase reservoir before invasion. Consider no viscous effects, and invade gradually.
void UnsaturatedEngine::invadeSingleCell1(CellHandle cell, double pressure)
{
    if (invadeBoundary==true) {
        for (int facet = 0; facet < 4; facet ++) {
            if (solver->T[solver->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
            if (cell->neighbor(facet)->info().Pcondition) continue;
            if (cell->neighbor(facet)->info().isWaterReservoir == true) {
                double nCellP = surfaceTension/cell->info().poreRadius[facet];
                if (pressure > nCellP) {
                    CellHandle nCell = cell->neighbor(facet);
                    nCell->info().p() = pressure;
                    nCell->info().isAirReservoir=true;
                    nCell->info().isWaterReservoir=false;
                    invadeSingleCell1(nCell, pressure);}}}}
    else {
        for (int facet = 0; facet < 4; facet ++) {
            if (solver->T[solver->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
            if (cell->neighbor(facet)->info().Pcondition) continue;//FIXME:defensive
            if (cell->neighbor(facet)->info().isFictious) continue;
            if (cell->neighbor(facet)->info().isWaterReservoir == true) {
                double nCellP = surfaceTension/cell->info().poreRadius[facet];
                if (pressure > nCellP) {
                    CellHandle nCell = cell->neighbor(facet);
                    nCell->info().p() = pressure;
                    nCell->info().isAirReservoir=true;
                    nCell->info().isWaterReservoir=false;
                    invadeSingleCell1(nCell, pressure);}}}}
}

void UnsaturatedEngine::invade1()
{
    updatePressureReservoir();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if(cell->info().isAirReservoir == true)
            invadeSingleCell1(cell,cell->info().p());
    }
    checkTrap(bndCondValue[3]);
    FiniteCellsIterator ncellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator ncell = tri.finite_cells_begin(); ncell != ncellEnd; ncell++ ) {
        if( (ncell->info().isWaterReservoir) || (ncell->info().isAirReservoir) ) continue;
        ncell->info().p() = bndCondValue[3] - ncell->info().trapCapP;
    }
    initReservoirBound();
}

//check trapped phase, define trapCapP.
void UnsaturatedEngine::checkTrap(double pressure)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if( (cell->info().isWaterReservoir) || (cell->info().isAirReservoir) ) continue;
      if(cell->info().trapCapP!=0) continue;
      cell->info().trapCapP=pressure;
    }
}

Real UnsaturatedEngine::getMinEntryValue1()
{
    updatePressureReservoir();
    Real nextEntry = 1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (invadeBoundary==true) {
        FiniteCellsIterator cellEnd = tri.finite_cells_end();
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().isAirReservoir == true) {
                for (int facet=0; facet<4; facet ++) {
                    if (tri.is_infinite(cell->neighbor(facet))) continue;
                    if (cell->neighbor(facet)->info().Pcondition) continue;
                    if ( cell->neighbor(facet)->info().isWaterReservoir == true  ) {
                        double nCellP = surfaceTension/cell->info().poreRadius[facet];
                        nextEntry = min(nextEntry,nCellP);}}}}}
    else {
        FiniteCellsIterator cellEnd = tri.finite_cells_end();
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().isAirReservoir == true) {
                for (int facet=0; facet<4; facet ++) {
                    if (tri.is_infinite(cell->neighbor(facet))) continue;
                    if (cell->neighbor(facet)->info().Pcondition) continue;//FIXME:defensive
                    if (cell->neighbor(facet)->info().isFictious) continue;
                    if ( cell->neighbor(facet)->info().isWaterReservoir == true  ) {
                        double nCellP = surfaceTension/cell->info().poreRadius[facet];
                        nextEntry = min(nextEntry,nCellP);}}}}}
    if (nextEntry==1e50) {
        cout << "End drainage !" << endl;
        return nextEntry=0;}
    else return nextEntry;
}

void UnsaturatedEngine::updatePressure()
{
    boundaryConditions(*solver);
    solver->pressureChanged=true;
    solver->reApplyBoundaryConditions();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().isWaterReservoir==true) cell->info().p()=bndCondValue[2];
      if (cell->info().isAirReservoir==true) cell->info().p()=bndCondValue[3];
    } 
}

//update pressure and reservoir attr
void UnsaturatedEngine::updatePressureReservoir()
{
    updatePressure();//NOTE:updatePressure must be run before update reservoirs.
    updateAirReservoir();
    updateWaterReservoir();  
}

//NOTE:keep boundingCells[2],boundingCells[3] always being reservoirs.
void UnsaturatedEngine::initReservoirBound()
{
    initWaterReservoirBound();
    initAirReservoirBound();
}

//boundingCells[2] is water reservoir. 
void UnsaturatedEngine::initWaterReservoirBound()
{
    if (solver->boundingCells[2].size()==0) {
        cerr<<"ERROR! set bndCondIsPressure[2] true. boundingCells.size=0!";
    }
    else {
        vector<CellHandle>::iterator it = solver->boundingCells[2].begin();
        for ( it ; it != solver->boundingCells[2].end(); it++) {
            if ((*it)->info().index == 0) continue;
            (*it)->info().isWaterReservoir = true;
        }
    }
}
void UnsaturatedEngine::updateWaterReservoir()
{    
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        cell->info().isWaterReservoir = false;
    }    
    initWaterReservoirBound();    
    vector<CellHandle>::iterator it = solver->boundingCells[2].begin();
    for ( it ; it != solver->boundingCells[2].end(); it++) {
        if ((*it)->info().index == 0) continue;
        waterReservoirRecursion(*it);
    }
}
void UnsaturatedEngine::waterReservoirRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        if (solver->T[solver->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().p() != bndCondValue[2]) continue;
        if (cell->neighbor(facet)->info().isWaterReservoir==true) continue;
        CellHandle nCell = cell->neighbor(facet);
        nCell->info().isWaterReservoir = true;
        waterReservoirRecursion(nCell);
    }
}
//boundingCells[3] is air reservoir
void UnsaturatedEngine::initAirReservoirBound()
{
    if (solver->boundingCells[3].size()==0) {
        cerr<<"ERROR! set bndCondIsPressure[3] true. boundingCells.size=0!";
    }
    else {
        vector<CellHandle>::iterator it = solver->boundingCells[3].begin();
        for ( it ; it != solver->boundingCells[3].end(); it++) {
            if((*it)->info().index == 0) continue;
            (*it)->info().isAirReservoir = true;
        }
    }
}

void UnsaturatedEngine::updateAirReservoir()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        cell->info().isAirReservoir = false;
    }
    initAirReservoirBound();
    vector<CellHandle>::iterator it = solver->boundingCells[3].begin();
    for ( it ; it != solver->boundingCells[3].end(); it++) {
        if ((*it)->info().index == 0) continue;
        airReservoirRecursion(*it);
    }
}

void UnsaturatedEngine::airReservoirRecursion(CellHandle cell)
{
    for (int facet = 0; facet < 4; facet ++) {
        if (solver->T[solver->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
        if (cell->neighbor(facet)->info().p() != bndCondValue[3]) continue;
        if (cell->neighbor(facet)->info().isAirReservoir == true) continue;
        CellHandle nCell = cell->neighbor(facet);
        nCell->info().isAirReservoir = true;
        airReservoirRecursion(nCell);
    }
}

Real UnsaturatedEngine::getSaturation1()
{
    updatePressureReservoir();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    Real capillaryVolume = 0.0; //total capillary volume
    Real airVolume = 0.0; 	//air volume
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    if (invadeBoundary==true) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;//NOTE:reservoirs cells should not be included in saturation
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().isAirReservoir==true) {
                airVolume = airVolume + cell->info().capillaryCellVolume;}}}
    else {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;
            if (cell->info().isFictious) continue;
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().isAirReservoir==true) {
                airVolume = airVolume + cell->info().capillaryCellVolume;}}}
    Real saturation = 1 - airVolume/capillaryVolume;
    return saturation;
}

///invade mode 2. Consider no trapped phase.
void UnsaturatedEngine::invadeSingleCell2(CellHandle cell, double pressure)
{
    if (invadeBoundary==true) {
        for (int facet = 0; facet < 4; facet ++) {
            if (solver->T[solver->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
            if (cell->neighbor(facet)->info().Pcondition) continue;
            if (cell->neighbor(facet)->info().p()!=0) continue;
            double nCellP = surfaceTension/cell->info().poreRadius[facet];
            if (pressure > nCellP) {
                CellHandle nCell = cell->neighbor(facet);
                nCell->info().p() = pressure;
                invadeSingleCell2(nCell, pressure);}}}
    else {
        for (int facet = 0; facet < 4; facet ++) {
            if (solver->T[solver->currentTes].Triangulation().is_infinite(cell->neighbor(facet))) continue;
            if (cell->neighbor(facet)->info().Pcondition) continue;//FIXME:defensive
            if (cell->neighbor(facet)->info().isFictious) continue;
            if (cell->neighbor(facet)->info().p()!=0) continue;
            double nCellP = surfaceTension/cell->info().poreRadius[facet];
            if (pressure > nCellP) {
                CellHandle nCell = cell->neighbor(facet);
                nCell->info().p() = pressure;
                invadeSingleCell2(nCell, pressure);}}}
}

void UnsaturatedEngine::invade2()
{
    updatePressure2();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (cell->info().p()!=0) {
            invadeSingleCell2(cell, cell->info().p());
        }
    }
}

void UnsaturatedEngine::updatePressure2()
{
    boundaryConditions(*solver);
    solver->pressureChanged=true;
    solver->reApplyBoundaryConditions();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().p()!=0) cell->info().p()=bndCondValue[3];
    }   
}

Real UnsaturatedEngine::getMinEntryValue2()
{
    Real nextEntry = 1e50;
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    if (invadeBoundary==true) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().p()!=0) {
                for (int facet=0; facet<4; facet ++) {
                    if (tri.is_infinite(cell->neighbor(facet))) continue;
                    if (cell->neighbor(facet)->info().Pcondition) continue;
                    if (cell->neighbor(facet)->info().p()==0) {
                        double nCellP = surfaceTension/cell->info().poreRadius[facet];
                        nextEntry = min(nextEntry,nCellP);}}}}}
    else {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (cell->info().p()!=0) {
                for (int facet=0; facet<4; facet ++) {
                    if (tri.is_infinite(cell->neighbor(facet))) continue;
                    if (cell->neighbor(facet)->info().Pcondition) continue;
                    if (cell->neighbor(facet)->info().isFictious) continue;//FIXME:defensive
                    if (cell->neighbor(facet)->info().p()==0) {
                        double nCellP = surfaceTension/cell->info().poreRadius[facet];
                        nextEntry = min(nextEntry,nCellP);}}}}}
    if (nextEntry==1e50) {
        cout << "End drainage !" << endl;
        return nextEntry=0;
    }
    else {
        return nextEntry;
    }
}

Real UnsaturatedEngine::getSaturation2()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    Real capillaryVolume = 0.0;
    Real waterVolume = 0.0;
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    if (invadeBoundary==true) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().p()==0) {
                waterVolume = waterVolume + cell->info().capillaryCellVolume;}}}
    else {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;
            if (cell->info().isFictious) continue;
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().p()==0) {
                waterVolume = waterVolume + cell->info().capillaryCellVolume;}}}
    Real saturation = waterVolume/capillaryVolume;
    return saturation;
}

double UnsaturatedEngine::computeEffPoreRadius(CellHandle cell, int j)
{
    double rInscribe = abs(solver->computeEffectiveRadius(cell, j));  
    CellHandle cellh = CellHandle(cell);
    int facetNFictious = solver->detectFacetFictiousVertices (cellh,j);
  switch (facetNFictious) {
    case (0) : { return computeEffPoreRadiusFine(cell,j); }; break;
    case (1) : { return rInscribe; }; break;
    case (2) : { return rInscribe; }; break;    
  }   
}
//seperate rmin=getMinPoreRadius(cell,j) later;
double UnsaturatedEngine::computeEffPoreRadiusFine(CellHandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Vector3r posA = makeVector3r(cell->vertex(facetVertices[j][0])->point().point());
    Vector3r posB = makeVector3r(cell->vertex(facetVertices[j][1])->point().point());
    Vector3r posC = makeVector3r(cell->vertex(facetVertices[j][2])->point().point());
    double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
    double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
    double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
    double AB = (posA-posB).norm();
    double AC = (posA-posC).norm();
    double BC = (posB-posC).norm();
    double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
    double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
    double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
    double rAB = 0.5*(AB-rA-rB); if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC); if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC); if (rAC<0) { rAC=0; }

    double rmin = max(rAB,max(rBC,rAC)); if (rmin==0) { rmin= 1.0e-10; }
    double rmax = abs(solver->computeEffectiveRadius(cell, j));//rmin>rmax ?
//     if(rmin>rmax) { cerr<<"WARNING! rmin>rmax. rmin="<<rmin<<" ,rmax="<<rmax<<endl; }
    
    double deltaForceRMin = computeDeltaForce(cell,j,rmin);
    double deltaForceRMax = computeDeltaForce(cell,j,rmax);
    if(deltaForceRMax<0) {
        double effPoreRadius = rmax;
//         cerr<<"deltaForceRMax Negative. cellID: "<<cell->info().index<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;
        return effPoreRadius;}
    else if(deltaForceRMin<0) {
        double effPoreRadius = bisection(cell,j,rmin,rmax);// cerr<<"1";//we suppose most cases should be this.
        return effPoreRadius;}
    else if( (deltaForceRMin>0) && (deltaForceRMin<deltaForceRMax) ) {
        double effPoreRadius = rmin;// cerr<<"2";
        return effPoreRadius;}
    else if(deltaForceRMin>deltaForceRMax) {
        double effPoreRadius = rmax;
//         cerr<<"WARNING! deltaForceRMin>deltaForceRMax. cellID: "<<cell->info().index<<"; deltaForceRMin="<<deltaForceRMin<<"; deltaForceRMax="<<deltaForceRMax<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;
        return effPoreRadius;}
}

double UnsaturatedEngine::bisection(CellHandle cell, int j, double a, double b)
{
    double m = 0.5*(a+b);
    if (abs(b-a)>abs((solver->computeEffectiveRadius(cell, j)*1.0e-6))) {
        if ( computeDeltaForce(cell,j,m) * computeDeltaForce(cell,j,a) < 0 ) {
            b = m;
            return bisection(cell,j,a,b);}
        else {
            a = m;
            return bisection(cell,j,a,b);}}
    else return m;
}

double UnsaturatedEngine::computeDeltaForce(CellHandle cell,int j, double rCap)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Vector3r posA = makeVector3r(cell->vertex(facetVertices[j][0])->point().point());
    Vector3r posB = makeVector3r(cell->vertex(facetVertices[j][1])->point().point());
    Vector3r posC = makeVector3r(cell->vertex(facetVertices[j][2])->point().point());
    double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
    double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
    double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
    double AB = (posA-posB).norm();
    double AC = (posA-posC).norm();
    double BC = (posB-posC).norm();    
    double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
    double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
    double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
    double rAB = 0.5*(AB-rA-rB); if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC); if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC); if (rAC<0) { rAC=0; }

    //In triangulation ArB,rCap is the radius of sphere r; 
    double _AB = (pow((rA+rCap),2)+pow(AB,2)-pow((rB+rCap),2))/(2*(rA+rCap)*AB); if(_AB>1.0) {_AB=1.0;} if(_AB<-1.0) {_AB=-1.0;}
    double alphaAB = acos(_AB);
    double _BA = (pow((rB+rCap),2)+pow(AB,2)-pow((rA+rCap),2))/(2*(rB+rCap)*AB); if(_BA>1.0) {_BA=1.0;} if(_BA<-1.0) {_BA=-1.0;}
    double alphaBA = acos(_BA);
    double _ArB = (pow((rA+rCap),2)+pow((rB+rCap),2)-pow(AB,2))/(2*(rA+rCap)*(rB+rCap)); if(_ArB>1.0) {_ArB=1.0;} if(_ArB<-1.0) {_ArB=-1.0;}
    double alphaArB = acos(_ArB);
    
    double lengthLiquidAB = alphaArB*rCap;
    double AreaArB = 0.5*(rA+rCap)*(rB+rCap)*sin(alphaArB);
    double areaLiquidAB = AreaArB-0.5*alphaAB*pow(rA,2)-0.5*alphaBA*pow(rB,2)-0.5*alphaArB*pow(rCap,2);

    double _AC = (pow((rA+rCap),2)+pow(AC,2)-pow((rC+rCap),2))/(2*(rA+rCap)*AC); if(_AC>1.0) {_AC=1.0;} if(_AC<-1.0) {_AC=-1.0;}
    double alphaAC = acos(_AC);
    double _CA = (pow((rC+rCap),2)+pow(AC,2)-pow((rA+rCap),2))/(2*(rC+rCap)*AC); if(_CA>1.0) {_CA=1.0;} if(_CA<-1.0) {_CA=-1.0;}
    double alphaCA = acos(_CA);
    double _ArC = (pow((rA+rCap),2)+pow((rC+rCap),2)-pow(AC,2))/(2*(rA+rCap)*(rC+rCap)); if(_ArC>1.0) {_ArC=1.0;} if(_ArC<-1.0) {_ArC=-1.0;}
    double alphaArC = acos(_ArC);

    double lengthLiquidAC = alphaArC*rCap;
    double AreaArC = 0.5*(rA+rCap)*(rC+rCap)*sin(alphaArC);
    double areaLiquidAC = AreaArC-0.5*alphaAC*pow(rA,2)-0.5*alphaCA*pow(rC,2)-0.5*alphaArC*pow(rCap,2);

    double _BC = (pow((rB+rCap),2)+pow(BC,2)-pow((rC+rCap),2))/(2*(rB+rCap)*BC); if(_BC>1.0) {_BC=1.0;} if(_BC<-1.0) {_BC=-1.0;}
    double alphaBC = acos(_BC);
    double _CB = (pow((rC+rCap),2)+pow(BC,2)-pow((rB+rCap),2))/(2*(rC+rCap)*BC); if(_CB>1.0) {_CB=1.0;} if(_CB<-1.0) {_CB=-1.0;}
    double alphaCB = acos(_CB);
    double _BrC = (pow((rB+rCap),2)+pow((rC+rCap),2)-pow(BC,2))/(2*(rB+rCap)*(rC+rCap)); if(_BrC>1.0) {_BrC=1.0;} if(_BrC<-1.0) {_BrC=-1.0;}
    double alphaBrC = acos(_BrC);

    double lengthLiquidBC = alphaBrC*rCap;
    double AreaBrC = 0.5*(rB+rCap)*(rC+rCap)*sin(alphaBrC);
    double areaLiquidBC = AreaBrC-0.5*alphaBC*pow(rB,2)-0.5*alphaCB*pow(rC,2)-0.5*alphaBrC*pow(rCap,2);

    double areaCap = sqrt(cell->info().facetSurfaces[j].squared_length()) * cell->info().facetFluidSurfacesRatio[j];
    double areaPore = areaCap - areaLiquidAB - areaLiquidAC - areaLiquidBC;
    
    //FIXME:rethink here,areaPore Negative, Flat facets, do nothing ?
//     if(areaPore<0) {cerr<<"ERROR! areaPore Negative. cellID: "<<cell->info().index<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;}
    double perimeterPore = lengthLiquidAB + lengthLiquidAC + lengthLiquidBC + (A - alphaAB - alphaAC)*rA + (B - alphaBA - alphaBC)*rB + (C - alphaCA - alphaCB)*rC;
//     if(perimeterPore<0) {cerr<<"ERROR! perimeterPore Negative. cellID: "<<cell->info().index<<". posA="<<posA<<"; posB="<<posB<<"; posC="<< posC<<"; rA="<< rA<<"; rB="<< rB<<"; rC="<<rC<<endl;}

    double deltaForce = perimeterPore - areaPore/rCap;//deltaForce=surfaceTension*(perimeterPore - areaPore/rCap)
    return deltaForce;
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
        file << cell->info().index << " " <<cell->neighbor(0)->info().index << " " << surfaceTension/cell->info().poreRadius[0] << endl;
        file << cell->info().index << " " <<cell->neighbor(1)->info().index << " " << surfaceTension/cell->info().poreRadius[1] << endl;
        file << cell->info().index << " " <<cell->neighbor(2)->info().index << " " << surfaceTension/cell->info().poreRadius[2] << endl;
        file << cell->info().index << " " <<cell->neighbor(3)->info().index << " " << surfaceTension/cell->info().poreRadius[3] << endl;
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
        Real deltaX = (solver->xMax-solver->xMin)/N;
        Real deltaZ = (solver->zMax-solver->zMin)/N;
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
    file << "#Checking the boundingCells statement";
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
            file << "#Checking the water reservoir cells statement";
            file << "CellID" << "	CellPressure" << "	isAirReservoir" << "	isWaterReservoir" <<endl;
            vector<CellHandle>::iterator it = solver->boundingCells[boundN].begin();
            for ( it ; it != solver->boundingCells[boundN].end(); it++) {
                if ((*it)->info().index == 0) continue;
                file << (*it)->info().index <<" "<<(*it)->info().p()<<" "<<(*it)->info().isAirReservoir<<" "<<(*it)->info().isWaterReservoir<<endl;
            }
            file.close();
        }
        else if (boundN==3) {
            ofstream file;
            file.open("airReservoirBoundInfo.txt");
            file << "#Checking the air reservoir cells statement";
            file << "CellID"<<"	CellPressure"<<"	isAirReservoir"<<"	isWaterReservoir"<<endl;
            vector<CellHandle>::iterator it = solver->boundingCells[boundN].begin();
            for ( it ; it != solver->boundingCells[boundN].end(); it++) {
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

//----temp function for Vahid Joekar-Niasar's data----
double UnsaturatedEngine::getRadiusMin(CellHandle cell, int j)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    if (tri.is_infinite(cell->neighbor(j))) return 0;

    Vector3r posA = makeVector3r(cell->vertex(facetVertices[j][0])->point().point());
    Vector3r posB = makeVector3r(cell->vertex(facetVertices[j][1])->point().point());
    Vector3r posC = makeVector3r(cell->vertex(facetVertices[j][2])->point().point());
    double rA = sqrt(cell->vertex(facetVertices[j][0])->point().weight());
    double rB = sqrt(cell->vertex(facetVertices[j][1])->point().weight());
    double rC = sqrt(cell->vertex(facetVertices[j][2])->point().weight());
    double AB = (posA-posB).norm();
    double AC = (posA-posC).norm();
    double BC = (posB-posC).norm();
    double A = acos(((posB-posA).dot(posC-posA))/((posB-posA).norm()*(posC-posA).norm()));
    double B = acos(((posA-posB).dot(posC-posB))/((posA-posB).norm()*(posC-posB).norm()));
    double C = acos(((posA-posC).dot(posB-posC))/((posA-posC).norm()*(posB-posC).norm()));
    double rAB = 0.5*(AB-rA-rB); if (rAB<0) { rAB=0; }
    double rBC = 0.5*(BC-rB-rC); if (rBC<0) { rBC=0; }
    double rAC = 0.5*(AC-rA-rC); if (rAC<0) { rAC=0; }  

    double rmin = max(rAB,max(rBC,rAC)); if (rmin==0) { rmin= 1.0e-10; }
    return rmin;
}

void UnsaturatedEngine::debugTemp()
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    ofstream file;
    file.open("bugTemp.txt");
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
        if (tri.is_infinite(cell)) continue;
	file << cell->info().index << "  " <<cell->info().poreRadius[0]<<" "<<getRadiusMin(cell,0)<<" "<<abs(solver->computeEffectiveRadius(cell, 0))<<endl;
	file << cell->info().index << "  " <<cell->info().poreRadius[1]<<" "<<getRadiusMin(cell,1)<<" "<<abs(solver->computeEffectiveRadius(cell, 1))<<endl;
	file << cell->info().index << "  " <<cell->info().poreRadius[2]<<" "<<getRadiusMin(cell,2)<<" "<<abs(solver->computeEffectiveRadius(cell, 2))<<endl;
	file << cell->info().index << "  " <<cell->info().poreRadius[3]<<" "<<getRadiusMin(cell,3)<<" "<<abs(solver->computeEffectiveRadius(cell, 3))<<endl;
    }
    file.close();
}//----------end temp function for Vahid Joekar-Niasar's data (clear later)---------------------

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

Real UnsaturatedEngine::getWindowsSaturation(int windowsID)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
      if (cell->info().windowsID==0) {cerr<<"Please initialize windowsID"<<endl;break;}
    }
    if (isPhaseTrapped) {
        return getWindowsSaturation1(windowsID);
    }
    else {
        return getWindowsSaturation2(windowsID);
    }
}
Real UnsaturatedEngine::getWindowsSaturation1(int i)
{
    updatePressureReservoir();
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    Real capillaryVolume = 0.0; //total capillary volume
    Real airVolume = 0.0; 	//air volume
    FiniteCellsIterator cellEnd = tri.finite_cells_end();

    if (invadeBoundary==true) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;//NOTE:reservoirs cells should not be included in saturation
	    if (cell->info().windowsID != i) continue;
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().isAirReservoir==true) {
                airVolume = airVolume + cell->info().capillaryCellVolume;}}}
    else {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;
            if (cell->info().isFictious) continue;
	    if (cell->info().windowsID != i) continue;
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().isAirReservoir==true) {
                airVolume = airVolume + cell->info().capillaryCellVolume;}}}
    Real saturation = 1 - airVolume/capillaryVolume;
    return saturation;
}
Real UnsaturatedEngine::getWindowsSaturation2(int i)
{
    RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
    Real capillaryVolume = 0.0;
    Real waterVolume = 0.0;
    FiniteCellsIterator cellEnd = tri.finite_cells_end();
    if (invadeBoundary==true) {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;
	    if (cell->info().windowsID != i) continue;
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().p()==0) {
                waterVolume = waterVolume + cell->info().capillaryCellVolume;}}}
    else {
        for ( FiniteCellsIterator cell = tri.finite_cells_begin(); cell != cellEnd; cell++ ) {
            if (tri.is_infinite(cell)) continue;
            if (cell->info().Pcondition) continue;
            if (cell->info().isFictious) continue;
	    if (cell->info().windowsID != i) continue;
            capillaryVolume = capillaryVolume + cell->info().capillaryCellVolume;
            if (cell->info().p()==0) {
                waterVolume = waterVolume + cell->info().capillaryCellVolume;}}}
    Real saturation = waterVolume/capillaryVolume;
    return saturation;
}//----------end temp functions for comparison with experiment-------------------

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
}

void UnsaturatedEngine::computeFacetPoreForcesWithCache(bool onlyCache)
{
	RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	FiniteCellsIterator cellEnd = Tri.finite_cells_end();
	CVector nullVect(0,0,0);
	//reset forces
	if (!onlyCache) for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) v->info().forces=nullVect;
	
// 	#ifdef parallel_forces
// 	if (solver->noCache) {
// 		solver->perVertexUnitForce.clear(); solver->perVertexPressure.clear();
// 		solver->perVertexUnitForce.resize(solver->T[currentTes].maxId+1);
// 		solver->perVertexPressure.resize(solver->T[currentTes].maxId+1);}
// 	#endif
	CellHandle neighbourCell;
	VertexHandle mirrorVertex;
	CVector tempVect;
	//FIXME : Ema, be carefull with this (noCache), it needs to be turned true after retriangulation
	if (solver->noCache) {for (FlowSolver::VCellIterator cellIt=solver->T[currentTes].cellHandles.begin(); cellIt!=solver->T[currentTes].cellHandles.end(); cellIt++){
			CellHandle& cell = *cellIt;
			//reset cache
			for (int k=0;k<4;k++) cell->info().unitForceVectors[k]=nullVect;

			for (int j=0; j<4; j++) if (!Tri.is_infinite(cell->neighbor(j))) {
					neighbourCell = cell->neighbor(j);
					const CVector& Surfk = cell->info().facetSurfaces[j];
					//FIXME : later compute that fluidSurf only once in hydraulicRadius, for now keep full surface not modified in cell->info for comparison with other forces schemes
					//The ratio void surface / facet surface
					Real area = sqrt(Surfk.squared_length()); if (area<=0) cerr <<"AREA <= 0!!"<<endl;
					CVector facetNormal = Surfk/area;
					const std::vector<CVector>& crossSections = cell->info().facetSphereCrossSections;
					CVector fluidSurfk = cell->info().facetSurfaces[j]*cell->info().facetFluidSurfacesRatio[j];
					/// handle fictious vertex since we can get the projected surface easily here
					if (cell->vertex(j)->info().isFictious) {
						Real projSurf=abs(Surfk[solver->boundary(cell->vertex(j)->info().id()).coordinate]);
						tempVect=-projSurf*solver->boundary(cell->vertex(j)->info().id()).normal;
						cell->vertex(j)->info().forces = cell->vertex(j)->info().forces+tempVect*cell->info().p();
						//define the cached value for later use with cache*p
						cell->info().unitForceVectors[j]=cell->info().unitForceVectors[j]+ tempVect;
					}
					/// Apply weighted forces f_k=sqRad_k/sumSqRad*f
					CVector facetUnitForce = -fluidSurfk*cell->info().solidLine[j][3];
					CVector facetForce = cell->info().p()*facetUnitForce;
										
					for (int y=0; y<3;y++) {
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
// 					#ifdef parallel_forces
// 					solver->perVertexUnitForce[cell->vertex(j)->info().id()].push_back(&(cell->info().unitForceVectors[j]));
// 					solver->perVertexPressure[cell->vertex(j)->info().id()].push_back(&(cell->info().p()));
// 					#endif
			}
		}
		solver->noCache=false;//cache should always be defined after execution of this function
		if (onlyCache) return;
	} else {//use cached values when triangulation doesn't change
// 		#ifndef parallel_forces
		for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
			for (int yy=0;yy<4;yy++) cell->vertex(yy)->info().forces = cell->vertex(yy)->info().forces + cell->info().unitForceVectors[yy]*cell->info().p();}
			
//  		#else
// 		#pragma omp parallel for num_threads(ompThreads)
// 		for (int vn=0; vn<= solver->T[currentTes].maxId; vn++) {
// 			VertexHandle& v = solver->T[currentTes].vertexHandles[vn];
// 			const int& id =  v->info().id();
// 			CVector tf (0,0,0);
// 			int k=0;
// 			for (vector<const Real*>::iterator c = solver->perVertexPressure[id].begin(); c != solver->perVertexPressure[id].end(); c++)
// 				tf = tf + (*(solver->perVertexUnitForce[id][k++]))*(**c);
// 			v->info().forces = tf;
// 		}
// 		#endif
	}
	if (solver->debugOut) {
		CVector totalForce = nullVect;
		for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v)	{
			if (!v->info().isFictious) totalForce = totalForce + v->info().forces;
			else if (solver->boundary(v->info().id()).flowCondition==1) totalForce = totalForce + v->info().forces;	}
		cout << "totalForce = "<< totalForce << endl;}
}

#endif //UNSATURATED_FLOW
#endif //FLOW_ENGINE