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
// #define TWOPHASEFLOW
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
	double poreBodyRadius;
	double poreBodyVolume;
	double porosity;
	int windowsID;//a temp cell info for experiment comparison(used by chao)
	double solidLine [4][4];//the length of intersecting line between sphere and facet. [i][j] is for facet "i" and sphere (facetVertices)"[i][j]". Last component [i][3] for 1/sumLines in the facet "i" (used by chao).
	
	//DynamicTwoPhaseFlow 
	std::vector<double> entryPressure;
	std::vector<double> entrySaturation;
	std::vector<int> poreNeighbors;
	std::vector<int> poreIdConnectivity;
	std::vector<double> listOfkNorm;
	std::vector<double> listOfkNorm2;
	std::vector<double> listOfEntrySaturation;
	std::vector<double> listOfEntryPressure;
	std::vector<double> kNorm2;
	std::vector<double> listOfThroatArea;
	std::vector<double> particleSurfaceArea;	//Surface area of four particles enclosing one grain-based tetrahedra
	double accumulativeDVSwelling;
	double saturation2;
	int numberFacets;
	int isFictiousId;
	double mergedVolume;
	int mergednr;
	unsigned int mergedID;
	double thresholdSaturation;
	double flux;
	double accumulativeDV; 	
	double airWaterArea;
	bool isWResInternal;
      	double conductivityWRes;
	double minSaturation;
	int poreId;
	bool airBC;
	bool waterBC;
	double thresholdPressure;
	double apparentSolidVolume;
	double dvSwelling;
	double dvTPF;
	bool isNWResDef;
	int invadedFrom;
	int label;//for marking disconnected clusters. NW-res: 0; W-res: 1; W-clusters by 2,3,4...

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
		airBC = false;
		waterBC = false;
		numberFacets = 4;
		mergedVolume = 0;
		mergednr = 0;
		mergedID = 0;
		apparentSolidVolume = 0.0;
		dvSwelling = 0.0;
		entryPressure.resize(4,0);
		entrySaturation.resize(4,0);
		poreIdConnectivity.resize(4,-1);
		particleSurfaceArea.resize(4,0);
		thresholdSaturation = 0.0;
		flux = 0.0;			//NOTE can potentially be removed, currently not used but might be handy in future work
		accumulativeDV = 0.0;
		thresholdPressure = 0.0;
		airWaterArea = 0.0;
		accumulativeDV = 0.0;
		minSaturation = 0.0;
		poreId = -1;
		isWResInternal = false;
		dvTPF = 0.0;			//FIXME dvTPF is currently only used to impose pressure as dv() cannot be imposed from FlowEngine currently
		isNWResDef = false;
		conductivityWRes = 0.0;
		invadedFrom = 0;
		label=-1;
		porosity=0.;
	}
	
};

class TwoPhaseVertexInfo : public FlowVertexInfo_TwoPhaseFlowEngineT {
	public:
	//same here if needed
};

typedef TemplateFlowEngine_TwoPhaseFlowEngineT<TwoPhaseCellInfo,TwoPhaseVertexInfo> TwoPhaseFlowEngineT;
REGISTER_SERIALIZABLE(TwoPhaseFlowEngineT);

// A class to represent isolated single-phase cluster (main application in convective drying at the moment)
class PhaseCluster : public Serializable
{
		double totalCellVolume;
// 		CellHandle entryPoreHandle;
	public :
		virtual ~PhaseCluster();
		vector<TwoPhaseFlowEngineT::CellHandle> pores;
		vector<std::pair<std::pair<unsigned int,unsigned int>,double> > interfaces;
		TwoPhaseFlowEngineT::RTriangulation* tri;
		void reset() {label=entryPore=-1;volume=entryRadius=interfacialArea=0; pores.clear(); interfaces.clear();}
		
		vector<int> getPores() { vector<int> res;
			for (vector<TwoPhaseFlowEngineT::CellHandle>::iterator it =  pores.begin(); it!=pores.end(); it++) res.push_back((*it)->info().id);
			return res;}
			
		boost::python::list getInterfaces(){
			boost::python::list ints;
			for (vector<std::pair<std::pair<unsigned int,unsigned int>,double> >::iterator it =  interfaces.begin(); it!=interfaces.end(); it++)
				ints.append(boost::python::make_tuple(it->first.first,it->first.second,it->second));
			return ints;
		}

		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(PhaseCluster,Serializable,"Preliminary.",
		((int,label,-1,,"Unique label of this cluster, should be reflected in pores of this cluster."))
		((double,volume,0,,"cumulated volume of all pores."))
		((double,entryRadius,0,,"smallest entry capillary pressure."))
		((int,entryPore,-1,,"the pore of the cluster incident to the throat with smallest entry Pc."))
		((double,interfacialArea,0,,"interfacial area of the cluster"))
		,,,
		.def("getPores",&PhaseCluster::getPores,"get the list of pores by index")
		.def("getInterfaces",&PhaseCluster::getInterfaces,"get the list of interfacial pore-throats associated to a cluster, listed as [id1,id2,area] where id2 is the neighbor pore outside the cluster.")
		)
};
REGISTER_SERIALIZABLE(PhaseCluster);

class TwoPhaseFlowEngine : public TwoPhaseFlowEngineT
{
	public :
	double airBoundaryPressure = 0.0;
	std::vector<CellHandle> listOfPores;
	bool imposeDeformationFluxTPFSwitch =false;
	double totalCellVolume;
	vector<shared_ptr<PhaseCluster> > clusters; // the list of clusters

	//We can overload every functions of the base engine to make it behave differently
	//if we overload action() like this, this engine is doing nothing in a standard timestep, it can still have useful functions
	virtual void action() {};

	//If a new function is specific to the derived engine, put it here, else go to the base TemplateFlowEngine if it is useful for everyone
	void computePoreBodyVolume();
	void computePoreBodyRadius();
	void computeSolidLine();
	void savePhaseVtk(const char* folder);

	//compute entry pore throat radius (drainage)
	void computePoreThroatRadiusMethod1();//MS-P method
	void computePoreThroatRadiusTrickyMethod1();//set the radius of pore throat between side pores negative.
	double computeEffPoreThroatRadius(CellHandle cell, int j);
	double computeEffPoreThroatRadiusFine(CellHandle cell, int j);
	double computeMSPRcByPosRadius(const Vector3r& posA, const double& rA, const Vector3r& posB, const double& rB, const Vector3r& posC, const double& rC);
	double computeTriRadian(double a, double b, double c);
	double computeEffRcByPosRadius(const Vector3r& posA, const double& rA, const Vector3r& posB, const double& rB, const Vector3r& posC, const double& rC){double reff=solver->computeEffectiveRadiusByPosRadius(makeCgPoint(posA),rA,makeCgPoint(posB),rB,makeCgPoint(posC),rC); return reff<0?1.0e-10:reff;};
	double bisection(const Vector3r& posA, const double& rA, const Vector3r& posB, const double& rB, const Vector3r& posC, const double& rC, double a, double b);
	double computeDeltaForce(const Vector3r& posA, const double& rA, const Vector3r& posB, const double& rB, const Vector3r& posC, const double& rC, double r);

	void computePoreThroatRadiusMethod2();//radius of the inscribed circle
	void computePoreThroatRadiusMethod3();//radius of area-equivalent circle
	
	///begin of invasion (mainly drainage) model
	void initialization();
	void initializeReservoirs();

	void invasion();//functions can be shared by two modes
	void invasionSingleCell(CellHandle cell);
	void updatePressure();
	double getMinDrainagePc();
	double getMaxImbibitionPc();
	double getSaturation(bool isSideBoundaryIncluded=false);

	void invasion1();//with-trap
	void updateReservoirs1();
	void WResRecursion(CellHandle cell);
	void NWResRecursion(CellHandle cell);
	void checkTrap(double pressure);
	void updateReservoirLabel();
	void invasion2();//without-trap
	void updateReservoirs2();
	///end of invasion model
	
	//## Clusters ##
	void updateCellLabel();
	void updateSingleCellLabelRecursion(CellHandle cell, PhaseCluster* cluster);
	void clusterGetFacet(PhaseCluster* cluster, CellHandle cell, int facet);//update cluster inetrfacial area and max entry radius wrt to a facet
	void clusterGetPore(PhaseCluster* cluster, CellHandle cell);//add pore to cluster, updating flags and cluster volume
	vector<int> clusterInvadePore(PhaseCluster* cluster, CellHandle cell);//remove pore from cluster, if it splits the cluster in many pieces introduce new one(s)
	vector<int> pyClusterInvadePore(int cellId) {
		int label = solver->T[solver->currentTes].cellHandles[cellId]->info().label;
		if (label<=1) {LOG_WARN("the pore is not in a cluster, label="<<label); return vector<int>();}
		return clusterInvadePore(clusters[label].get(), solver->T[solver->currentTes].cellHandles[cellId]);}
	boost::python::list pyClusters();
// 	int getMaxCellLabel();

	//compute forces
	void computeFacetPoreForcesWithCache(bool onlyCache=false);	
	void computeCapillaryForce() {computeFacetPoreForcesWithCache(false);}
	
	//combine with pendular model
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
	  return bridgeIds;
	}
	bool detectBridge(RTriangulation::Finite_edges_iterator& edge);
	
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
	double getSolidVolumeInCell(CellHandle cell);
	
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
	void copyPoreDataToCells();
        void verifyCompatibilityBC();
	void makeListOfPoresInCells(bool fast);

        
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
	std::vector<double> listOfMergedVolume;

	
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
	
      boost::python::list solidSurfaceAreaPerParticle(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
	  boost::python::list ids;
	  if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
	  for (unsigned int i=0;i<4;i++) ids.append(solver->T[solver->currentTes].cellHandles[id]->info().particleSurfaceArea[i]);
	return ids;
	}

	//post-processing
	void savePoreNetwork(const char* folder);
	void saveVtk(const char* folder) {bool initT=solver->noCache; solver->noCache=false; solver->saveVtk(folder); solver->noCache=initT;}
		
	boost::python::list cellporeThroatRadius(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
	  boost::python::list ids;
	  if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
	  for (unsigned int i=0;i<4;i++) ids.append(solver->T[solver->currentTes].cellHandles[id]->info().poreThroatRadius[i]);
	return ids;
	}

	boost::python::list getNeighbors(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
		boost::python::list ids;
		if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
		for (unsigned int i=0;i<4;i++) {
			const CellHandle& neighbourCell = solver->T[solver->currentTes].cellHandles[id]->neighbor(i);
			if (!solver->T[currentTes].Triangulation().is_infinite(neighbourCell)) ids.append(neighbourCell->info().id);}
		return ids;}

	//TODO
	//Dynamic code
	boost::python::list cellEntrySaturation(unsigned int id){ // Temporary function to allow for simulations in Python, can be easily accessed in c++
	  boost::python::list ids;
	  if (id>=solver->T[solver->currentTes].cellHandles.size()) {LOG_ERROR("id out of range, max value is "<<solver->T[solver->currentTes].cellHandles.size()); return ids;}
	  for (unsigned int i=0;i<4;i++) ids.append(solver->T[solver->currentTes].cellHandles[id]->info().entrySaturation[i]);
	return ids;
	}


	
	
	//FIXME, needs to trigger initSolver() Somewhere, else changing flow.debug or other similar things after first calculation has no effect
	//FIXME, I removed indexing cells from inside UnsatEngine (SoluteEngine shouldl be ok (?)) in order to get pressure computed, problem is they are not indexed at all if flow is not calculated

	void computeOnePhaseFlow() {scene = Omega::instance().getScene().get(); if (!solver) cerr<<"no solver!"<<endl; solver->gaussSeidel(scene->dt);initSolver(*solver);}
	///manipulate/get/set on pore geometry
	bool isCellNeighbor(unsigned int cell1, unsigned int cell2);
	void setPoreThroatRadius(unsigned int cell1, unsigned int cell2, double radius);
	double getPoreThroatRadius(unsigned int cell1, unsigned int cell2);

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
	CELL_SCALAR_SETTER(Real,.poreBodyRadius,setPoreBodyRadius) //Temporary function to allow for simulations in Python, for lbm coupling.	
	CELL_SCALAR_GETTER(Real,.poreBodyVolume,cellVoidVolume) //Temporary function to allow for simulations in Python	

	CELL_SCALAR_GETTER(Real,.mergedVolume,cellMergedVolume) //Temporary function to allow for simulations in Python
	CELL_SCALAR_SETTER(Real,.dvTPF,setCellDV) //Temporary function to allow for simulations in Python
	CELL_SCALAR_GETTER(Real,.porosity,cellPorosity)
	CELL_SCALAR_SETTER(bool,.hasInterface,setCellHasInterface) //Temporary function to allow for simulations in Python
	CELL_SCALAR_GETTER(int,.label,cellLabel)
	CELL_SCALAR_GETTER(Real,.volume(),cellVolume) //Temporary function to allow for simulations in Python	


	//Dynamic Code
	CELL_SCALAR_GETTER(Real,.thresholdSaturation,cellThresholdSaturation) //Temporary function to allow for simulations in Python	
	CELL_SCALAR_GETTER(Real,.mergedID,cellMergedID) //Temporary function to allow for simulations in Python	

	
	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(TwoPhaseFlowEngine,TwoPhaseFlowEngineT,"documentation here",
	((double,surfaceTension,0.0728,,"Water Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))
	((bool,recursiveInvasion,true,,"If true the invasion stops only when no entry pc is less than current capillary pressure, implying simultaneous invasion of many pores. Else only one pore invasion per invasion step."))
	((bool,initialWetting,true,,"Initial wetting saturated (=true) or non-wetting saturated (=false)"))
	((bool, isPhaseTrapped,true,,"If True, both phases can be entrapped by the other, which would correspond to snap-off. If false, both phases are always connected to their reservoirs, thus no snap-off."))
	((bool, isInvadeBoundary, true,,"Invasion side boundary condition. If True, pores of side boundary can be invaded; if False, the pore throats connecting side boundary are closed, those pores are excluded in saturation calculation."))	
	((bool, drainageFirst, true,,"If true, activate drainage first (initial saturated), then imbibition; if false, activate imbibition first (initial unsaturated), then drainage."))
	((double,dtDynTPF,0.0,,"Parameter which stores the smallest time step, based on the residence time"))
	((int,entryPressureMethod,1,,"integer to define the method used to determine the pore throat radii and the according entry pressures. 1)radius of entry pore throat based on MS-P method; 2) radius of the inscribed circle; 3) radius of the circle with equivalent surface area of the pore throat."))
// 	((double,partiallySaturatedPores,false,,"Include partially saturated pores or not?"))

	//BEGIN Latest dynamic/pore merging things (to clean maybe)
	((double,entryMethodCorrection,float(entryPressureMethod),,"Parameter that is used in computing entry pressure of a pore throat: P_ij =  entryMethodCorrection * surfaceTension / radius_porethroat "))
	//Dynamic TwoPhaseFlow
	((vector<bool>, bndCondIsWaterReservoir, vector<bool>(6,false),,"Boundary conditions, if bndCondIsPressure[] = True, is it air or water boundary condition? True is water reservoir"))
	((unsigned int,maxIDMergedCells,0,,"maximum number of merged ID, this is computed in mergeCells()"))
	((double,waterPressurePartiallySatPores,0.0,,"water pressure based on the volume-averaged water pressure in partially-saturated pore units (i.e. pore units having an interface)"))
	((double,waterPressure,0.0,,"Volume-averaged water pressure"))
	((double,waterSaturation,0.0,,"Water saturation, excluding the boundary cells"))
	((double,voidVolume,0.0,,"total void volume, excluding boundary cells"))
  	((bool,stopSimulation, false,,"Boolean to indicate that dynamic flow simulations cannot find a solution (or next time step). If True, stop simulations"))
  	((bool,debugTPF, false,,"Print debuging messages two phase flow engine "))
	((double,airWaterInterfacialArea,0.0,,"Air-water interfacial area, based on the pore-unit assembly and regular-shaped pore units"))
	((double,areaAveragedPressure,0.0,,"Air-water interfacial area averaged water pressure "))
	((double,maximumRatioPoreThroatoverPoreBody,0.90,,"maximum ratio of pore throat radius over pore body radius, this is used during merging of tetrahedra."))
	((double,totalWaterVolume,0.0,,"total watervolume"))
	((string,modelRunName,"dynamicDrainage",,"Name of simulation, to be implemented into output files"))
	((double,safetyFactorTimeStep,1.0,,"Safey coefficient for time step"))
	((double,fluxInViaWBC,0.0,,"Total water flux over water boundary conditions"))
	((double, accumulativeFlux,0.0,,"accumulative influx of water"))
	((double, truncationPrecision,1e-6,,"threshold at which a saturation is truncated"))
	((unsigned int, numberOfPores, 0,,"Number of pores (i.e. number of tetrahedra, but compensated for merged tetrahedra"))
	((bool, firstDynTPF, true,,"this bool activated the initialization of the dynamic flow engine, such as merging and defining initial values"))
	((bool, keepTriangulation, false,,"this bool activated triangulation or not during initialization"))
 	((bool, remesh, false,,"update triangulation? -- YET TO BE IMPLEMENTED"))                         //FIXME - trinagulation of unsaturated pore units still to be implemented properly
	((bool, deformation, false,,"Boolean to indicate whether simulations of dynamic flow are withing a deformating packing or not. If true, change of void volume due to deformation is considered in flow computations."))
	((int, iterationTPF, -1,,"Iteration number"))
	((double, initialPC, 2000.0,,"Initial capillary pressure of the water-air inside the packing"))
	((double, accumulativeDeformationFlux, 0.0,,"accumulative internal flux caused by deformation"))
	((bool, solvePressureSwitch, true,,"solve for pressure during actionTPF()"))
	((double, deltaTimeTruncation, 0.0,,"truncation of time step, to avoid very small time steps during local imbibition, NOTE it does affect the mass conservation not set to 0"))
	((double, waterBoundaryPressure, 0.0,,"Water pressure at boundary used in computations, is set automaticaly, but this value can be used to change water pressure during simulations"))
	((double, waterVolumeTruncatedLost, 0.0,,"Water volume that has been truncated."))
	((bool, getQuantitiesUpdateCont, false,,"Continuous update of various macro-scale quantities or not. Note that the updating quantities is computationally expensive"))
	((double, simpleWaterPressure, 0.0,,"Water pressure based on averaging over pore volume"))
	((double, centroidAverageWaterPressure, 0.0,,"Water pressure based on centroid-corrected averaging, see Korteland et al. (2010) - what is the correct definition of average pressure?"))
	((double, fractionMinSaturationInvasion, -1.0,,"Set the threshold saturation at which drainage can occur (Sthr = fractionMinSaturationInvasion), note that -1 implied the conventional definition of Sthr"))
	((vector<double>, setFractionParticles, vector<double>(scene->bodies->size(),0.0),,"Correction fraction for swelling of particles by mismatch of surface area of particles with those from actual surface area in pore units"))
       	((bool,primaryTPF, true,,"Boolean to indicate whether the initial conditions are for primary drainage of imbibition (dictated by drainageFirst) or secondary drainage or imbibition. Note that during simulations, a switch from drainage to imbibition or vise versa can easily be made by changing waterBoundaryPressure"))
       	((bool,swelling, false,,"If true, include swelling of particles during TPF computations"))
	
	
	//END Latest dynamic/pore merging

	((bool, isCellLabelActivated, false,, "Activate cell labels for marking disconnected wetting clusters. NW-reservoir label 0; W-reservoir label 1; disconnected W-clusters label from 2. "))
	((bool, computeForceActivated, true,,"Activate capillary force computation. WARNING: turning off means capillary force is not computed at all, but the drainage can still work."))
	((bool, isDrainageActivated, true,, "Activates drainage."))
	((bool, isImbibitionActivated, false,, "Activates imbibition."))

	
	,/*TwoPhaseFlowEngineT()*/,
	clusters.resize(2); clusters[0]=shared_ptr<PhaseCluster>(new PhaseCluster); clusters[1]=shared_ptr<PhaseCluster>(new PhaseCluster);
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
	.def("getPoreThroatRadiusList",&TwoPhaseFlowEngine::cellporeThroatRadius,(boost::python::arg("cell_ID")),"get 4 pore throat radii of a cell.")
	.def("getNeighbors",&TwoPhaseFlowEngine::getNeighbors,"get 4 neigboring cells")
	.def("getCellHasInterface",&TwoPhaseFlowEngine::cellHasInterface,"indicates whether a NW-W interface is present within the cell")
	.def("getCellInSphereRadius",&TwoPhaseFlowEngine::cellInSphereRadius,"get the radius of the inscribed sphere in a pore unit")
	.def("setPoreBodyRadius",&TwoPhaseFlowEngine::setPoreBodyRadius,"set the entry pore body radius.")
	.def("getCellVoidVolume",&TwoPhaseFlowEngine::cellVoidVolume,"get the volume of pore space in each pore unit")
	//Pore merging
	.def("getCellMergedVolume",&TwoPhaseFlowEngine::cellMergedVolume,"get the merged volume of pore space in each pore unit")
	.def("setCellHasInterface",&TwoPhaseFlowEngine::setCellHasInterface,"change wheter a cell has a NW-W interface")
	.def("savePoreNetwork",&TwoPhaseFlowEngine::savePoreNetwork,(boost::python::arg("folder")="./poreNetwork"),"Extract the pore network of the granular material (i.e. based on triangulation of the pore space")
	.def("reTriangulateSpheres",&TwoPhaseFlowEngine::reTriangulate,"apply triangulation, while maintaining saturation")
	.def("actionMergingAlgorithm",&TwoPhaseFlowEngine::actionMergingAlgorithm,"apply triangulation, while maintaining saturation")
	.def("getCell2",&TwoPhaseFlowEngine::getCell2,(boost::python::arg("pos")),"get id of the cell containing (X,Y,Z).")		//should be removed finally, duplicate function
	.def("setCellDeltaVolume",&TwoPhaseFlowEngine::setCellDV,(boost::python::arg("id"),boost::python::arg("value")),"get id of the cell containing (X,Y,Z).")
	.def("mergeCells",&TwoPhaseFlowEngine::mergeCells,"Extract the pore network of the granular material")
	//Dynamic flow
 	.def("calculateResidualSaturation",&TwoPhaseFlowEngine::calculateResidualSaturation,"Calculate the residual saturation for each pore body")
	.def("copyPoreDataToCells",&TwoPhaseFlowEngine::copyPoreDataToCells,"copy data from merged pore units back to grain-based tetrahedra, this should be done before exporting VTK files")
	.def("getCellEntrySaturation",&TwoPhaseFlowEngine::cellEntrySaturation,"get the entry saturation of each pore throat")
	.def("getCellThresholdSaturation",&TwoPhaseFlowEngine::cellThresholdSaturation,"get the saturation of imbibition")
	.def("getCellMergedID",&TwoPhaseFlowEngine::cellMergedID,"get the saturation of imbibition")
	.def("actionTPF",&TwoPhaseFlowEngine::actionTPF,"run 1 time step flow Engine")
	.def("getSolidSurfaceAreaPerParticle",&TwoPhaseFlowEngine::solidSurfaceAreaPerParticle,(boost::python::arg("cell_ID")),"get solid area inside a packing of particles")
// 	.def("readTriangulation",&TwoPhaseFlowEngine::readTriangulation,"get the solid area of various solids in a pore")
	.def("imposeDeformationFluxTPF",&TwoPhaseFlowEngine::imposeDeformationFluxTPF,"Impose fluxes defined in dvTPF")
	.def("getCellPorosity",&TwoPhaseFlowEngine::cellPorosity,"get the porosity of individual cells.")
	.def("setCellHasInterface",&TwoPhaseFlowEngine::setCellHasInterface,"change wheter a cell has a NW-W interface")
	.def("getCellLabel",&TwoPhaseFlowEngine::cellLabel,"get cell label. 0 for NW-reservoir; 1 for W-reservoir; others for disconnected W-clusters.")
	.def("getMinDrainagePc",&TwoPhaseFlowEngine::getMinDrainagePc,"Get the minimum entry capillary pressure for the next drainage step.")
	.def("getMaxImbibitionPc",&TwoPhaseFlowEngine::getMaxImbibitionPc,"Get the maximum entry capillary pressure for the next imbibition step.")
	.def("getSaturation",&TwoPhaseFlowEngine::getSaturation,(boost::python::arg("isSideBoundaryIncluded")),"Get saturation of entire packing. If isSideBoundaryIncluded=false (default), the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
	.def("invasion",&TwoPhaseFlowEngine::invasion,"Run the drainage invasion.")
	.def("computeCapillaryForce",&TwoPhaseFlowEngine::computeCapillaryForce,"Compute capillary force. ")
	.def("saveVtk",&TwoPhaseFlowEngine::saveVtk,(boost::python::arg("folder")="./VTK"),"Save pressure field in vtk format. Specify a folder name for output.")
	.def("getPotentialPendularSpheresPair",&TwoPhaseFlowEngine::getPotentialPendularSpheresPair,"Get the list of sphere ID pairs of potential pendular liquid bridge.")
	// Clusters
	.def("getClusters",&TwoPhaseFlowEngine::pyClusters/*,(boost::python::arg("folder")="./VTK")*/,"Get the list of clusters.")
	.def("clusterInvadePore",&TwoPhaseFlowEngine::pyClusterInvadePore,boost::python::arg("cellId"),"drain the pore identified by cellId and update the clusters accordingly.")
	// others
	.def("getCellVolume",&TwoPhaseFlowEngine::cellVolume,"get the volume of each cell")
	.def("isCellNeighbor",&TwoPhaseFlowEngine::isCellNeighbor,(boost::python::arg("cell1_ID"), boost::python::arg("cell2_ID")),"check if cell1 and cell2 are neigbors.")
	.def("setPoreThroatRadius",&TwoPhaseFlowEngine::setPoreThroatRadius, (boost::python::arg("cell1_ID"), boost::python::arg("cell2_ID"), boost::python::arg("radius")), "set the pore throat radius between cell1 and cell2.")
	.def("getPoreThroatRadius",&TwoPhaseFlowEngine::getPoreThroatRadius, (boost::python::arg("cell1_ID"), boost::python::arg("cell2_ID")), "get the pore throat radius between cell1 and cell2.")
	.def("getEffRcByPosRadius",&TwoPhaseFlowEngine::computeEffRcByPosRadius, (boost::python::arg("position1"),boost::python::arg("radius1"),boost::python::arg("position2"),boost::python::arg("radius2"),boost::python::arg("position3"),boost::python::arg("radius3")), "get effective radius by three spheres position and radius.(inscribed sphere)")
	.def("getMSPRcByPosRadius",&TwoPhaseFlowEngine::computeMSPRcByPosRadius, (boost::python::arg("position1"),boost::python::arg("radius1"),boost::python::arg("position2"),boost::python::arg("radius2"),boost::python::arg("position3"),boost::python::arg("radius3")), "get entry radius wrt MSP method by three spheres position and radius.")
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TwoPhaseFlowEngine);

#endif //TwoPhaseFLOW
 
