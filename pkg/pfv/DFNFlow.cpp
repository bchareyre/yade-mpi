 
/*************************************************************************
*  Copyright (C) 2014 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE

#include<pkg/dem/JointedCohesiveFrictionalPM.hpp>
// #include<pkg/dem/ScGeom.hpp>

//keep this #ifdef for commited versions unless you really have stable version that should be compiled by default
//it will save compilation time for everyone else
//when you want it compiled, you can pass -DDFNFLOW to cmake, or just uncomment the following line

//#define DFNFLOW

#ifdef DFNFLOW
#include "FlowEngine_DFNFlowEngineT.hpp"

class DFNCellInfo : public FlowCellInfo_DFNFlowEngineT
{
	public:
	bool crack;
	Real crackArea;
	DFNCellInfo() : crack(false)  {} /// enable to visualize cracked cells in Paraview
// 	Real anotherVariable;
// 	void anotherFunction() {};
	
// 	/// ROBERT
// 	std::vector<int> faceBreakCount;
// 	std::vector<int> facetAperture;
// 	DFNCellInfo(void)
// 	{	
// 		faceBreakCount.resize(4,0);
// 		facetAperture.resize(4,0);
// 	}
// 	inline std::vector<int>& count(void) {return faceBreakCount;}
// 	inline std::vector<int>& aperture(void) {return facetAperture;}

};

class DFNVertexInfo : public FlowVertexInfo_DFNFlowEngineT {
	public:
	//same here if needed
};

typedef CGT::_Tesselation<CGT::TriangulationTypes<DFNVertexInfo,DFNCellInfo> > DFNTesselation;
#ifdef LINSOLV
class DFNBoundingSphere : public CGT::FlowBoundingSphereLinSolv<DFNTesselation>
#else
class DFNBoundingSphere : public CGT::FlowBoundingSphere<DFNTesselation>
#endif
{
public:
  void saveVtk(const char* folder)
  {
	RTriangulation& Tri = T[noCache?(!currentTes):currentTes].Triangulation();
        static unsigned int number=0;
        char filename[250];
	mkdir(folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        sprintf(filename,"%s/out_%d.vtk",folder,number++);
	int firstReal=-1;

	/// count fictious vertices and cells
	vtkInfiniteVertices = vtkInfiniteCells = 0;
 	FiniteCellsIterator cellEnd = Tri.finite_cells_end();
        for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (!isDrawable) vtkInfiniteCells+=1;
	}
	for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
                if (!v->info().isReal()) vtkInfiniteVertices+=1;
                else if (firstReal==-1) firstReal = vtkInfiniteVertices;
	}

        basicVTKwritter vtkfile((unsigned int) Tri.number_of_vertices()-vtkInfiniteVertices, (unsigned int) Tri.number_of_finite_cells()-vtkInfiniteCells);

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
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal() && cell->vertex(3)->info().isReal();
        	if (isDrawable){vtkfile.write_cell(cell->vertex(0)->info().id()-firstReal, cell->vertex(1)->info().id()-firstReal, cell->vertex(2)->info().id()-firstReal, cell->vertex(3)->info().id()-firstReal);}
        }
        vtkfile.end_cells();

	if (permeabilityMap){
	vtkfile.begin_data("Permeability",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal() && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().s);}
	}
	vtkfile.end_data();}
	else{
	vtkfile.begin_data("Pressure",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal() && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().p());}
	}
	vtkfile.end_data();}

	if (1){
	averageRelativeCellVelocity();
	vtkfile.begin_data("Velocity",CELL_DATA,VECTORS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal() && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().averageVelocity()[0], cell->info().averageVelocity()[1], cell->info().averageVelocity()[2]);}
	}
	vtkfile.end_data();}
	
	if(1){
	vtkfile.begin_data("fracturedCells",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal() && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().crack);}
	}
	vtkfile.end_data();}
  }
};

typedef TemplateFlowEngine_DFNFlowEngineT<DFNCellInfo,DFNVertexInfo, DFNTesselation,DFNBoundingSphere> DFNFlowEngineT;
REGISTER_SERIALIZABLE(DFNFlowEngineT);
YADE_PLUGIN((DFNFlowEngineT));
class DFNFlowEngine : public DFNFlowEngineT
{
	public :
	void trickPermeability(Solver* flow);
	void interpolateCrack(Tesselation& Tes,Tesselation& NewTes);
	void trickPermeability(RTriangulation::Facet_circulator& facet,Real aperture,RTriangulation::Finite_edges_iterator& edge);
	void trickPermeability(RTriangulation::Finite_edges_iterator& edge,Real aperture);
	void setPositionsBuffer(bool current);
	Real leakOffRate;
    	Real averageAperture;
	Real averageFracturePermeability;
    	Real maxAperture;	
	Real crackArea;
	bool edgeOnJoint;
	Real getCrackArea() {return crackArea;}
	Real getLeakOffRate() {return leakOffRate;}
    	Real getAverageAperture() {return averageAperture;}
    	Real getMaxAperture() {return maxAperture;}
// 	void computeTotalFractureArea(Real totalFractureArea,bool printFractureTotalArea); /// Trying to get fracture's surface
	Real totalFractureArea; /// Trying to get fracture's surface
//	CELL_SCALAR_GETTER(double,.crackArea,crackArea)

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DFNFlowEngine,DFNFlowEngineT,"This is an enhancement of the FlowEngine for intact and fractured rocks that takes into acount pre-existing discontinuities and bond breakage between particles. The local conductivity around the broken link is calculated according to parallel plates model",
	((Real,jointsResidualAperture,1.e-6,,"residual aperture of joints"))
// 	/// ROBERT
// 	((Real,facetEdgeBreakThreshold,1,,"minimum number of edges that need to be broken for a facet to be tricked"))
	((Real,slotInitialAperture,1.e-6,,"initial aperture of injection slots"))
	((Real,inducedCracksResidualAperture,0.,,"residual aperture of induced cracks"))
	((Real,apertureFactor,1.,,"calibration parameter for fracture conductance to deal with tortuosity"))
	((bool,updatePositions,false,,"update particles positions when rebuilding the mesh (experimental)"))
 	((bool,printFractureTotalArea,false,,"The final fracture area computed through the network")) /// Trying to get fracture's surface
	((bool,calcCrackArea,true,,"The amount of crack per pore is updated if calcCrackArea=True")) /// Trying to get fracture's surface
	,,,
	.def("getCrackArea",&DFNFlowEngine::getCrackArea,(boost::python::arg("id")),"get the cracked area within cell 'id'.")
	.def("getLeakOffRate",&DFNFlowEngine::getLeakOffRate,"report leak-off rate")
    	.def("getAverageAperture",&DFNFlowEngine::getAverageAperture,"report the current average aperture")
    	.def("getMaxAperture",&DFNFlowEngine::getMaxAperture,"report the max aperture")
// 	.def("computeTotalFractureArea",&DFNFlowEngineT::computeTotalFractureArea,"Compute and print the total fracture area of the network") /// Trying to get fracture's surface
// 	.def("trickPermeability",&DFNFlowEngineT::trickPermeability,"measure the mean trickPermeability in the period")
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(DFNFlowEngine);
YADE_PLUGIN((DFNFlowEngine));

/// In this version, we never update positions when !updatePositions, i.e. keep triangulating the same positions
void DFNFlowEngine::setPositionsBuffer(bool current)
{
	vector<posData>& buffer = current? positionBufferCurrent : positionBufferParallel;
	if ( !updatePositions && buffer.size()>0 ) return;
	buffer.clear();
	buffer.resize(scene->bodies->size());
	shared_ptr<Sphere> sph(new Sphere);
        const int Sph_Index = sph->getClassIndexStatic();
	FOREACH ( const shared_ptr<Body>& b, *scene->bodies ) {
                if ( !b || ignoredBody==b->getId() ) continue;
                posData& dat = buffer[b->getId()];
		dat.id = b->getId();
		dat.pos = b->state->pos;
		dat.isSphere = (b->shape->getClassIndex()==Sph_Index);
		if (dat.isSphere) dat.radius = YADE_CAST<Sphere*>(b->shape.get())->radius;
		dat.exists=true;
	}
}

/// function allows us to interpolate information about fractured/non fractured cells so we can identify newly fractured cells, monitor half width, and identify fracture tip. We also use the loop to compute leakoff rate
void DFNFlowEngine::interpolateCrack(Tesselation& Tes,Tesselation& NewTes){
        RTriangulation& Tri = Tes.Triangulation();
	RTriangulation& newTri = NewTes.Triangulation();
	FiniteCellsIterator cellEnd = newTri.finite_cells_end();
	#ifdef YADE_OPENMP
    	const long size = NewTes.cellHandles.size();
	#pragma omp parallel for num_threads(ompThreads>0 ? ompThreads : 1)
    	for (long i=0; i<size; i++){
		CellHandle& newCell = NewTes.cellHandles[i];
        #else
	FOREACH(CellHandle& newCell, NewTes.cellHandles){
        #endif
		CVector center (0,0,0);
		if (newCell->info().fictious()==0) for (int k=0;k<4;k++) center = center + 0.25*(Tes.vertex(newCell->vertex(k)->info().id())->point()-CGAL::ORIGIN);		
		
		CellHandle oldCell = Tri.locate(Point(center[0],center[1],center[2]));
		newCell->info().crack = oldCell->info().crack;
//		For later commit newCell->info().fractureTip = oldCell->info().fractureTip;
//		For later commit newCell->info().cellHalfWidth = oldCell->info().cellHalfWidth;

		/// compute leakoff rate by summing the flow through facets abutting non-cracked neighbors
		if (oldCell->info().crack && !oldCell->info().fictious()){
			Real facetFlowRate=0;
			facetFlowRate -= oldCell->info().dv();
			for (int k=0; k<4;k++) {
				if (!oldCell->neighbor(k)->info().crack){
					facetFlowRate = oldCell->info().kNorm()[k]*(oldCell->info().shiftedP()-oldCell->neighbor(k)->info().shiftedP());
					leakOffRate += facetFlowRate;
				}
			}
		}
	}
    }

void DFNFlowEngine::trickPermeability(RTriangulation::Facet_circulator& facet,Real aperture,RTriangulation::Finite_edges_iterator& ed_it)
{
	const RTriangulation::Facet& currentFacet = *facet; /// seems verbose but facet->first was declaring a junk cell and crashing program (https://bugs.launchpad.net/yade/+bug/1666339)
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	const CellHandle& cell1 = currentFacet.first;
	const CellHandle& cell2 = currentFacet.first->neighbor(facet->second);
	if (Tri.is_infinite(cell1) || Tri.is_infinite(cell2)) cerr<<"Infinite cell found in trickPermeability, should be handled somehow, maybe"<<endl;
	
// 	/// ROBERT
// 	if (cell1->info().count()[currentFacet.second] < 3){
// 		cell1->info().count()[currentFacet.second] += 1;
// 		//cell1->info().aperture()[currentFacet.second] += aperture // used with avgAperture below if desired
// 	}
// 	if (!edgeOnJoint && cell1->info().count()[currentFacet.second] < facetEdgeBreakThreshold) return; // only allow facets with 2 or 3 broken edges to be tricked
	// Need to decide aperture criteria! Otherwise it selects the most recent broken edge aperture for facet's perm calc. Here is one possibility: 
	//Real avgAperture = cell1->info().aperture()[currentFacet.second]/Real(cell1->info().count()[currentFacet.second]);
	
	cell1->info().kNorm()[currentFacet.second] = cell2->info().kNorm()[Tri.mirror_index(cell1,currentFacet.second)] = apertureFactor*pow((aperture),3)/(12*viscosity);
	/// For vtk recorder:
	cell1->info().crack=1;
	cell2->info().crack=1;
	cell2->info().blocked = cell1->info().blocked = cell2->info().Pcondition = cell1->info().Pcondition = false; /// those ones will be included in the flow problem
	Point& CellCentre1 = cell1->info(); /// Trying to get fracture's surface 
	Point& CellCentre2 = cell2->info(); /// Trying to get fracture's surface 
	CVector networkFractureLength = CellCentre1 - CellCentre2; /// Trying to get fracture's surface 
	double networkFractureDistance = sqrt(networkFractureLength.squared_length()); /// Trying to get fracture's surface 
	Real networkFractureArea = pow(networkFractureDistance,2);  /// Trying to get fracture's surface 
	totalFractureArea += networkFractureArea; /// Trying to get fracture's surface 
// 	cout <<" ------------------ The total surface area up to here is --------------------" << totalFractureArea << endl;
// 	printFractureTotalArea = totalFractureArea; /// Trying to get fracture's surface 
	if (calcCrackArea) {
			CVector edge = ed_it->first->vertex(ed_it->second)->point().point() - ed_it->first->vertex(ed_it->third)->point().point();
			CVector unitV = edge*(1./sqrt(edge.squared_length()));
			Point p3 = ed_it->first->vertex(ed_it->third)->point().point() + unitV*(cell1->info() - ed_it->first->vertex(ed_it->third)->point().point())*unitV;
			Real halfCrackArea = 0.25*sqrt(std::abs(cross_product(CellCentre1-p3,CellCentre2-p3).squared_length()));//
			cell1->info().crackArea += halfCrackArea;
			cell2->info().crackArea += halfCrackArea;
			crackArea += 2*halfCrackArea;
		}
}

void DFNFlowEngine::trickPermeability(RTriangulation::Finite_edges_iterator& edge,Real aperture)
{
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	RTriangulation::Facet_circulator facet1 = Tri.incident_facets(*edge);
	RTriangulation::Facet_circulator facet0 = facet1++;
	trickPermeability(facet0,aperture,edge);
	while (facet1!=facet0) {trickPermeability(facet1,aperture,edge); facet1++;}
	/// Needs the fracture surface for this edge?
// 	double edgeArea = solver->T[solver->currentTes].computeVFacetArea(edge); cout<<"edge area="<<edgeArea<<endl;
}

void DFNFlowEngine::trickPermeability(Solver* flow)
{
	leakOffRate=0;	
	const RTriangulation& Tri = flow->T[solver->currentTes].Triangulation();
	if (!first) interpolateCrack(solver->T[solver->currentTes],flow->T[flow->currentTes]);
	const JCFpmPhys* jcfpmphys;
	const shared_ptr<InteractionContainer> interactions = scene->interactions;
	int numberOfCrackedOrJointedInteractions=0; 
	Real SumOfApertures=0.; 
	averageAperture=0;
	maxAperture=0;
	crackArea=0;
	//Real totalFractureArea=0; /// Trying to get fracture's surface
// 	const shared_ptr<IGeom>& ig;
// 	const ScGeom* geom; // = static_cast<ScGeom*>(ig.get());
	FiniteEdgesIterator edge = Tri.finite_edges_begin();
	for( ; edge != Tri.finite_edges_end(); ++edge) {
	  
		const VertexInfo& vi1 = (edge->first)->vertex(edge->second)->info();
		const VertexInfo& vi2 = (edge->first)->vertex(edge->third)->info();
		const shared_ptr<Interaction>& interaction = interactions->find( vi1.id(),vi2.id() );
		
		if (interaction && interaction->isReal()) {
			if (edge->first->info().isFictious) continue; /// avoid trick permeability for fictitious
			jcfpmphys = YADE_CAST<JCFpmPhys*>(interaction->phys.get());
			
			if (jcfpmphys->isOnJoint || jcfpmphys->isBroken) {
				
				numberOfCrackedOrJointedInteractions+=1;
				
				/// here are some workarounds
				Real residualAperture = jcfpmphys->isOnJoint? jointsResidualAperture : inducedCracksResidualAperture; /// inducedCracksResidualAperture=0. by default
// 				/// ROBERT
// 				Real residualAperture;
// 				if (jcfpmphys->isOnJoint){
// 					residualAperture = jointsResidualAperture;
// 					edgeOnJoint = true; 
// 				}else{ 
// 					residualAperture = inducedCracksResidualAperture;					
// 					edgeOnJoint = false;
// 				}
				/// for injection slot (different from pre-existing fractures if needed)
				if (jcfpmphys->isOnSlot) {residualAperture = slotInitialAperture;}
				
				/// avoid trick permeability (permeability=matrix permeability)
// 				if ( !jcfpmphys->isOnJoint && (residualAperture <= 0.) ) continue;
				if ( (jcfpmphys->crackJointAperture<=0.) && (residualAperture<=0.) ) continue;
				
// 				Real aperture = jcfpmphys->crackJointAperture;
// 				if (aperture < residualAperture) aperture = residualAperture;
				Real aperture = (jcfpmphys->crackJointAperture <= residualAperture)? residualAperture : jcfpmphys->crackJointAperture;
			
// 				cout<<"aperture = " << aperture <<endl;
// 				if (aperture > valueToDefine) aperture = valueToDefine; /// to avoid problem for large deformations?
				if (aperture <= residualAperture) aperture = residualAperture;
				if (aperture > maxAperture) maxAperture = aperture; 
				SumOfApertures += aperture;
				
				trickPermeability(edge,aperture);
			};
		}
	}
	averageAperture = SumOfApertures/numberOfCrackedOrJointedInteractions; /// DEBUG
// 	cout << " Average aperture in joint ( -D ) = " << AverageAperture << endl; /// DEBUG
}

// Real DFNFlowEngine::computeTotalFractureArea(totalFractureArea,printFractureTotalArea) /// Trying to get fracture's surface
// {
// 	 if (printFractureTotalArea >0) {
// 		cout<< " The total fracture area computed from the Network is: " << totalFractureArea <<endl;
// 	 }
// }

#endif //DFNFLOW
#endif //FLOWENGINE
