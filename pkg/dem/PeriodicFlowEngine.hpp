/*************************************************************************
*  Copyright (C) 2009 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*  Copyright (C) 2012 by Donia Marzougui <donia.marzougui@grenoble-inp.fr>*
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/// The periodic variant of FlowEngine is defined here. It should become a template class for more flexibility.
/// It is a bit more complicated as for FlowEngine, though, because we need template inheriting from template, which breaks YADE_CLASS_XXX logic_error
/// See below the commented exemple, for a possible solution

#include <yade/pkg/dem/FlowEngine.hpp>

class PeriodicCellInfo : public FlowCellInfo
{	
	public:
	static CVector gradP;
	//for real cell, baseIndex is the rank of the cell in cellHandles. For ghost cells, it is the baseIndex of the corresponding real cell.
	//Unlike ordinary index, baseIndex is also indexing cells with imposed pressures
	int baseIndex;
	int period[3];
	static CVector hSize[3];
	static CVector deltaP;
	int ghost;
	Real* _pression;
	PeriodicCellInfo (void){
		_pression=&pression;
		period[0]=period[1]=period[2]=0;
		baseIndex=-1;
		volumeSign=0;}
	~PeriodicCellInfo (void) {}

	inline const double shiftedP (void) const {return isGhost? (*_pression)+pShift() :(*_pression) ;}
	inline const double pShift (void) const {return deltaP[0]*period[0] + deltaP[1]*period[1] +deltaP[2]*period[2];}
// 	inline const double p (void) {return shiftedP();}
	inline void setP (const Real& p) {pression=p;}
	bool isReal (void) {return !(isFictious || isGhost);}
};


class PeriodicVertexInfo : public FlowVertexInfo {
	public:
	PeriodicVertexInfo& operator= (const CVector &u) { CVector::operator= (u); return *this; }
	PeriodicVertexInfo& operator= (const float &scalar) { s=scalar; return *this; }
	PeriodicVertexInfo& operator= (const unsigned int &id) { i= id; return *this; }
	int period[3];
	//FIXME: the name is misleading, even non-ghost can be out of the period and therefore they need to be shifted as well
	inline const CVector ghostShift (void) {
		return period[0]*PeriodicCellInfo::hSize[0]+period[1]*PeriodicCellInfo::hSize[1]+period[2]*PeriodicCellInfo::hSize[2];}
	PeriodicVertexInfo (void) {isFictious=false; s=0; i=0; period[0]=period[1]=period[2]=0; isGhost=false;}
	bool isReal (void) {return !(isFictious || isGhost);}
};

typedef CGT::TriangulationTypes<PeriodicVertexInfo,PeriodicCellInfo>			PeriFlowTriangulationTypes;
typedef CGT::PeriodicTesselation<CGT::_Tesselation<PeriFlowTriangulationTypes> >	PeriFlowTesselation;
#ifdef LINSOLV
#define _PeriFlowSolver CGT::PeriodicFlowLinSolv<PeriFlowTesselation>
#else
#define _PeriFlowSolver CGT::PeriodicFlow<PeriFlowTesselation>
#endif
//CGT::PeriodicFlowLinSolv<CGT::PeriodicTesselation<CGT::_Tesselation<TriangulationTypes<PeriodicVertexInfo,PeriodicCellInfo> > > >

typedef TemplateFlowEngine<	PeriodicCellInfo,
				PeriodicVertexInfo,
				CGT::PeriodicTesselation<CGT::_Tesselation<CGT::TriangulationTypes<PeriodicVertexInfo,PeriodicCellInfo> > >,
				_PeriFlowSolver>
				FlowEngine_PeriodicInfo;

REGISTER_SERIALIZABLE(FlowEngine_PeriodicInfo);
YADE_PLUGIN((FlowEngine_PeriodicInfo));


class PeriodicFlowEngine : public FlowEngine_PeriodicInfo
{
	public :
		void triangulate (FlowSolver& flow);
		void buildTriangulation (Real pzero, FlowSolver& flow);
		void initializeVolumes (FlowSolver&  flow);
		void updateVolumes (FlowSolver&  flow);
		Real volumeCell (CellHandle cell);

		Real volumeCellSingleFictious (CellHandle cell);
		inline void locateCell(CellHandle baseCell, unsigned int& index, int& baseIndex, FlowSolver& flow, unsigned int count=0);
		Vector3r meanVelocity();

		virtual ~PeriodicFlowEngine();

		virtual void action();
		//Cache precomputed values for pressure shifts, based on current hSize and pGrad
		void preparePShifts();
		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(PeriodicFlowEngine,FlowEngine_PeriodicInfo,"A variant of :yref:`FlowEngine` implementing periodic boundary conditions. The API is very similar.",
		((Real,duplicateThreshold, 0.06,,"distance from cell borders that will triger periodic duplication in the triangulation |yupdate|"))
		((Vector3r, gradP, Vector3r::Zero(),,"Macroscopic pressure gradient"))
		,,
		wallIds=vector<int>(6,-1);
		solver = shared_ptr<FlowSolver> (new FlowSolver);
		epsVolMax=epsVolCumulative=retriangulationLastIter=0;
		ReTrg=1;
		first=true;
		,
		//nothing special to define, we re-use FlowEngine methods
		//.def("meanVelocity",&PeriodicFlowEngine::meanVelocity,"measure the mean velocity in the period")
		)
		DECLARE_LOGGER;


};
REGISTER_SERIALIZABLE(PeriodicFlowEngine);