 
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

// #define DFNFLOW

#ifdef DFNFLOW
#include "FlowEngine_DFNFlowEngineT.hpp"

class DFNCellInfo : public FlowCellInfo_DFNFlowEngineT
{
	public:
	Real anotherVariable;
	bool crack;
// 	bool preExistingJoint;
// 	void anotherFunction() {};
// 	DFNCellInfo() : FlowCellInfo(),crack(false)  {}
	DFNCellInfo() : crack(false)  {}
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
        static unsigned int number = 0;
        char filename[80];
	mkdir(folder, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        sprintf(filename,"%s/out_%d.vtk",folder,number++);
	int firstReal=-1;

	//count fictious vertices and cells
	vtkInfiniteVertices=vtkInfiniteCells=0;
 	FiniteCellsIterator cellEnd = Tri.finite_cells_end();
        for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != cellEnd; cell++) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (!isDrawable) vtkInfiniteCells+=1;
	}
	for (FiniteVerticesIterator v = Tri.finite_vertices_begin(); v != Tri.finite_vertices_end(); ++v) {
                if (!v->info().isReal()) vtkInfiniteVertices+=1;
                else if (firstReal==-1) firstReal=vtkInfiniteVertices;
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
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
        	if (isDrawable){vtkfile.write_cell(cell->vertex(0)->info().id()-firstReal, cell->vertex(1)->info().id()-firstReal, cell->vertex(2)->info().id()-firstReal, cell->vertex(3)->info().id()-firstReal);}
        }
        vtkfile.end_cells();

	if (permeabilityMap){
	vtkfile.begin_data("Permeability",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().s);}
	}
	vtkfile.end_data();}
	else{
	vtkfile.begin_data("Pressure",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().p());}
	}
	vtkfile.end_data();}

	if (1){
	averageRelativeCellVelocity();
	vtkfile.begin_data("Velocity",CELL_DATA,VECTORS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
		if (isDrawable){vtkfile.write_data(cell->info().averageVelocity()[0],cell->info().averageVelocity()[1],cell->info().averageVelocity()[2]);}
	}
	vtkfile.end_data();}
	
	if(1){
	vtkfile.begin_data("fracturedCells",CELL_DATA,SCALARS,FLOAT);
	for (FiniteCellsIterator cell = Tri.finite_cells_begin(); cell != Tri.finite_cells_end(); ++cell) {
		bool isDrawable = cell->info().isReal() && cell->vertex(0)->info().isReal() && cell->vertex(1)->info().isReal() && cell->vertex(2)->info().isReal()  && cell->vertex(3)->info().isReal();
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
	void trickPermeability();
	void trickPermeability (RTriangulation::Facet_circulator& facet,Real aperture, Real residualAperture);
	void trickPermeability (RTriangulation::Finite_edges_iterator& edge,Real aperture, Real residualAperture);
	void setPositionsBuffer(bool current);
// 	void computeTotalFractureArea(Real totalFracureArea,bool printFractureTotalArea);/// Trying to get fracture's surface
	Real totalFracureArea; /// Trying to get fracture's surface

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DFNFlowEngine,DFNFlowEngineT,"This is an enhancement of the FlowEngine for intact and fractured rocks that takes into acount pre-existing discontinuities and bond breakage between particles. The local conductivity around the broken link is calculated according to parallel plates model",
	((Real, jointResidual, 0,,"calibration parameter for residual aperture of joints"))
	((bool, updatePositions, false,,"update particles positions when rebuilding the mesh (experimental)"))
 	((bool, printFractureTotalArea, 0,,"The final fracture area computed through the network")) /// Trying to get fracture's surface
	,
	,
	,
// 	.def("computeTotalFractureArea",&DFNFlowEngineT::computeTotalFractureArea," Compute and print the total fracture area of the network") /// Trying to get fracture's surface
// 	.def("trickPermeability",&DFNFlowEngineT::trickPermeability,"measure the mean trickPermeability in the period")
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(DFNFlowEngine);
YADE_PLUGIN((DFNFlowEngine));

//In this version, we never update positions when !updatePositions, i.e. keep triangulating the same positions
void DFNFlowEngine::setPositionsBuffer(bool current)
{
	vector<posData>& buffer = current? positionBufferCurrent : positionBufferParallel;
	if (!updatePositions && buffer.size()>0) return;
	buffer.clear();
	buffer.resize(scene->bodies->size());
	shared_ptr<Sphere> sph ( new Sphere );
        const int Sph_Index = sph->getClassIndexStatic();
	FOREACH ( const shared_ptr<Body>& b, *scene->bodies ) {
                if (!b || ignoredBody==b->getId()) continue;
                posData& dat = buffer[b->getId()];
		dat.id=b->getId();
		dat.pos=b->state->pos;
		dat.isSphere= (b->shape->getClassIndex() ==  Sph_Index);
		if (dat.isSphere) dat.radius = YADE_CAST<Sphere*>(b->shape.get())->radius;
		dat.exists=true;
	}
}

void DFNFlowEngine::trickPermeability(RTriangulation::Facet_circulator& facet, Real aperture, Real residualAperture)
{
	const RTriangulation::Facet& currentFacet = *facet; // seems verbose but facet->first was declaring a junk cell and crashing program (https://bugs.launchpad.net/yade/+bug/1666339)
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	const CellHandle& cell1 = currentFacet.first;
	const CellHandle& cell2 = currentFacet.first->neighbor(facet->second);
	if ( Tri.is_infinite(cell1) || Tri.is_infinite(cell2)) cerr<<"Infinite cell found in trickPermeability, should be handled somehow, maybe"<<endl;
	cell1->info().kNorm()[currentFacet.second]=cell2->info().kNorm()[Tri.mirror_index(cell1, currentFacet.second)] = pow((aperture+residualAperture),3)/(12*viscosity);
	//For vtk recorder:
	cell1->info().crack= 1;
	cell2->info().crack= 1;
	cell2->info().blocked = cell1->info().blocked = cell2->info().Pcondition = cell1->info().Pcondition = false;//those ones will be included in the flow problem
	Point& CellCentre1 = cell1->info(); /// Trying to get fracture's surface 
	Point& CellCentre2 = cell2->info(); /// Trying to get fracture's surface 
	CVector networkFractureLength = CellCentre1 - CellCentre2; /// Trying to get fracture's surface 
	double networkFractureDistance = sqrt(networkFractureLength.squared_length()); /// Trying to get fracture's surface 
	Real networkFractureArea = pow(networkFractureDistance,2);  /// Trying to get fracture's surface 
	totalFracureArea += networkFractureArea; /// Trying to get fracture's surface 
// 	cout <<" ------------------ The total surface area up to here is --------------------" << totalFracureArea << endl;
// 	printFractureTotalArea = totalFracureArea; /// Trying to get fracture's surface 
}

void DFNFlowEngine::trickPermeability(RTriangulation::Finite_edges_iterator& edge, Real aperture, Real residualAperture)
{
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	RTriangulation::Facet_circulator facet1 = Tri.incident_facets(*edge);
	RTriangulation::Facet_circulator facet0=facet1++;
	trickPermeability(facet0, aperture,residualAperture);
	while ( facet1!=facet0 ) {trickPermeability(facet1, aperture, residualAperture); facet1++;}
	/// Needs the fracture surface for this edge?
// 	double edgeArea = solver->T[solver->currentTes].computeVFacetArea(edge); cout<<"edge area="<<edgeArea<<endl;
}

void DFNFlowEngine::trickPermeability()
{
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	const JCFpmPhys* jcfpmphys;
	const shared_ptr<InteractionContainer> interactions = scene->interactions;
	int numberOfCrackedOrJoinedInteractions = 0; /// DEBUG
	Real SumOfApertures = 0.; /// DEBUG
	Real AverageAperture =0; /// DEBUG
	Real totalFracureArea=0; /// Trying to get fracture's surface
// 	const shared_ptr<IGeom>& ig;
// 	const ScGeom* geom; // = static_cast<ScGeom*>(ig.get());
	FiniteEdgesIterator edge = Tri.finite_edges_begin();
	for( ; edge!= Tri.finite_edges_end(); ++edge) {
	  
		const VertexInfo& vi1=(edge->first)->vertex(edge->second)->info();
		const VertexInfo& vi2=(edge->first)->vertex(edge->third)->info();
		const shared_ptr<Interaction>& interaction=interactions->find( vi1.id(),vi2.id() );
		
		if (interaction && interaction->isReal()) {
			
			jcfpmphys = YADE_CAST<JCFpmPhys*>(interaction->phys.get());
			
			if ( jcfpmphys->isOnJoint || jcfpmphys->isBroken ) {
				
				numberOfCrackedOrJoinedInteractions +=1;
				
				// here are some workarounds
// 				Real residualAperture = jcfpmphys->isOnJoint? jointResidual : 0.1*jointResidual;
				Real residualAperture = jcfpmphys->isOnJoint? jointResidual : 0.; // do we define a residual aperture for induced cracks?
// 				Real residualAperture = jointResidual;
// 				cout<<"residual aperture = " << residualAperture <<endl;
				
				if (!jcfpmphys->isOnJoint && jcfpmphys->crackJointAperture<=0) continue; // avoid trick permeability for closed induced crack (permeability=matrix permeability)
				
				Real aperture = jcfpmphys->crackJointAperture;
// 				cout<<"aperture = " << aperture <<endl;

				SumOfApertures += aperture;
				trickPermeability(edge, aperture, residualAperture);
// 				trickPermeability(edge, jcfpmphys->crackJointAperture, residualAperture); // we should be able to use this line
				
			};	
		}
	}
	AverageAperture = SumOfApertures/numberOfCrackedOrJoinedInteractions; /// DEBUG
// 	cout << " Average aperture in joint ( -D ) = " << AverageAperture << endl; /// DEBUG
}

// Real DFNFlowEngine::computeTotalFractureArea(totalFracureArea,printFractureTotalArea) /// Trying to get fracture's surface
// {
// 	 if (printFractureTotalArea >0) {
// 		cout<< " The total fracture area computed from the Network is: " << totalFracureArea <<endl;
// 	 }
// }

#endif //DFNFLOW
#endif //FLOWENGINE
