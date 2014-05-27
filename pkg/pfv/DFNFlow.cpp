 
/*************************************************************************
*  Copyright (C) 2014 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE

//keep this #ifdef for commited versions unless you really have stable version that should be compiled by default
//it will save compilation time for everyone else
//when you want it compiled, you can pass -DDFNFLOW to cmake, or just uncomment the following line
// #define DFNFLOW
#ifdef DFNFLOW
#define TEMPLATE_FLOW_NAME DFNFlowEngineT
#include <yade/pkg/pfv/FlowEngine.hpp>

class DFNCellInfo : public FlowCellInfo
{
	public:
	Real anotherVariable;
	void anotherFunction() {};
};


class DFNVertexInfo : public FlowVertexInfo {
	public:
	//same here if needed
};

typedef TemplateFlowEngine<DFNCellInfo,DFNVertexInfo> DFNFlowEngineT;
REGISTER_SERIALIZABLE(DFNFlowEngineT);
YADE_PLUGIN((DFNFlowEngineT));

class DFNFlowEngine : public DFNFlowEngineT
{
	public :
	void trickPermeability();
	void trickPermeability (RTriangulation::Facet_circulator& facet,Real somethingBig);
	void trickPermeability (RTriangulation::Finite_edges_iterator& edge,Real somethingBig);
	void setPositionsBuffer(bool current);

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(DFNFlowEngine,DFNFlowEngineT,"documentation here",
	((Real, myNewAttribute, 0,,"useless example"))
	((bool, updatePositions, False,,"update particles positions when rebuilding the mesh (experimental)"))
	,/*DFNFlowEngineT()*/,
	,
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


void DFNFlowEngine::trickPermeability (RTriangulation::Facet_circulator& facet, Real somethingBig)
{
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	const CellHandle& cell1 = facet->first;
	const CellHandle& cell2 = facet->first->neighbor(facet->second);
	if ( Tri.is_infinite(cell1) || Tri.is_infinite(cell2)) cerr<<"Infinite cell found in trickPermeability, should be handled somehow, maybe"<<endl;
	cell1->info().kNorm()[facet->second] = somethingBig;
	cell2->info().kNorm()[Tri.mirror_index(cell1, facet->second)] = somethingBig;
}

void DFNFlowEngine::trickPermeability(RTriangulation::Finite_edges_iterator& edge, Real somethingBig)
{
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	RTriangulation::Facet_circulator facet1 = Tri.incident_facets(*edge);
	RTriangulation::Facet_circulator facet0=facet1++;
	trickPermeability(facet0,somethingBig);
	while ( facet1!=facet0 ) {trickPermeability(facet1,somethingBig); facet1++;}
}


void DFNFlowEngine::trickPermeability()
{
	Real somethingBig=10;//is that big??
	const RTriangulation& Tri = solver->T[solver->currentTes].Triangulation();
	//We want to change permeability perpendicular to the 10th edge, let's say.
	//in the end this function should have a loop on all edges I guess
	FiniteEdgesIterator edge = Tri.finite_edges_begin();
	for(int k=10; k>0;--k) edge++;
	trickPermeability(edge,somethingBig);
}


#endif //DFNFLOW
#endif //FLOWENGINE