 
/*************************************************************************
*  Copyright (C) 2014 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2013 by Thomas. Sweijen <T.sweijen@uu.nl>               *
*  Copyright (C) 2012 by Chao Yuan <chao.yuan@3sr-grenoble.fr>           *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


// This is an example of how to derive a new FlowEngine with additional data and possibly completely new behaviour.
// Every functions of the base engine can be overloaded, and new functions can be added

//keep this #ifdef as long as you don't really want to realize a final version publicly, it will save compilation time for everyone else
//when you want it compiled, you can pass -DTWOPHASEFLOW to cmake, or just uncomment the following line
//#define TWOPHASEFLOW
#ifdef TWOPHASEFLOW
#include "FlowEngine_TwoPhaseFlowEngineT.hpp"
// #include <Eigen/Sparse>
// #include <Eigen/SparseCore>
// #include <Eigen/CholmodSupport>
// #include <Eigen/SparseCholesky>
// #include <Eigen/IterativeLinearSolvers>
#include<Eigen/SparseLU>
/// We can add data to the Info types by inheritance
class TwoPhaseCellInfo : public FlowCellInfo_TwoPhaseFlowEngineT
{
	public:
  	bool isWRes;//Flags for marking cell(pore unit) state: isWettingReservoirCell, isNonWettingReservoirCell, isTrappedWettingCell, isTrappedNonWettingCell
	bool isNWRes;
	bool isTrapW;
	bool isTrapNW;
	double saturation;//the saturation of single pore (will be used in quasi-static imbibition and dynamic flow)
	double trapCapP;//for calculating the pressure of trapped pore, cell->info().p() = pressureNW- trapCapP. OR cell->info().p() = pressureW + trapCapP
	bool hasInterface; //Indicated whether a NW-W interface is present within the pore body
	std::vector<double> poreThroatRadius;
	std::vector<double> kWater;	
	double poreBodyRadius;
	double poreBodyVolume;
	int windowsID;//a temp cell info for experiment comparison(used by chao)
	double solidLine [4][4];//the length of intersecting line between sphere and facet. [i][j] is for facet "i" and sphere (facetVertices)"[i][j]". Last component [i][3] for 1/sumLines in the facet "i" (used by chao).
	
	//DynamicTwoPhaseFlow 
	double saturation2;
	int numberFacets;
	int isFictiousId;
	int errorCount;
	double mergedVolume;
	int mergednr;
	unsigned int mergedID;
	double thresholdSaturation;
	std::vector<double> entryPressure;
	std::vector<double> entrySaturation;
	double capillaryPressure;
	bool isNewlyUpdated;
	double flux;
	double ptemp;
	bool hasInterfaceTEMP;
	bool isBC;
	bool skip;
	bool updateDyn;
	double truncatedVolume;
	double lengthLoss;
	std::vector<double> kNormB;
	std::vector<int> poreNeighbors;
	std::vector<int> poreIdConnectivity;
	std::vector<double> listOfkNorm;
	std::vector<double> listOfkNorm2;
	std::vector<double> listOfEntrySaturation;
	std::vector<double> listOfEntryPressure;
	std::vector<double> kNorm2;
	std::vector<double> listOfThroatArea;
	double accumulativeDV; 	
	double airWaterArea;
	bool isWResInternal;
      	double conductivityWRes;

	double saturationMax;
	double interfacialAreaWA;
	double maxSaturation;
	double minSaturation;
	int poreId;
	double invasionPc;
	double hainesJump;	     
	double thresholdPressure;
	double dvTPF;
	bool isNWResDef;
	int invadedFrom;
	TwoPhaseCellInfo (void)
	{
		saturation2 = 0.0;
		isFictiousId = 0;
		isWRes = true; isNWRes = false; isTrapW = false; isTrapNW = false;
		saturation = 1.0;
		hasInterface = false;
		trapCapP = 0;
		poreThroatRadius.resize(4, 0);
		kNorm2.resize(4,0);
		poreBodyRadius = 0;
		poreBodyVolume = 0;
		windowsID = 0;
		for (int k=0; k<4;k++) for (int l=0; l<4;l++) solidLine[k][l]=0;
		
		//dynamic TwoPhaseFlow
		numberFacets = 4;
		mergedVolume = 0;
		mergednr = 0;
		mergedID = 0;
		errorCount = 0;
		entryPressure.resize(4,0);
		entrySaturation.resize(4,0);
		poreIdConnectivity.resize(4,-1);
		kWater.resize(4,0);
		thresholdSaturation = 0;
		capillaryPressure = 0;
		flux = 0.0;
		isNewlyUpdated = false;
		ptemp = 0.0;
		hasInterfaceTEMP = false;
		updateDyn = false;
		isBC = false;
		truncatedVolume = 0.0;
		lengthLoss = 0.0;
		kNormB.resize(4,0);
		saturationMax = 0.0;
		interfacialAreaWA = 0.0;
		skip = false;
		accumulativeDV = 0.0;
		thresholdPressure = 0.0;
		invasionPc = 0.0;
		airWaterArea = 0.0;
		accumulativeDV = 0.0;
		maxSaturation = 1.0;
		minSaturation = 0.0;
		poreId = -1;
		hainesJump = 0.0;
		isWResInternal = false;
		dvTPF = 0.0;
		isNWResDef = false;
		conductivityWRes = 0.0;
		invadedFrom = 0;
	}
	
};

class TwoPhaseVertexInfo : public FlowVertexInfo_TwoPhaseFlowEngineT {
	public:
	//same here if needed
};

typedef TemplateFlowEngine_TwoPhaseFlowEngineT<TwoPhaseCellInfo,TwoPhaseVertexInfo> TwoPhaseFlowEngineT;
REGISTER_SERIALIZABLE(TwoPhaseFlowEngineT);

class TwoPhaseFlowEngine : public TwoPhaseFlowEngineT
{
		double totalCellVolume;
		double airBoundaryPressure = 0.0;
		std::vector<CellHandle> listOfPores;
		bool imposeDeformationFluxTPFSwitch =false;
		
	public :
	//We can overload every functions of the base engine to make it behave differently
	//if we overload action() like this, this engine is doing nothing in a standard timestep, it can still have useful functions
	virtual void action() {};
	
	//If a new function is specific to the derived engine, put it here, else go to the base TemplateFlowEngine
	//if it is useful for everyone
	void computePoreBodyVolume();	
 	void computePoreBodyRadius();
// 	void computePoreThroatCircleRadius();
// 	double computePoreSatAtInterface(int ID);
	double computePoreCapillaryPressure(CellHandle cell);
	void savePhaseVtk(const char* folder);
// 	void computePoreThroatRadius();
// 	void computePoreThroatRadiusTricky();//set the radius of pore throat between side pores negative.
	
	double computeEffPoreThroatRadius(CellHandle cell, int j);
	double computeEffPoreThroatRadiusFine(CellHandle cell, int j);
	double bisection(CellHandle cell, int j, double a, double b);
	double computeTriRadian(double a, double b, double c);
	double computeDeltaForce(CellHandle cell,int j, double rC);
	void initialization();
	void computeSolidLine();
	void initializeReservoirs();
	
	void computePoreThroatRadiusMethod1();
	void computePoreThroatRadiusTrickyMethod1();//set the radius of pore throat between side pores negative.
	void computePoreThroatRadiusMethod2();
	void computePoreThroatRadiusMethod3();
	void savePoreNetwork();
	
	
	
	//Library TwoPhaseFlow
	double getKappa(int numberFacets);
	double getChi(int numberFacets);
 	double getLambda(int numberFacets);
	double getN(int numberFacets);
	double getDihedralAngle(int numberFacets);

	//Merging Library
	void mergeCells();
	void countFacets();
	void computeMergedVolumes();
	void getMergedCellStats();
	void calculateResidualSaturation();
	void adjustUnresolvedPoreThroatsAfterMerging();
 	void actionMergingAlgorithm();
	void checkVolumeConservationAfterMergingAlgorithm();

	//Dynamic Engine
	void actionTPF();
	void solvePressure();
	void setBoundaryConditions();
	void setInitialConditions();
	void setPoreNetwork();
	void setListOfPores();
	void getQuantities();
	double porePressureFromPcS(CellHandle cell, double saturation);

	double getConstantC4(CellHandle cell);  
	double getConstantC3(CellHandle cell);
	double dsdp(CellHandle cell, double pw);
	double poreSaturationFromPcS(CellHandle cell,double pw);
	
	void reTriangulate();
	void readTriangulation();
	void initializationTriangulation();
	void assignWaterVolumesTriangulation();
	void equalizeSaturationOverMergedCells();
	void updatePoreUnitProperties();
	void transferConditions();
	void imposeDeformationFluxTPF();
	void updateDeformationFluxTPF();
	
	std::vector<double> leftOverVolumePerSphere;
	std::vector<double> untreatedAreaPerSphere;
	std::vector<unsigned int> finishedUpdating;
	std::vector<double> waterVolume;
	std::vector< std::vector<unsigned int> > tetrahedra;
	std::vector< std::vector<double> > solidFractionSpPerTet;
	std::vector<double> deltaVoidVolume;
	std::vector<double> leftOverDVPerSphere;
	std::vector<double> saturationList;
	std::vector<bool> hasInterfaceList;
	std::vector<double> listOfFlux;
	
	Eigen::SparseMatrix<double> aMatrix;
	typedef Eigen::Triplet<double> ETriplet;
	std::vector<ETriplet> tripletList;
	Eigen::SparseLU<Eigen::SparseMatrix<double,Eigen::ColMajor>,Eigen::COLAMDOrdering<int> > eSolver;
	
	int getCell2(double posX, double posY, double posZ){	//Should be fixed properly
	  RTriangulation& tri = solver->T[solver->currentTes].Triangulation();
	  CellHandle cell = tri.locate(Point(posX,posY,posZ));
	  return cell->info().id;
	}

	boost::python::list cellporeThroatConductivity(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
	  boost::python::list ids;
	  if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
	  for (unsigned int i=0;i<4;i++) ids.append(solver->T[solver->currentTes].cellHandles[id]->info().kNorm() [i]);
	return ids;
	}
	
	boost::python::list cellporeThroatRadius(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
	  boost::python::list ids;
	  if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
	  for (unsigned int i=0;i<4;i++) ids.append(solver->T[solver->currentTes].cellHandles[id]->info().poreThroatRadius[i]);
	return ids;
	}

	boost::python::list getNeighbors(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
	  boost::python::list ids;
	  if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
	  for (unsigned int i=0;i<4;i++) ids.append(solver->T[solver->currentTes].cellHandles[id]->neighbor(i)->info().id);
	return ids;
	}
	
	
	//Dynamic code
	boost::python::list cellEntrySaturation(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
	  boost::python::list ids;
	  if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
	  for (unsigned int i=0;i<4;i++) ids.append(solver->T[solver->currentTes].cellHandles[id]->info().entrySaturation[i]);
	return ids;
	}


	
	
	//FIXME, needs to trigger initSolver() Somewhere, else changing flow.debug or other similar things after first calculation has no effect
	//FIXME, I removed indexing cells from inside UnsatEngine (SoluteEngine shouldl be ok (?)) in order to get pressure computed, problem is they are not indexed at all if flow is not calculated
	void computeOnePhaseFlow() {scene = Omega::instance().getScene().get(); initSolver(*solver); if (!solver) cerr<<"no solver! \n"<<endl; solver->gaussSeidel(scene->dt);/*cout << "Engine2 \n";initSolver(*solver);cout << "Engine3 \n";*/}
	CELL_SCALAR_GETTER(bool,.isWRes,cellIsWRes)
	CELL_SCALAR_GETTER(bool,.isNWRes,cellIsNWRes)
	CELL_SCALAR_GETTER(bool,.isTrapW,cellIsTrapW)
	CELL_SCALAR_GETTER(bool,.isTrapNW,cellIsTrapNW)
	CELL_SCALAR_SETTER(bool,.isNWRes,setCellIsNWRes)
	CELL_SCALAR_SETTER(bool,.isWRes,setCellIsWRes)
	CELL_SCALAR_GETTER(Real,.saturation,cellSaturation)
	CELL_SCALAR_SETTER(Real,.saturation,setCellSaturation)
	CELL_SCALAR_GETTER(bool,.isFictious,cellIsFictious) //Temporary function to allow for simulations in Python
	CELL_SCALAR_GETTER(bool,.hasInterface,cellHasInterface) //Temporary function to allow for simulations in Python
	CELL_SCALAR_GETTER(Real,.poreBodyRadius,cellInSphereRadius) //Temporary function to allow for simulations in Python	
	CELL_SCALAR_GETTER(Real,.poreBodyVolume,cellVoidVolume) //Temporary function to allow for simulations in Python	
	CELL_SCALAR_GETTER(Real,.mergedVolume,cellMergedVolume) //Temporary function to allow for simulations in Python	
	CELL_SCALAR_SETTER(bool,.hasInterface,setCellHasInterface) //Temporary function to allow for simulations in Python
	CELL_SCALAR_SETTER(Real,.dvTPF,setCellDV) //Temporary function to allow for simulations in Python
// 	CELL_SCALAR_SETTER(bool,.isWRes,cellIsWRes) //Temporary function to allow for simulations in Python

	//Dynamic Code
	CELL_SCALAR_GETTER(Real,.thresholdSaturation,cellThresholdSaturation) //Temporary function to allow for simulations in Python	
	CELL_SCALAR_GETTER(Real,.mergedID,cellMergedID) //Temporary function to allow for simulations in Python	

	
	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(TwoPhaseFlowEngine,TwoPhaseFlowEngineT,"documentation here",
	((double,surfaceTension,0.0728,,"Water Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))
	((bool,initialWetting,true,,"Initial wetting saturated (=true) or non-wetting saturated (=false)"))
	((bool, isPhaseTrapped,true,,"If True, both phases can be entrapped by the other, which would correspond to snap-off. If false, both phases are always connected to their reservoirs, thus no snap-off."))
	((bool, isInvadeBoundary, true,,"Invasion side boundary condition. If True, pores of side boundary can be invaded; if False, the pore throats connecting side boundary are closed, those pores are excluded in saturation calculation."))	
	((bool, drainageFirst, true,,"If true, activate drainage first (initial saturated), then imbibition; if false, activate imbibition first (initial unsaturated), then drainage."))
	((double,dtDynTPF,0.0,,"Parameter which stores the smallest time step, based on the residence time"))
	((int,entryPressureMethod,1,,"integer to define the method used to determine the pore throat radii and the according entry pressures. 1)radius of entry pore throat based on MS-P method; 2) radius of the inscribed circle; 3) radius of the circle with equivalent surface area of the pore throat."))
	((double,partiallySaturatedPores,false,,"Include partially saturated pores or not?"))
	((double,entryMethodCorrection,float(entryPressureMethod),,"Parameter that corrects various entry pressure methods "))


	
	//Dynamic TwoPhaseFlow
	((vector<bool>, bndCondIsWaterReservoir, vector<bool>(6,false),,"Water reservoir?"))
	((unsigned int,maxIDMergedCells,0,,"maximum number of merged ID, this is computed in mergeCells()"))
	((int,iterNR,0,,"Number of time steps done during dynamic flow simulations"))
	((double,capillaryPressure,0.0,,"Capillary pressure based on the volume averaged water pressure in all pores"))
	((double,waterPressurePartiallySatPores,0.0,,"Capillary pressure based on the volume averaged water pressure in partially saturated pores"))
	((double,waterPressure,0.0,,"Volume averaged water pressure"))
	((double,waterSaturation,0.0,,"Water saturation, excluding the boundary cells"))
	((double,voidVolume,0.0,,"total pore space"))
  	((bool,stopSimulation, false,," No event is happening, thus stop the simulation! "))
  	((bool,debugTPF, false,," No event is happening, thus stop the simulation! "))
	((double,airWaterInterfacialArea,0.0,,"Air water interfacial area"))
	((double,areaAveragedPressure,0.0,,"Air water interfacial area averaged water pressure "))
	((double,waterPressure_NHJ,0.0,,"water pressure without pores undergoing a haines jump"))


	((double,maximumRatioPoreThroatoverPoreBody,0.90,,"maximum ratio of pore throat radius over pore body radius, this is used during merging of tetrahedra."))
	((double,totalWaterVolume,0.0,,"total water volume"))
	((string,modelRunName,"dynamicImbibition",,"Python command to be run when remeshing. Anticipated usage: define blocked cells (see also :yref:`TemplateFlowEngine_@TEMPLATE_FLOW_NAME@.blockCell`), or apply exotic types of boundary conditions which need to visit the newly built mesh"))
	((double,safetyFactorTimeStep,1.0,,"Safey coefficient for time step"))
	((bool, makeMovie, false,,"Make movie? "))
	((double,fluxInViaWBC,0.0,,"Total water flux over water boundary conditions"))
	((double, accumulativeFlux,0.0,,"accumulative influx of water"))
	((double, truncationPrecision,1e-6,,"threshold at which a saturation is truncated"))
	((unsigned int, numberOfPores, 0,,"Number of pores (i.e. number of tetrahedra, but compensated for merged tetrahedra"))
	((bool, firstDynTPF, true,,"this bool activated the initialization of the dynamic flow engine, such as merging and defining initial values"))
	((bool, keepTriangulation, false,,"this bool activated triangulation or not during initialization"))
	((double, hainesVelocity,0.001,,"threshold at which a saturation is truncated"))
	((double, factorHJ,-1.0,,"threshold at which a saturation is truncated"))
 	((bool, remesh, false,,"update triangulation?"))
	((bool, deformation, false,,"update triangulation?"))
	((int, iterationTPF, -1,,"Iteration number"))
	((double, minWaterPressure, -30000.0,,"Iteration number"))
	((double, initialPC, 2000.0,,"Iteration number"))
	((double, sensorPressure, 0.0,,"Water pressure associated with a sensor"))
	((double, accumulativeDeformationFlux, 0.0,,"accumulative internal flux caused by deformation"))
	((bool, solvePressureSwitch, true,,"solve for pressure during actionTPF()"))
	((double, deltaTimeTruncation, 0.0,,"truncation of time step, to avoid very small time steps during local imbibition, NOTE it does affect the mass conservation not set to 0"))
	((double, waterBoundaryPressure, 0.0,,"Water pressure at boundary used in computations, is set automaticaly. "))
	((double, waterVolumeTruncatedLost, 0.0,,"Water volume that has been truncated."))
	((double, maxCapillaryPressure, 0.0,,"Water volume that has been truncated."))
	((bool, getQuantitiesUpdateCont, false,,"solve for pressure during actionTPF()"))


	


	,/*TwoPhaseFlowEngineT()*/,
	,
	.def("getCellIsFictious",&TwoPhaseFlowEngine::cellIsFictious,"Check the connection between pore and boundary. If true, pore throat connects the boundary.")
	.def("setCellIsNWRes",&TwoPhaseFlowEngine::setCellIsNWRes,"set status whether 'wetting reservoir' state")
	.def("setCellIsWRes",&TwoPhaseFlowEngine::setCellIsWRes,"set status whether 'wetting reservoir' state")
	.def("savePhaseVtk",&TwoPhaseFlowEngine::savePhaseVtk,(boost::python::arg("folder")="./phaseVtk"),"Save the saturation of local pores in vtk format. Sw(NW-pore)=0, Sw(W-pore)=1. Specify a folder name for output.")
	.def("getCellIsWRes",&TwoPhaseFlowEngine::cellIsWRes,"get status wrt 'wetting reservoir' state")
	.def("getCellIsNWRes",&TwoPhaseFlowEngine::cellIsNWRes,"get status wrt 'non-wetting reservoir' state")
	.def("getCellIsTrapW",&TwoPhaseFlowEngine::cellIsTrapW,"get status wrt 'trapped wetting phase' state")
	.def("getCellIsTrapNW",&TwoPhaseFlowEngine::cellIsTrapNW,"get status wrt 'trapped non-wetting phase' state")
	.def("getCellSaturation",&TwoPhaseFlowEngine::cellSaturation,"get saturation of one pore")
	.def("setCellSaturation",&TwoPhaseFlowEngine::setCellSaturation,"change saturation of one pore")
	.def("computeOnePhaseFlow",&TwoPhaseFlowEngine::computeOnePhaseFlow,"compute pressure and fluxes in the W-phase")
	.def("initialization",&TwoPhaseFlowEngine::initialization,"Initialize invasion setup. Build network, compute pore geometry info and initialize reservoir boundary conditions. ")
	.def("getPoreThroatRadius",&TwoPhaseFlowEngine::cellporeThroatRadius,"get 4 pore throat radii")
	.def("getNeighbors",&TwoPhaseFlowEngine::getNeighbors,"get 4 neigboring cells")
	.def("getCellHasInterface",&TwoPhaseFlowEngine::cellHasInterface,"indicates whether a NW-W interface is present within the cell")
	.def("getCellInSphereRadius",&TwoPhaseFlowEngine::cellInSphereRadius,"get the radius of the inscribed sphere in a pore unit")
	.def("getCellVoidVolume",&TwoPhaseFlowEngine::cellVoidVolume,"get the volume of pore space in each pore unit")
	.def("getCellMergedVolume",&TwoPhaseFlowEngine::cellMergedVolume,"get the merged volume of pore space in each pore unit")
	.def("setCellHasInterface",&TwoPhaseFlowEngine::setCellHasInterface,"change wheter a cell has a NW-W interface")
	.def("savePoreNetwork",&TwoPhaseFlowEngine::savePoreNetwork,"Extract the pore network of the granular material")
	.def("reTriangulateSpheres",&TwoPhaseFlowEngine::reTriangulate,"apply triangulation, while maintaining saturation")
	.def("actionMergingAlgorithm",&TwoPhaseFlowEngine::actionMergingAlgorithm,"apply triangulation, while maintaining saturation")

	
	.def("getCell2",&TwoPhaseFlowEngine::getCell2,(boost::python::arg("pos")),"get id of the cell containing (X,Y,Z).")		//should be removed finally, duplicate function
	.def("setCellDeltaVolume",&TwoPhaseFlowEngine::setCellDV,(boost::python::arg("id"),boost::python::arg("value")),"get id of the cell containing (X,Y,Z).")

	
	.def("mergeCells",&TwoPhaseFlowEngine::mergeCells,"Extract the pore network of the granular material")
 	.def("calculateResidualSaturation",&TwoPhaseFlowEngine::calculateResidualSaturation,"Calculate the residual saturation for each pore body")
	.def("getCellEntrySaturation",&TwoPhaseFlowEngine::cellEntrySaturation,"get the entry saturation of each pore throat")
	.def("getCellThresholdSaturation",&TwoPhaseFlowEngine::cellThresholdSaturation,"get the saturation of imbibition")
	.def("getCellMergedID",&TwoPhaseFlowEngine::cellMergedID,"get the saturation of imbibition")
	.def("actionTPF",&TwoPhaseFlowEngine::actionTPF,"run 1 time step flow Engine")
	.def("readTriangulation",&TwoPhaseFlowEngine::readTriangulation,"get the solid area of various solids in a pore")
	.def("imposeDeformationFluxTPF",&TwoPhaseFlowEngine::imposeDeformationFluxTPF,"Impose fluxes defined in dvTPF")




	
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TwoPhaseFlowEngine);

#endif //TwoPhaseFLOW
 
