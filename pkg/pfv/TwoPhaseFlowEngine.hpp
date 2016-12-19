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
	int windowsID;//a temp cell info for experiment comparison(used by chao)
	double solidLine [4][4];//the length of intersecting line between sphere and facet. [i][j] is for facet "i" and sphere (facetVertices)"[i][j]". Last component [i][3] for 1/sumLines in the facet "i" (used by chao).
	
	int label;//for marking disconnected clusters. NW-res: 0; W-res: 1; W-clusters by 2,3,4...
	TwoPhaseCellInfo (void)
	{
		isWRes = true; isNWRes = false; isTrapW = false; isTrapNW = false;
		saturation = 1.0;
		hasInterface = false;
		trapCapP = 0;
		poreThroatRadius.resize(4, 0);
		poreBodyRadius = 0;
		poreBodyVolume = 0;
		windowsID = 0;
		for (int k=0; k<4;k++) for (int l=0; l<4;l++) solidLine[k][l]=0;
		label=-1;
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
			
		boost::python::list getInterfaces(unsigned int clusterId){
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
	double totalCellVolume;
	vector<shared_ptr<PhaseCluster> > clusters; // the list of clusters

	//We can overload every functions of the base engine to make it behave differently
	//if we overload action() like this, this engine is doing nothing in a standard timestep, it can still have useful functions
	virtual void action() {};
	
	//If a new function is specific to the derived engine, put it here, else go to the base TemplateFlowEngine if it is useful for everyone

	//compute pore body radius
	void computePoreBodyVolume();
	void computePoreBodyRadius();
	void computeSolidLine();

	//compute entry pore throat radius (drainage)
	void computePoreThroatRadiusMethod1();//MS-P method
	void computePoreThroatRadiusTrickyMethod1();//set the radius of pore throat between side pores negative.
	double computeEffPoreThroatRadius(CellHandle cell, int j);
	double computeEffPoreThroatRadiusFine(CellHandle cell, int j);
	double bisection(CellHandle cell, int j, double a, double b);
	double computeTriRadian(double a, double b, double c);
	double computeDeltaForce(CellHandle cell,int j, double rC);

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
	void updateCellLabel();
	void updateSingleCellLabelRecursion(CellHandle cell, PhaseCluster* cluster);
	void clusterGetFacet(PhaseCluster* cluster, CellHandle cell, int facet);//update cluster inetrfacial area and max entry radius wrt to a facet
	void clusterGetPore(PhaseCluster* cluster, CellHandle cell);//add pore to cluster, updating flags and cluster volume
	boost::python::list pyClusters();
// 	int getMaxCellLabel();

	void invasion2();//without-trap
	void updateReservoirs2();
	///end of invasion model

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
	

		
	//post-processing
	void savePoreNetwork();
	void saveVtk(const char* folder) {bool initT=solver->noCache; solver->noCache=false; solver->saveVtk(folder); solver->noCache=initT;}
	void savePhaseVtk(const char* folder);
		
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

	//TODO
	double computePoreSatAtInterface(int ID);
	void computePoreCapillaryPressure(CellHandle cell);
	
	//FIXME, needs to trigger initSolver() Somewhere, else changing flow.debug or other similar things after first calculation has no effect
	//FIXME, I removed indexing cells from inside UnsatEngine (SoluteEngine shouldl be ok (?)) in order to get pressure computed, problem is they are not indexed at all if flow is not calculated
	void computeOnePhaseFlow() {scene = Omega::instance().getScene().get(); if (!solver) cerr<<"no solver!"<<endl; solver->gaussSeidel(scene->dt);initSolver(*solver);}
	
	CELL_SCALAR_GETTER(bool,.isWRes,cellIsWRes)
	CELL_SCALAR_GETTER(bool,.isNWRes,cellIsNWRes)
	CELL_SCALAR_GETTER(bool,.isTrapW,cellIsTrapW)
	CELL_SCALAR_GETTER(bool,.isTrapNW,cellIsTrapNW)
	CELL_SCALAR_SETTER(bool,.isNWRes,setCellIsNWRes)
	CELL_SCALAR_GETTER(Real,.saturation,cellSaturation)
	CELL_SCALAR_SETTER(Real,.saturation,setCellSaturation)
	CELL_SCALAR_GETTER(bool,.isFictious,cellIsFictious) //Temporary function to allow for simulations in Python
	CELL_SCALAR_GETTER(bool,.hasInterface,cellHasInterface) //Temporary function to allow for simulations in Python
	CELL_SCALAR_GETTER(Real,.poreBodyRadius,cellInSphereRadius) //Temporary function to allow for simulations in Python	
	CELL_SCALAR_GETTER(Real,.poreBodyVolume,cellVoidVolume) //Temporary function to allow for simulations in Python	
	CELL_SCALAR_SETTER(bool,.hasInterface,setCellHasInterface) //Temporary function to allow for simulations in Python
	CELL_SCALAR_GETTER(int,.label,cellLabel)

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(TwoPhaseFlowEngine,TwoPhaseFlowEngineT,"documentation here",
	((double,surfaceTension,0.0728,,"Water Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))
	((bool,recursiveInvasion,true,,"If true the invasion stops only when no entry pc is less than current capillary pressure, implying simultaneous invasion of many pores. Else only one pore invasion per invasion step."))
	((bool,initialWetting,true,,"Initial wetting saturated (=true) or non-wetting saturated (=false)"))
	((bool, isPhaseTrapped,true,,"If True, both phases can be entrapped by the other, which would correspond to snap-off. If false, both phases are always connected to their reservoirs, thus no snap-off."))
	((bool, isInvadeBoundary, true,,"Invasion side boundary condition. If True, pores of side boundary can be invaded; if False, the pore throats connecting side boundary are closed, those pores are excluded in saturation calculation."))	
	((bool, drainageFirst, true,,"If true, activate drainage first (initial saturated), then imbibition; if false, activate imbibition first (initial unsaturated), then drainage."))
	((double,dtDynTPF,0.0,,"Parameter which stores the smallest time step, based on the residence time"))
	((int,entryPressureMethod,1,,"integer to define the method used to determine the pore throat radii and the according entry pressures. 1)radius of entry pore throat based on MS-P method; 2) radius of the inscribed circle; 3) radius of the circle with equivalent surface area of the pore throat."))
	((double,partiallySaturatedPores,false,,"Include partially saturated pores or not?"))
	((bool, isCellLabelActivated, false,, "Activate cell labels for marking disconnected wetting clusters. NW-reservoir label 0; W-reservoir label 1; disconnected W-clusters label from 2. "))
	((bool, computeForceActivated, true,,"Activate capillary force computation. WARNING: turning off means capillary force is not computed at all, but the drainage can still work."))
	((bool, isDrainageActivated, true,, "Activates drainage."))
	((bool, isImbibitionActivated, false,, "Activates imbibition."))

	
	,/*TwoPhaseFlowEngineT()*/,
	clusters.resize(2); clusters[0]=shared_ptr<PhaseCluster>(new PhaseCluster); clusters[1]=shared_ptr<PhaseCluster>(new PhaseCluster);
	,
	.def("getCellIsFictious",&TwoPhaseFlowEngine::cellIsFictious,"Check the connection between pore and boundary. If true, pore throat connects the boundary.")
	.def("setCellIsNWRes",&TwoPhaseFlowEngine::setCellIsNWRes,"set status whether 'wetting reservoir' state")
	.def("savePhaseVtk",&TwoPhaseFlowEngine::savePhaseVtk,(boost::python::arg("folder")="./phaseVtk"),"Save the saturation of local pores in vtk format. Sw(NW-pore)=0, Sw(W-pore)=1. Specify a folder name for output.")
	.def("getCellIsWRes",&TwoPhaseFlowEngine::cellIsWRes,"get status wrt 'wetting reservoir' state")
	.def("getCellIsNWRes",&TwoPhaseFlowEngine::cellIsNWRes,"get status wrt 'non-wetting reservoir' state")
	.def("getCellIsTrapW",&TwoPhaseFlowEngine::cellIsTrapW,"get status wrt 'trapped wetting phase' state")
	.def("getCellIsTrapNW",&TwoPhaseFlowEngine::cellIsTrapNW,"get status wrt 'trapped non-wetting phase' state")
	.def("getCellSaturation",&TwoPhaseFlowEngine::cellSaturation,"get saturation of one pore")
	.def("setCellSaturation",&TwoPhaseFlowEngine::setCellSaturation,"change saturation of one pore")
	.def("computeOnePhaseFlow",&TwoPhaseFlowEngine::computeOnePhaseFlow,"compute pressure and fluxes in the W-phase")
	.def("initialization",&TwoPhaseFlowEngine::initialization,"Initialize invasion setup. Build network, compute pore geometry info and initialize reservoir boundary conditions. ")
	.def("computePoreSatAtInterface",&TwoPhaseFlowEngine::computePoreSatAtInterface,(boost::python::arg("ID")),"compute pressure and fluxes in the W-phase")
	.def("getPoreThroatRadius",&TwoPhaseFlowEngine::cellporeThroatRadius,"get 4 pore throat radii")
	.def("getNeighbors",&TwoPhaseFlowEngine::getNeighbors,"get 4 neigboring cells")
	.def("getCellHasInterface",&TwoPhaseFlowEngine::cellHasInterface,"indicates whether a NW-W interface is present within the cell")
	.def("getCellInSphereRadius",&TwoPhaseFlowEngine::cellInSphereRadius,"get the radius of the inscribed sphere in a pore unit")
	.def("getCellVoidVolume",&TwoPhaseFlowEngine::cellVoidVolume,"get the volume of pore space in each pore unit")
	.def("setCellHasInterface",&TwoPhaseFlowEngine::setCellHasInterface,"change wheter a cell has a NW-W interface")
	.def("savePoreNetwork",&TwoPhaseFlowEngine::savePoreNetwork,"Extract the pore network of the granular material")
	.def("getCellLabel",&TwoPhaseFlowEngine::cellLabel,"get cell label. 0 for NW-reservoir; 1 for W-reservoir; others for disconnected W-clusters.")
	.def("getMinDrainagePc",&TwoPhaseFlowEngine::getMinDrainagePc,"Get the minimum entry capillary pressure for the next drainage step.")
	.def("getMaxImbibitionPc",&TwoPhaseFlowEngine::getMaxImbibitionPc,"Get the maximum entry capillary pressure for the next imbibition step.")
	.def("getSaturation",&TwoPhaseFlowEngine::getSaturation,(boost::python::arg("isSideBoundaryIncluded")),"Get saturation of entire packing. If isSideBoundaryIncluded=false (default), the pores of side boundary are excluded in saturation calculating; if isSideBoundaryIncluded=true (only in isInvadeBoundary=true drainage mode), the pores of side boundary are included in saturation calculating.")
	.def("invasion",&TwoPhaseFlowEngine::invasion,"Run the drainage invasion.")
	.def("computeCapillaryForce",&TwoPhaseFlowEngine::computeCapillaryForce,"Compute capillary force. ")
	.def("saveVtk",&TwoPhaseFlowEngine::saveVtk,(boost::python::arg("folder")="./VTK"),"Save pressure field in vtk format. Specify a folder name for output.")
	.def("getPotentialPendularSpheresPair",&TwoPhaseFlowEngine::getPotentialPendularSpheresPair,"Get the list of sphere ID pairs of potential pendular liquid bridge.")
	.def("getClusters",&TwoPhaseFlowEngine::pyClusters/*,(boost::python::arg("folder")="./VTK")*/,"Get the list of clusters.")
	
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TwoPhaseFlowEngine);

#endif //TwoPhaseFLOW
 
