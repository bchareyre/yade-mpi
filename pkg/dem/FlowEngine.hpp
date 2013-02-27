/*************************************************************************
*  Copyright (C) 2009 by Emanuele Catalano                               *
*  emanuele.catalano@hmg.inpg.fr                                            *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#pragma once
#include<yade/core/PartialEngine.hpp>
#include<yade/pkg/dem/TriaxialCompressionEngine.hpp>
#include<yade/pkg/dem/TesselationWrapper.hpp>
#include<yade/lib/triangulation/FlowBoundingSphere.hpp>
#include<yade/lib/triangulation/PeriodicFlow.hpp>

class Flow;
class TesselationWrapper;
#ifdef LINSOLV
#define _FlowSolver CGT::FlowBoundingSphereLinSolv<CGT::FlowBoundingSphere<FlowTesselation> >
#else
#define _FlowSolver CGT::FlowBoundingSphere<FlowTesselation>
#endif

#define TPL template<class Solver>


class FlowEngine : public PartialEngine
{
	public :
	typedef _FlowSolver							FlowSolver;
	typedef FlowTesselation							Tesselation;
	typedef FlowSolver::RTriangulation					RTriangulation;
	typedef FlowSolver::Finite_vertices_iterator                    	Finite_vertices_iterator;
	typedef FlowSolver::Finite_cells_iterator				Finite_cells_iterator;
	typedef FlowSolver::Cell_handle						Cell_handle;
	typedef RTriangulation::Finite_edges_iterator				Finite_edges_iterator;
	typedef FlowSolver::Vertex_handle                    			Vertex_handle;


	
	protected:
		shared_ptr<FlowSolver> solver;
		shared_ptr<FlowSolver> backgroundSolver;
		volatile bool backgroundCompleted;
		Cell cachedCell;
		struct posData {Body::id_t id; Vector3r pos; Real radius; bool isSphere; bool exists; posData(){exists=0;}};
		vector<posData> positionBufferCurrent;//reflect last known positions before we start computations
		vector<posData> positionBufferParallel;//keep the positions from a given step for multithread factorization
		//copy positions in a buffer for faster and/or parallel access
		void setPositionsBuffer(bool current);

	public :
		int retriangulationLastIter;
		enum {wall_left=0, wall_right, wall_bottom, wall_top, wall_back, wall_front};
		Vector3r normal [6];
		bool currentTes;
		int id_offset;
		double Eps_Vol_Cumulative;
		int ReTrg;
		int ellapsedIter;
		TPL void initSolver (Solver& flow);
		TPL void Triangulate (Solver& flow);
		TPL void AddBoundary (Solver& flow);
		TPL void Build_Triangulation (double P_zero, Solver& flow);
		TPL void Build_Triangulation (Solver& flow);
		TPL void UpdateVolumes (Solver& flow);
		TPL void Initialize_volumes (Solver& flow);
		TPL void BoundaryConditions(Solver& flow);
		TPL void updateBCs ( Solver& flow ) {
			if (flow->T[flow->currentTes].max_id>0) BoundaryConditions(flow);//avoids crash at iteration 0, when the packing is not bounded yet
			else LOG_ERROR("updateBCs not applied");
			flow->pressureChanged=true;}

		TPL void imposeFlux(Vector3r pos, Real flux,Solver& flow);
		TPL unsigned int imposePressure(Vector3r pos, Real p,Solver& flow);
		TPL void setImposedPressure(unsigned int cond, Real p,Solver& flow);
		TPL void clearImposedPressure(Solver& flow);
		TPL void clearImposedFlux(Solver& flow);
		TPL void ComputeViscousForces(Solver& flow);
		TPL Real getCellFlux(unsigned int cond, const shared_ptr<Solver>& flow);
		TPL Real getBoundaryFlux(unsigned int boundary,Solver& flow) {return flow->boundaryFlux(boundary);}
		TPL Vector3r fluidForce(unsigned int id_sph, Solver& flow) {
			const CGT::Vecteur& f=flow->T[flow->currentTes].vertex(id_sph)->info().forces; return Vector3r(f[0],f[1],f[2]);}
		TPL Vector3r shearLubForce(unsigned int id_sph, Solver& flow) {
			return (flow->viscousShearForces.size()>id_sph)?flow->viscousShearForces[id_sph]:Vector3r::Zero();}
		TPL Vector3r shearLubTorque(unsigned int id_sph, Solver& flow) {
			return (flow->viscousShearTorques.size()>id_sph)?flow->viscousShearTorques[id_sph]:Vector3r::Zero();}
		TPL Vector3r normalLubForce(unsigned int id_sph, Solver& flow) {
			return (flow->normLubForce.size()>id_sph)?flow->normLubForce[id_sph]:Vector3r::Zero();}
		TPL Matrix3r bodyShearLubStress(unsigned int id_sph, Solver& flow) {
			return (flow->viscousBodyStress.size()>id_sph)?flow->viscousBodyStress[id_sph]:Matrix3r::Zero();}
		TPL Matrix3r bodyNormalLubStress(unsigned int id_sph, Solver& flow) {
			return (flow->lubBodyStress.size()>id_sph)?flow->lubBodyStress[id_sph]:Matrix3r::Zero();}
		TPL python::list getConstrictions(bool all, Solver& flow) {
			vector<Real> csd=flow->getConstrictions(); python::list pycsd;
			for (unsigned int k=0;k<csd.size();k++) if ((all && csd[k]!=0) || csd[k]>0) pycsd.append(csd[k]); return pycsd;}
		TPL python::list getConstrictionsFull(bool all, Solver& flow) {
			vector<Constriction> csd=flow->getConstrictionsFull(); python::list pycsd;
			for (unsigned int k=0;k<csd.size();k++) if ((all && csd[k].second[0]!=0) || csd[k].second[0]>0) {
				python::list cons;
				cons.append(csd[k].first.first);
				cons.append(csd[k].first.second);
				cons.append(csd[k].second[0]);
				cons.append(csd[k].second[1]);
				cons.append(csd[k].second[2]);
				cons.append(csd[k].second[3]);
				pycsd.append(cons);}
			return pycsd;}
		
		template<class Cellhandle>
		Real Volume_cell_single_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_double_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell_triple_fictious (Cellhandle cell);
		template<class Cellhandle>
		Real Volume_cell (Cellhandle cell);
		void Oedometer_Boundary_Conditions();
		void Average_real_cell_velocity();
		void saveVtk() {solver->saveVtk();}
		vector<Real> AvFlVelOnSph(unsigned int id_sph) {return solver->Average_Fluid_Velocity_On_Sphere(id_sph);}

		void setBoundaryVel(Vector3r vel) {topBoundaryVelocity=vel; Update_Triangulation=true;}
		void PressureProfile(double wallUpY, double wallDownY) {return solver->MeasurePressureProfile(wallUpY,wallDownY);}
		double MeasurePorePressure(Vector3r pos){return solver->MeasurePorePressure(pos[0], pos[1], pos[2]);}
		TPL int getCell(double posX, double posY, double posZ, Solver& flow){return flow->getCell(posX, posY, posZ);}
		double MeasureAveragedPressure(double posY){return solver->MeasureAveragedPressure(posY);}
		double MeasureTotalAveragedPressure(){return solver->MeasureTotalAveragedPressure();}
		#ifdef LINSOLV
		TPL void exportMatrix(string filename,Solver& flow) {if (useSolver==3) flow->exportMatrix(filename.c_str());
			else cerr<<"available for Cholmod solver (useSolver==3)"<<endl;}
		TPL void exportTriplets(string filename,Solver& flow) {if (useSolver==3) flow->exportTriplets(filename.c_str());
			else cerr<<"available for Cholmod solver (useSolver==3)"<<endl;}
		#endif

		void emulateAction(){
			scene = Omega::instance().getScene().get();
			action();}
		//Instanciation of templates for python binding
		Vector3r 	_shearLubForce(unsigned int id_sph) {return shearLubForce(id_sph,solver);}
		Vector3r 	_shearLubTorque(unsigned int id_sph) {return shearLubTorque(id_sph,solver);}
		Vector3r 	_normalLubForce(unsigned int id_sph) {return normalLubForce(id_sph,solver);}
		Matrix3r 	_bodyShearLubStress(unsigned int id_sph) {return bodyShearLubStress(id_sph,solver);}
		Matrix3r 	_bodyNormalLubStress(unsigned int id_sph) {return bodyNormalLubStress(id_sph,solver);}
		Vector3r 	_fluidForce(unsigned int id_sph) {return fluidForce(id_sph,solver);}
		void 		_imposeFlux(Vector3r pos, Real flux) {return imposeFlux(pos,flux,*solver);}
		unsigned int 	_imposePressure(Vector3r pos, Real p) {return imposePressure(pos,p,solver);}	
		void 		_setImposedPressure(unsigned int cond, Real p) {setImposedPressure(cond,p,solver);}
		void 		_clearImposedPressure() {clearImposedPressure(solver);}
		void 		_clearImposedFlux() {clearImposedFlux(solver);}
		void 		_updateBCs() {updateBCs(solver);}
		Real 		_getCellFlux(unsigned int cond) {return getCellFlux(cond,solver);}
		Real 		_getBoundaryFlux(unsigned int boundary) {return getBoundaryFlux(boundary,solver);}
		int		_getCell(Vector3r pos) {return getCell(pos[0],pos[1],pos[2],solver);}
		#ifdef LINSOLV
		void 		_exportMatrix(string filename) {exportMatrix(filename,solver);}
		void 		_exportTriplets(string filename) {exportTriplets(filename,solver);}
		#endif
		python::list 	_getConstrictions(bool all) {return getConstrictions(all,solver);}
		python::list 	_getConstrictionsFull(bool all) {return getConstrictionsFull(all,solver);}

		virtual ~FlowEngine();

		virtual void action();
		virtual void backgroundAction();

		YADE_CLASS_BASE_DOC_ATTRS_DEPREC_INIT_CTOR_PY(FlowEngine,PartialEngine,"An engine to solve flow problem in saturated granular media.\n\n.. note::\n\t Multi-threading seems to work fine for Cholesky decomposition, but it fails for the solve phase in which -j1 is the fastest, here we specify thread numbers independently using :yref:`FlowEngine::numFactorizeThreads` and :yref:`FlowEngine::numSolveThreads`. These multhreading settings are only impacting the behaviour of openblas library and are relatively independant of :yref:`FlowEngine::multithread`. However, the settings have to be globally consistent. For instance, :yref:`FlowEngine::multithread`=True with  multithread :yref:`FlowEngine::numSolveThreads`=:yref:`FlowEngine::numFactorizeThreads`=4 means that openblas will mobilize 8 processors at some point, if the system doesn't have so many procs. it will hurt performance.",
					((bool,isActivated,true,,"Activates Flow Engine"))
					((bool,first,true,,"Controls the initialization/update phases"))
					((double, fluidBulkModulus, 0.,,"Bulk modulus of fluid (inverse of compressibility) K=-dP*V/dV [Pa]. Flow is compressible if fluidBulkModulus > 0, else incompressible."))
					((Real, dt, 0,,"timestep [s]"))
// 					((bool,save_vtk,false,,"Enable/disable vtk files creation for visualization"))
					((bool,permeability_map,false,,"Enable/disable stocking of average permeability scalar in cell infos."))
					((bool, save_mgpost, false,,"Enable/disable mgpost file creation"))
					((bool, slice_pressures, false, ,"Enable/Disable slice pressure measurement"))
					((bool, velocity_profile, false, ,"Enable/Disable slice velocity measurement"))
					((bool, consolidation,false,,"Enable/Disable storing consolidation files"))
					((bool, slip_boundary, true,, "Controls friction condition on lateral walls"))
					((bool,WaveAction, false,, "Allow sinusoidal pressure condition to simulate ocean waves"))
					((double, Sinus_Amplitude, 0,, "Pressure value (amplitude) when sinusoidal pressure is applied"))
					((double, Sinus_Average, 0,,"Pressure value (average) when sinusoidal pressure is applied"))
					((bool, CachedForces, true,,"Des/Activate the cached forces calculation"))
					((bool, Debug, false,,"Activate debug messages"))
					((double, wall_thickness,0.001,,"Walls thickness"))
					((double,P_zero,0,,"Initial internal pressure for oedometer test"))
					((double,Tolerance,1e-06,,"Gauss-Seidel Tolerance"))
					((double,Relax,1.9,,"Gauss-Seidel relaxation"))
					((bool, Update_Triangulation, 0,,"If true the medium is retriangulated. Can be switched on to force retriangulation after some events (else it will be true periodicaly based on :yref:`FlowEngine::EpsVolPercent_RTRG` and :yref:`FlowEngine::PermuteInterval`"))
					((int,PermuteInterval,1000,,"Maximum number of timesteps between re-triangulation events. See also :yref:`FlowEngine::EpsVolPercent_RTRG`."))
					((double, eps_vol_max, 0,,"Maximal absolute volumetric strain computed at each iteration"))
					((double, EpsVolPercent_RTRG,0.01,,"Percentuage of cumulate eps_vol at which retriangulation of pore space is performed"))
					((double, porosity, 0,,"Porosity computed at each retriangulation"))
					((bool,compute_K,false,,"Activates permeability measure within a granular sample"))
					((bool,meanKStat,false,,"Local permeabilities' correction through an optimized threshold"))
					((bool,clampKValues,true,,"If true, clamp local permeabilities between min/max*globalK threshold. This clamping can avoid singular values in the permeability matrix and may reduce numerical errors in the solve phase. It will also hide junk values if they exist, or bias all values in very heterogeneous problems. So, use this with care."))
					((Real,minKdivKmean,0.0001,,"define the min K value (see :yref:`FlowEngine::clampKValues`)"))
					((Real,maxKdivKmean,100,,"define the max K value (see :yref:`FlowEngine::clampKValues`)"))
					((double,permeability_factor,0.0,,"permability multiplier"))
					((double,viscosity,1.0,,"viscosity of fluid"))
					((double,stiffness, 10000,,"stiffness modulus"))
					((Real,loadFactor,1.1,,"Load multiplicator for oedometer test"))
					((double, K, 0,, "Permeability of the sample"))
					((int, useSolver, 0,, "Solver to use 0=G-Seidel, 1=Taucs, 2-Pardiso, 3-CHOLMOD"))
// 					((std::string,key,"",,"A string appended at the output files, use it to name simulations."))
					((double, V_d, 0,,"darcy velocity of fluid in sample"))
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
					((Vector3r, topBoundaryVelocity, Vector3r::Zero(),, "velocity on top boundary, only change it using :yref:`FlowEngine::setBoundaryVel`"))
					((int, ignoredBody,-1,,"Id of a sphere to exclude from the triangulation.)"))
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
					((bool, areaR2Permeability, 1,,"Use corrected formula for permeabilities calculation in flowboundingsphere (areaR2permeability variable)"))
					((bool, viscousShear, false,,"Compute viscous shear terms as developped by Donia Marzougui"))
					((bool, shearLubrication, false,,"Compute shear lubrication force as developped by Brule"))
					((double, eps, 0.00001,,"minimum distance between particles"))
					((bool, pressureForce, true,,"Compute the pressure field and associated fluid forces. WARNING: turning off means fluid flow is not computed at all."))

					((bool, normalLubrication, false,,"Compute normal lubrication force as developped by Brule"))
					((bool, viscousNormalBodyStress, false,,"Compute normal viscous stress applied on each body"))
					((bool, viscousShearBodyStress, false,,"Compute shear viscous stress applied on each body"))
					((bool, multithread, false,,"Build triangulation and factorize in the background (multi-thread mode)"))
					#ifdef EIGENSPARSE_LIB
					((int, numSolveThreads, 1,,"number of openblas threads in the solve phase."))
					((int, numFactorizeThreads, 1,,"number of openblas threads in the factorization phase"))
					#endif
					((Real, allDeprecs, 0,,"transitory variable: point removed attributes to this so that older scripts won't crash."))
					,
					/*deprec*/
					((meanK_opt,clampKValues,"the name changed"))
					((meanK_correction,allDeprecs,"the name changed"))
					,,
					timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
					for (int i=0; i<6; ++i){normal[i]=Vector3r::Zero();}
					normal[wall_bottom].y()=normal[wall_left].x()=normal[wall_back].z()=1;
					normal[wall_top].y()=normal[wall_right].x()=normal[wall_front].z()=-1;					
					solver = shared_ptr<FlowSolver> (new FlowSolver);
					first=true;
					Update_Triangulation=false;
					eps_vol_max=Eps_Vol_Cumulative=retriangulationLastIter=0;
					ReTrg=1;
					backgroundCompleted=true;
					ellapsedIter=0;
					,
					.def("imposeFlux",&FlowEngine::_imposeFlux,(python::arg("pos"),python::arg("p")),"Impose incoming flux in boundary cell of location 'pos'.")
					.def("imposePressure",&FlowEngine::_imposePressure,(python::arg("pos"),python::arg("p")),"Impose pressure in cell of location 'pos'. The index of the condition is returned (for multiple imposed pressures at different points).")
					.def("setImposedPressure",&FlowEngine::_setImposedPressure,(python::arg("cond"),python::arg("p")),"Set pressure value at the point indexed 'cond'.")
					.def("clearImposedPressure",&FlowEngine::_clearImposedPressure,"Clear the list of points with pressure imposed.")
					.def("clearImposedFlux",&FlowEngine::_clearImposedFlux,"Clear the list of points with flux imposed.")
					.def("getCellFlux",&FlowEngine::_getCellFlux,(python::arg("cond")),"Get influx in cell associated to an imposed P (indexed using 'cond').")
					.def("getBoundaryFlux",&FlowEngine::_getBoundaryFlux,(python::arg("boundary")),"Get total flux through boundary defined by its body id.")
					.def("getConstrictions",&FlowEngine::_getConstrictions,(python::arg("all")=true),"Get the list of constriction radii (inscribed circle) for all finite facets (if all==True) or all facets not incident to a virtual bounding sphere (if all==False).  When all facets are returned, negative radii denote facet incident to one or more fictious spheres.")
					.def("getConstrictionsFull",&FlowEngine::_getConstrictionsFull,(python::arg("all")=true),"Get the list of constrictions (inscribed circle) for all finite facets (if all==True), or all facets not incident to a fictious bounding sphere (if all==False). When all facets are returned, negative radii denote facet incident to one or more fictious spheres. The constrictions are returned in the format {{cell1,cell2}{rad,nx,ny,nz}}")
					.def("saveVtk",&FlowEngine::saveVtk,"Save pressure field in vtk format.")
					.def("AvFlVelOnSph",&FlowEngine::AvFlVelOnSph,(python::arg("Id_sph")),"Compute a sphere-centered average fluid velocity")
					.def("fluidForce",&FlowEngine::_fluidForce,(python::arg("Id_sph")),"Return the fluid force on sphere Id_sph.")
					.def("shearLubForce",&FlowEngine::_shearLubForce,(python::arg("Id_sph")),"Return the shear lubrication force on sphere Id_sph.")
					.def("shearLubTorque",&FlowEngine::_shearLubTorque,(python::arg("Id_sph")),"Return the shear lubrication torque on sphere Id_sph.")
					.def("normalLubForce",&FlowEngine::_normalLubForce,(python::arg("Id_sph")),"Return the normal lubrication force on sphere Id_sph.")
					.def("bodyShearLubStress",&FlowEngine::_bodyShearLubStress,(python::arg("Id_sph")),"Return the shear lubrication stress on sphere Id_sph.")
					.def("bodyNormalLubStress",&FlowEngine::_bodyNormalLubStress,(python::arg("Id_sph")),"Return the normal lubrication stress on sphere Id_sph.")
					.def("setBoundaryVel",&FlowEngine::setBoundaryVel,(python::arg("vel")),"Change velocity on top boundary.")
					.def("PressureProfile",&FlowEngine::PressureProfile,(python::arg("wallUpY"),python::arg("wallDownY")),"Measure pore pressure in 6 equally-spaced points along the height of the sample")
					.def("MeasurePorePressure",&FlowEngine::MeasurePorePressure,(python::arg("pos")),"Measure pore pressure in position pos[0],pos[1],pos[2]")
					.def("MeasureAveragedPressure",&FlowEngine::MeasureAveragedPressure,(python::arg("posY")),"Measure slice-averaged pore pressure at height posY")
					.def("MeasureTotalAveragedPressure",&FlowEngine::MeasureTotalAveragedPressure,"Measure averaged pore pressure in the entire volume")

					.def("updateBCs",&FlowEngine::_updateBCs,"tells the engine to update it's boundary conditions before running (especially useful when changing boundary pressure - should not be needed for point-wise imposed pressure)")
					.def("emulateAction",&FlowEngine::emulateAction,"get scene and run action (may be used to manipulate engine outside the main loop).")
					.def("getCell",&FlowEngine::_getCell,(python::arg("pos")),"get id of the cell containing (X,Y,Z).")
					#ifdef LINSOLV
					.def("exportMatrix",&FlowEngine::_exportMatrix,(python::arg("filename")="matrix"),"Export system matrix to a file with all entries (even zeros will displayed).")
					.def("exportTriplets",&FlowEngine::_exportTriplets,(python::arg("filename")="triplets"),"Export system matrix to a file with only non-zero entries.")
					#endif
					)
		DECLARE_LOGGER;
};


template<class Solver>
unsigned int FlowEngine::imposePressure(Vector3r pos, Real p,Solver& flow)
{
	if (!flow) LOG_ERROR("no flow defined yet, run at least one iter");
	flow->imposedP.push_back( pair<CGT::Point,Real>(CGT::Point(pos[0],pos[1],pos[2]),p) );
	//force immediate update of boundary conditions
	Update_Triangulation=true;
	return flow->imposedP.size()-1;
}



REGISTER_SERIALIZABLE(FlowEngine);

#ifdef LINSOLV
#define _PeriFlowSolver CGT::PeriodicFlowLinSolv
#else
#define _PeriFlowSolver CGT::PeriodicFlow
#endif

class PeriodicFlowEngine : public FlowEngine
{
	public :
		public :
		typedef _PeriFlowSolver							FlowSolver;
		typedef PeriFlowTesselation						Tesselation;
		typedef FlowSolver::RTriangulation					RTriangulation;
		typedef FlowSolver::Finite_vertices_iterator                    	Finite_vertices_iterator;
		typedef FlowSolver::Finite_cells_iterator				Finite_cells_iterator;
		typedef FlowSolver::Cell_handle						Cell_handle;
		typedef RTriangulation::Finite_edges_iterator				Finite_edges_iterator;
		typedef RTriangulation::Vertex_handle					Vertex_handle;
		
		shared_ptr<FlowSolver> solver;
		shared_ptr<FlowSolver> backgroundSolver;
		
		void Triangulate (shared_ptr<FlowSolver>& flow);
// 		void AddBoundary ();
		void Build_Triangulation (Real pzero, shared_ptr<FlowSolver>& flow);
		void Initialize_volumes (shared_ptr<FlowSolver>&  flow);
		void UpdateVolumes (shared_ptr<FlowSolver>&  flow);
		Real Volume_cell (Cell_handle cell);

		Real Volume_cell_single_fictious (Cell_handle cell);
		inline void locateCell(Cell_handle baseCell, unsigned int& index, int& baseIndex, shared_ptr<FlowSolver>& flow, unsigned int count=0);
		Vector3r meanVelocity();

		virtual ~PeriodicFlowEngine();

		virtual void action();
		void backgroundAction();
		//Cache precomputed values for pressure shifts, based on current hSize and pGrad
		void preparePShifts();
		
		//(re)instanciation of templates and others, for python binding
		void saveVtk() {solver->saveVtk();}
		Vector3r 	_shearLubForce(unsigned int id_sph) {return shearLubForce(id_sph,solver);}
		Vector3r 	_shearLubTorque(unsigned int id_sph) {return shearLubTorque(id_sph,solver);}
		Vector3r 	_normalLubForce(unsigned int id_sph) {return normalLubForce(id_sph,solver);}
		Matrix3r 	_bodyShearLubStress(unsigned int id_sph) {return bodyShearLubStress(id_sph,solver);}
		Matrix3r 	_bodyNormalLubStress(unsigned int id_sph) {return bodyNormalLubStress(id_sph,solver);}

		Vector3r 	_fluidForce(unsigned int id_sph) {return fluidForce(id_sph, solver);}
		void 		_imposeFlux(Vector3r pos, Real flux) {return imposeFlux(pos,flux,*solver);}
		unsigned int 	_imposePressure(Vector3r pos, Real p) {return imposePressure(pos,p,this->solver);}
		Real 		_getBoundaryFlux(unsigned int boundary) {return getBoundaryFlux(boundary,solver);}
			
		void 		_updateBCs() {updateBCs(solver);}
		double 		MeasurePorePressure(Vector3r pos){return solver->MeasurePorePressure(pos[0], pos[1], pos[2]);}
		double 		MeasureTotalAveragedPressure(){return solver->MeasureTotalAveragedPressure();}
		void 		PressureProfile(double wallUpY, double wallDownY) {return solver->MeasurePressureProfile(wallUpY,wallDownY);}

		int		_getCell(Vector3r pos) {return getCell(pos[0],pos[1],pos[2],solver);}
		#ifdef LINSOLV
		void 		_exportMatrix(string filename) {exportMatrix(filename,solver);}
		void 		_exportTriplets(string filename) {exportTriplets(filename,solver);}
		#endif
		
// 		void 		_setImposedPressure(unsigned int cond, Real p) {setImposedPressure(cond,p,solver);}
// 		void 		_clearImposedPressure() {clearImposedPressure(solver);}
		Real 		_getCellFlux(unsigned int cond) {return getCellFlux(cond,solver);}
		python::list 	_getConstrictions(bool all) {return getConstrictions(all,solver);}
		python::list 	_getConstrictionsFull(bool all) {return getConstrictionsFull(all,solver);}

		//commodities
		void compTessVolumes() {
			solver->T[solver->currentTes].Compute();
			solver->T[solver->currentTes].ComputeVolumes();
		}
		Real getVolume (Body::id_t id) {
			if (solver->T[solver->currentTes].Max_id() <= 0) {emulateAction(); LOG_WARN("Not triangulated yet, emulating action");}
			if (solver->T[solver->currentTes].Volume(id) == -1) {compTessVolumes(); LOG_WARN("Computing all volumes now, as you did not request it explicitely.");}
			return (solver->T[solver->currentTes].Max_id() >= id) ? solver->T[solver->currentTes].Volume(id) : -1;}

		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(PeriodicFlowEngine,FlowEngine,"An engine to solve flow problem in saturated granular media",
			((Real,duplicateThreshold, 0.06,,"distance from cell borders that will triger periodic duplication in the triangulation |yupdate|"))
			((Vector3r, gradP, Vector3r::Zero(),,"Macroscopic pressure gradient"))
			,,
			wallTopId=wallBottomId=wallFrontId=wallBackId=wallLeftId=wallRightId=-1;
			solver = shared_ptr<FlowSolver> (new FlowSolver);
			Update_Triangulation=false;
			eps_vol_max=Eps_Vol_Cumulative=retriangulationLastIter=0;
			ReTrg=1;
			first=true;
			,
			.def("meanVelocity",&PeriodicFlowEngine::meanVelocity,"measure the mean velocity in the period")
			.def("fluidForce",&PeriodicFlowEngine::_fluidForce,(python::arg("Id_sph")),"Return the fluid force on sphere Id_sph.")
			.def("shearLubForce",&PeriodicFlowEngine::_shearLubForce,(python::arg("Id_sph")),"Return the shear lubrication force on sphere Id_sph.")
			.def("shearLubTorque",&PeriodicFlowEngine::_shearLubTorque,(python::arg("Id_sph")),"Return the shear lubrication torque on sphere Id_sph.")
			.def("normalLubForce",&PeriodicFlowEngine::_normalLubForce,(python::arg("Id_sph")),"Return the normal lubrication force on sphere Id_sph.")
			.def("bodyShearLubStress",&PeriodicFlowEngine::_bodyShearLubStress,(python::arg("Id_sph")),"Return the shear lubrication stress on sphere Id_sph.")
			.def("bodyNormalLubStress",&PeriodicFlowEngine::_bodyNormalLubStress,(python::arg("Id_sph")),"Return the normal lubrication stress on sphere Id_sph.")

// 			.def("imposeFlux",&FlowEngine::_imposeFlux,(python::arg("pos"),python::arg("p")),"Impose incoming flux in boundary cell of location 'pos'.")
			.def("saveVtk",&PeriodicFlowEngine::saveVtk,"Save pressure field in vtk format.")
			.def("imposePressure",&PeriodicFlowEngine::_imposePressure,(python::arg("pos"),python::arg("p")),"Impose pressure in cell of location 'pos'. The index of the condition is returned (for multiple imposed pressures at different points).")
			.def("getBoundaryFlux",&PeriodicFlowEngine::_getBoundaryFlux,(python::arg("boundary")),"Get total flux through boundary defined by its body id.")
			.def("MeasurePorePressure",&PeriodicFlowEngine::MeasurePorePressure,(python::arg("pos")),"Measure pore pressure in position pos[0],pos[1],pos[2]")
			.def("MeasureTotalAveragedPressure",&PeriodicFlowEngine::MeasureTotalAveragedPressure,"Measure averaged pore pressure in the entire volume") 
			.def("PressureProfile",&PeriodicFlowEngine::PressureProfile,(python::arg("wallUpY"),python::arg("wallDownY")),"Measure pore pressure in 6 equally-spaced points along the height of the sample")

			.def("updateBCs",&PeriodicFlowEngine::_updateBCs,"tells the engine to update it's boundary conditions before running (especially useful when changing boundary pressure - should not be needed for point-wise imposed pressure)")
			
			.def("getCell",&PeriodicFlowEngine::_getCell,python::arg("pos"),"get id of the cell containing 'pos'.")
			.def("getConstrictions",&PeriodicFlowEngine::_getConstrictions,(python::arg("all")=true),"Get the list of constriction radii (inscribed circle) for all finite facets (if all==True) or all facets not incident to a virtual bounding sphere (if all==False).  When all facets are returned, negative radii denote facet incident to one or more fictious spheres.")
			.def("getConstrictionsFull",&PeriodicFlowEngine::_getConstrictionsFull,(python::arg("all")=true),"Get the list of constrictions (inscribed circle) for all finite facets (if all==True), or all facets not incident to a fictious bounding sphere (if all==False). When all facets are returned, negative radii denote facet incident to one or more fictious spheres. The constrictions are returned in the format {{cell1,cell2}{rad,nx,ny,nz}}")
			#ifdef LINSOLV
			.def("exportMatrix",&PeriodicFlowEngine::_exportMatrix,(python::arg("filename")="matrix"),"Export system matrix to a file with all entries (even zeros will displayed).")
			.def("exportTriplets",&PeriodicFlowEngine::_exportTriplets,(python::arg("filename")="triplets"),"Export system matrix to a file with only non-zero entries.")
			#endif
			.def("compTessVolumes",&PeriodicFlowEngine::compTessVolumes,"Like TesselationWrapper::computeVolumes()")
			.def("volume",&PeriodicFlowEngine::getVolume,(python::arg("id")=0),"Returns the volume of Voronoi's cell of a sphere.")
		)
		DECLARE_LOGGER;


};

REGISTER_SERIALIZABLE(PeriodicFlowEngine);


// #endif
