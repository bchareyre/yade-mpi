/*************************************************************************
*  Copyright (C) 2012 by Chao Yuan & Bruno Chareyre                      *
*  chao.yuan@3sr-grenoble.fr & bruno.chareyre@hmg.inpg.fr                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<yade/core/PartialEngine.hpp>
#include<yade/pkg/dem/TriaxialCompressionEngine.hpp>
#include<yade/pkg/dem/TesselationWrapper.hpp>
#include<yade/lib/triangulation/FlowBoundingSphere.hpp>

class Flow;
class TesselationWrapper;
#define _FlowSolver CGT::FlowBoundingSphere<FlowTesselation>
#define TPL template<class Solver>

class UnsaturatedEngine : public PartialEngine
{
	public :
	typedef _FlowSolver							FlowSolver;
	typedef FlowTesselation							Tesselation;
	typedef FlowSolver::RTriangulation					RTriangulation;
	typedef FlowSolver::Finite_vertices_iterator                    	Finite_vertices_iterator;
	typedef FlowSolver::Finite_cells_iterator				Finite_cells_iterator;
	typedef FlowSolver::Cell_handle						Cell_handle;
	typedef RTriangulation::Finite_edges_iterator				Finite_edges_iterator;


	
	protected:
		shared_ptr<FlowSolver> solver;
		struct posData {Body::id_t id; Vector3r pos; Real radius; bool isSphere; bool exists; posData(){exists=0;}};
		vector<posData> positionBufferCurrent;//reflect last known positions before we start computations
		vector<posData> positionBufferParallel;//keep the positions from a given step for multithread factorization
// 		//copy positions in a buffer for faster and/or parallel access
		void setPositionsBuffer(bool current);
		void testFunction();

	public :
		enum {wall_left=0, wall_right, wall_bottom, wall_top, wall_back, wall_front};
		Vector3r normal [6];
		bool currentTes;
		int id_offset;
		TPL void initSolver (Solver& flow);
		TPL void Triangulate (Solver& flow);
		TPL void AddBoundary (Solver& flow);
		TPL void Build_Triangulation (double P_zero, Solver& flow);
		TPL void Build_Triangulation (Solver& flow);
		TPL void Initialize_volumes (Solver& flow);
		TPL void BoundaryConditions(Solver& flow);

		TPL unsigned int imposePressure(Vector3r pos, Real p,Solver& flow);
		TPL void setImposedPressure(unsigned int cond, Real p,Solver& flow);
		TPL void clearImposedPressure(Solver& flow);

		template<class Cellhandle>
		Real Volume_cell_single_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_double_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_triple_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell (Cellhandle cell);
		void saveVtk() {solver->saveVtk();}
		python::list getConstrictions() {
			vector<Real> csd=solver->getConstrictions(); python::list pycsd;
			for (unsigned int k=0;k<csd.size();k++) pycsd.append(csd[k]); return pycsd;}
		double MeasurePorePressure(Vector3r pos){return solver->MeasurePorePressure(pos[0], pos[1], pos[2]);}
		TPL int getCell(double posX, double posY, double posZ, Solver& flow){return flow->getCell(posX, posY, posZ);}

		void emulateAction(){
			scene = Omega::instance().getScene().get();
			action();}
		unsigned int 	_imposePressure(Vector3r pos, Real p) {return imposePressure(pos,p,solver);}
		void 		_setImposedPressure(unsigned int cond, Real p) {setImposedPressure(cond,p,solver);}
		void 		_clearImposedPressure() {clearImposedPressure(solver);}
		int		_getCell(Vector3r pos) {return getCell(pos[0],pos[1],pos[2],solver);}

		virtual ~UnsaturatedEngine();

		virtual void action();

		YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(UnsaturatedEngine,PartialEngine,"Preliminary version of a model for unsaturated soils",
					((bool,first,true,,"Controls the initialization/update phases"))
					((bool, Debug, false,,"Activate debug messages"))
					((double, wall_thickness,0.001,,"Walls thickness"))
					((double,P_zero,0,,"Initial internal pressure"))
					((double, porosity, 0,,"Porosity computed at each retriangulation"))
					((bool, Flow_imposed_TOP_Boundary, true,, "if false involve pressure imposed condition"))
					((bool, Flow_imposed_BOTTOM_Boundary, true,, "if false involve pressure imposed condition"))
					((bool, Flow_imposed_FRONT_Boundary, true,, "if false involve pressure imposed condition"))
					((bool, Flow_imposed_BACK_Boundary, true,, "if false involve pressure imposed condition"))
					((bool, Flow_imposed_LEFT_Boundary, true,, "if false involve pressure imposed condition"))
					((bool, Flow_imposed_RIGHT_Boundary, true,,"if false involve pressure imposed condition"))
					((double, Pressure_TOP_Boundary, 0,, "Pressure imposed on top boundary"))
					((double, Pressure_BOTTOM_Boundary,  0,, "Pressure imposed on bottom boundary"))
					((double, Pressure_FRONT_Boundary,  0,, "Pressure imposed on front boundary"))
					((double, Pressure_BACK_Boundary,  0,,"Pressure imposed on back boundary"))
					((double, Pressure_LEFT_Boundary,  0,, "Pressure imposed on left boundary"))
					((double, Pressure_RIGHT_Boundary,  0,, "Pressure imposed on right boundary"))
					((int, wallTopId,3,,"Id of top boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallBottomId,2,,"Id of bottom boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallFrontId,5,,"Id of front boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallBackId,4,,"Id of back boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallLeftId,0,,"Id of left boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallRightId,1,,"Id of right boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((bool, BOTTOM_Boundary_MaxMin, 1,,"If true (default value) bounding sphere is added as function of max/min sphere coord, if false as function of yade wall position"))
					((bool, TOP_Boundary_MaxMin, 1,,"If true (default value) bounding sphere is added as function of max/min sphere coord, if false as function of yade wall position"))
					((bool, RIGHT_Boundary_MaxMin, 1,,"If true (default value) bounding sphere is added as function of max/min sphere coord, if false as function of yade wall position"))
					((bool, LEFT_Boundary_MaxMin, 1,,"If true (default value) bounding sphere is added as function of max/min sphere coord, if false as function of yade wall position"))
					((bool, FRONT_Boundary_MaxMin, 1,,"If true (default value) bounding sphere is added as function of max/min sphere coord, if false as function of yade wall position"))
					((bool, BACK_Boundary_MaxMin, 1,,"If true (default value) bounding sphere is added as function of max/min sphere coord, if false as function of yade wall position"))
					,
					/*deprec*/
					,,
					for (int i=0; i<6; ++i){normal[i]=Vector3r::Zero();}
					normal[wall_bottom].y()=normal[wall_left].x()=normal[wall_back].z()=1;
					normal[wall_top].y()=normal[wall_right].x()=normal[wall_front].z()=-1;
					solver = shared_ptr<FlowSolver> (new FlowSolver);
					first=true;
					,
					.def("imposePressure",&UnsaturatedEngine::_imposePressure,(python::arg("pos"),python::arg("p")),"Impose pressure in cell of location 'pos'. The index of the condition is returned (for multiple imposed pressures at different points).")
					.def("setImposedPressure",&UnsaturatedEngine::_setImposedPressure,(python::arg("cond"),python::arg("p")),"Set pressure value at the point indexed 'cond'.")
					.def("clearImposedPressure",&UnsaturatedEngine::_clearImposedPressure,"Clear the list of points with pressure imposed.")
					.def("getConstrictions",&UnsaturatedEngine::getConstrictions,"Get the list of constrictions (inscribed circle) for all finite facets.")
					.def("saveVtk",&UnsaturatedEngine::saveVtk,"Save pressure field in vtk format.")
					.def("MeasurePorePressure",&UnsaturatedEngine::MeasurePorePressure,(python::arg("pos")),"Measure pore pressure in position pos[0],pos[1],pos[2]")
					.def("emulateAction",&UnsaturatedEngine::emulateAction,"get scene and run action (may be used to manipulate engine outside the main loop).")
					.def("getCell",&UnsaturatedEngine::_getCell,(python::arg("pos")),"get id of the cell containing (X,Y,Z).")
					.def("testFunction",&UnsaturatedEngine::testFunction,"The playground for Chao's experiments.")
					)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(UnsaturatedEngine);
