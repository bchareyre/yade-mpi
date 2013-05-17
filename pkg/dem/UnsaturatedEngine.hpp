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
	typedef RTriangulation::Vertex_handle					Vertex_handle;
	typedef RTriangulation::Point						CGALSphere;
	typedef CGALSphere::Point						Point;

	
	protected:
		shared_ptr<FlowSolver> solver;
		shared_ptr<FlowSolver> backgroundSolver;
		volatile bool backgroundCompleted;
		Cell cachedCell;
		struct posData {Body::id_t id; Vector3r pos; Real radius; bool isSphere; bool exists; posData(){exists=0;}};
		vector<posData> positionBufferCurrent;//reflect last known positions before we start computations
		vector<posData> positionBufferParallel;//keep the positions from a given step for multithread factorization
// 		//copy positions in a buffer for faster and/or parallel access
		void setPositionsBuffer(bool current);
		Real testFunction();

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
		TPL void initializeCellIndex(Solver& flow);
		TPL void get_pore_radius(Solver& flow);

		TPL unsigned int imposePressure(Vector3r pos, Real p,Solver& flow);
		TPL void setImposedPressure(unsigned int cond, Real p,Solver& flow);
		TPL void clearImposedPressure(Solver& flow);
		TPL void invadeSingleCell(Cell_handle cell, double pressure, Solver& flow);
		TPL void invade (Solver& flow );
		TPL Real get_min_EntryValue (Solver& flow );
		TPL Real getSaturation(Solver& flow);
		TPL int saveListOfNodes(Solver& flow);
		TPL int saveListOfConnections(Solver& flow);
// 		TPL int saveCellSphereRadius(Solver& flow);//temp		
// 		TPL int saveLatticeNodes(Solver& flow); //not work
// 		template<class Cellhandle>
// 		int printLength(Cellhandle cell);//temp
		template<class Cellhandle>
		Real Volume_cell_single_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_double_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_triple_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell (Cellhandle cell);
		template<class Cellhandle>
		Real capillary_Volume_cell (Cellhandle cell);
		template<class Cellhandle>
		double compute_EffPoreRadius(Cellhandle cell, int j);
		template<class Cellhandle>
		double bisection(Cellhandle cell, int j, double a, double b);
		template<class Cellhandle>
		double computeDeltaPressure(Cellhandle cell,int j, double rcap);
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
		void 		_buildTriangulation() {setPositionsBuffer(true); Build_Triangulation(solver);}
		void		_invade() {invade(solver);}
		Real		_get_min_EntryValue() {return get_min_EntryValue(solver);}
		Real 		_getSaturation () {return getSaturation(solver);}
		int		_saveListOfNodes() {return saveListOfNodes(solver);}
		int		_saveListOfConnections() {return saveListOfConnections(solver);}
// 		int		_saveCellSphereRadius() {return saveCellSphereRadius(solver);}
// 		int		_saveLatticeNodes() {return saveLatticeNodes(solver);}

		virtual ~UnsaturatedEngine();

		virtual void action();

		YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(UnsaturatedEngine,PartialEngine,"Preliminary version of a model for unsaturated soils",
					((bool,first,true,,"Controls the initialization/update phases"))
					((bool, Debug, false,,"Activate debug messages"))
					((double, wall_thickness,0.001,,"Walls thickness"))
					((double,P_zero,0,,"Initial internal pressure"))
					((double,gasPressure,0,,"Invasion pressure"))
					((double,surfaceTension,0.0728,,"Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))
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
					.def("buildTriangulation",&UnsaturatedEngine::_buildTriangulation,"Triangulate spheres of the current scene.")
					.def("getSaturation",&UnsaturatedEngine::_getSaturation,"get saturation")
					.def("getMinEntryValue",&UnsaturatedEngine::_get_min_EntryValue,"get the minimum air entry pressure for the next invade step")
					.def("saveListOfNodes",&UnsaturatedEngine::_saveListOfNodes,"Save the list of nodes.")
					.def("saveListOfConnnections",&UnsaturatedEngine::_saveListOfConnections,"Save the connections between cells.")
// 					.def("saveCellSphereRadius",&UnsaturatedEngine::_saveCellSphereRadius,"temp file for saving cells and sphere radius")
// 					.def("saveLatticeNodes",&UnsaturatedEngine::_saveLatticeNodes,"Save the statement of lattice nodes. 0 for out of sphere; 1 for inside of sphere.")
					.def("invade",&UnsaturatedEngine::_invade,"Run the drainage invasion from all cells with air pressure. ")
					)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(UnsaturatedEngine);
