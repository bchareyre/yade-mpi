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
#include<yade/lib/triangulation/FlowBoundingSphere.hpp>
#include<yade/pkg/dem/TesselationWrapper.hpp>

class Flow;
class TesselationWrapper;
#ifdef LINSOLV
#define FlowSolver CGT::FlowBoundingSphereLinSolv
#else
#define FlowSolver CGT::FlowBoundingSphere
#endif



class FlowEngine : public PartialEngine
{
	private:
		shared_ptr<FlowSolver> flow;
		int retriangulationLastIter;
	public :
		enum {wall_left=0, wall_right, wall_bottom, wall_top, wall_back, wall_front};
		Vector3r normal [6];
		bool currentTes;
		int id_offset;
		double Eps_Vol_Cumulative;
		int ReTrg;
		void Triangulate ();
		void AddBoundary ();
		void Build_Triangulation (double P_zero );
		void Build_Triangulation ();
		void UpdateVolumes ();
		void Initialize_volumes ();
		Real Volume_cell_single_fictious (CGT::Cell_handle cell);
		Real Volume_cell_double_fictious (CGT::Cell_handle cell);
		Real Volume_cell_triple_fictious (CGT::Cell_handle cell);
		Real Volume_cell (CGT::Cell_handle cell);
		void Oedometer_Boundary_Conditions();
		void BoundaryConditions();
		void imposeFlux(Vector3r pos, Real flux);
		unsigned int imposePressure(Vector3r pos, Real p);
		void setImposedPressure(unsigned int cond, Real p);
		void clearImposedPressure();
		void Average_real_cell_velocity();
		void ApplyViscousForces();
		Real getFlux(unsigned int cond);
		void saveVtk() {flow->saveVtk();}
		vector<Real> AvFlVelOnSph(unsigned int id_sph) {return flow->Average_Fluid_Velocity_On_Sphere(id_sph);}
		python::list getConstrictions() {
			vector<Real> csd=flow->getConstrictions(); python::list pycsd;
			for (unsigned int k=0;k<csd.size();k++) pycsd.append(csd[k]); return pycsd;}
		Vector3r fluidForce(unsigned int id_sph) {const CGT::Vecteur& f=flow->T[flow->currentTes].vertex(id_sph)->info().forces; return Vector3r(f[0],f[1],f[2]);}
		Vector3r fluidShearForce(unsigned int id_sph) {return (flow->viscousShearForces.size()>id_sph)?flow->viscousShearForces[id_sph]:Vector3r::Zero();}
		void setBoundaryVel(Vector3r vel) {topBoundaryVelocity=vel; Update_Triangulation=true;}
		void PressureProfile(double wallUpY, double wallDownY) {return flow->Measure_Pore_Pressure(wallUpY,wallDownY);}

		virtual ~FlowEngine();

		virtual void action();

		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(FlowEngine,PartialEngine,"An engine to solve flow problem in saturated granular media",
					((bool,isActivated,true,,"Activates Flow Engine"))
					((bool,first,true,,"Controls the initialization/update phases"))
// 					((bool,save_vtk,false,,"Enable/disable vtk files creation for visualization"))
					((bool,permeability_map,false,,"Enable/disable stocking of average permeability scalar in cell infos."))
					((bool,save_mplot,false,,"Enable/disable mplot files creation"))
					((bool, save_mgpost, false,,"Enable/disable mgpost file creation"))
					((bool, slice_pressures, false, ,"Enable/Disable slice pressure measurement"))
					((bool, velocity_profile, false, ,"Enable/Disable slice velocity measurement"))
					((bool, consolidation,false,,"Enable/Disable storing consolidation files"))
					((bool, slip_boundary, true,, "Controls friction condition on lateral walls"))
					((bool, blocked_grains, false,, "Grains will/won't be moved by forces"))
					((bool,WaveAction, false,, "Allow sinusoidal pressure condition to simulate ocean waves"))
					((double, Sinus_Amplitude, 0,, "Pressure value (amplitude) when sinusoidal pressure is applied"))
					((double, Sinus_Average, 0,,"Pressure value (average) when sinusoidal pressure is applied"))
					((bool, CachedForces, true,,"Des/Activate the cached forces calculation"))
					((bool, Debug, false,,"Activate debug messages"))
					((double, wall_thickness,0.001,,"Walls thickness"))
					((double,P_zero,0,,"Initial internal pressure for oedometer test"))
					((double,Tolerance,1e-06,,"Gauss-Seidel Tolerance"))
					((double,Relax,1.9,,"Gauss-Seidel relaxation"))
					((bool, Update_Triangulation, 0,,"If true the medium is retriangulated"))
					((int,PermuteInterval,100000,,"Pore space re-triangulation period"))
					((double, eps_vol_max, 0,,"Maximal absolute volumetric strain computed at each iteration"))
					((double, EpsVolPercent_RTRG,0.01,,"Percentuage of cumulate eps_vol at which retriangulation of pore space is performed"))
					((double, porosity, 0,,"Porosity computed at each retriangulation"))
					((bool,compute_K,false,,"Activates permeability measure within a granular sample"))
					((bool,meanK_correction,true,,"Local permeabilities' correction through meanK threshold"))
					((bool,meanK_opt,false,,"Local permeabilities' correction through an optimized threshold"))
					((double,permeability_factor,0.0,,"permability multiplier"))
					((double,viscosity,1.0,,"viscosity of fluid"))
					((Real,loadFactor,1.1,,"Load multiplicator for oedometer test"))
					((double, K, 0,, "Permeability of the sample"))
					((double, MaxPressure, 0,, "Maximal value of water pressure within the sample - Case of consolidation test"))
					((double, currentStress, 0,, "Current value of axial stress"))
					((double, currentStrain, 0,, "Current value of axial strain"))
					((int, intervals, 30,, "Number of layers for computation average fluid pressure profiles to build consolidation curves"))
					((int, useSolver, 0,, "Solver to use 0=G-Seidel, 1=Taucs, 2-Pardiso"))
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
					((int, wallTopId,3,,"Id of top boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallBottomId,2,,"Id of bottom boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallFrontId,5,,"Id of front boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallBackId,4,,"Id of back boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallLeftId,0,,"Id of left boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((int, wallRightId,1,,"Id of right boundary (default value is ok if aabbWalls are appended BEFORE spheres.)"))
					((Vector3r, id_force, 0,, "Fluid force acting on sphere with id=flow.id_sphere"))
					((bool, BOTTOM_Boundary_MaxMin, 1,,"If true bounding sphere is added as function fo max/min sphere coord, if false as function of yade wall position"))
					((bool, TOP_Boundary_MaxMin, 1,,"If true bounding sphere is added as function fo max/min sphere coord, if false as function of yade wall position"))
					((bool, RIGHT_Boundary_MaxMin, 1,,"If true bounding sphere is added as function fo max/min sphere coord, if false as function of yade wall position"))
					((bool, LEFT_Boundary_MaxMin, 1,,"If true bounding sphere is added as function fo max/min sphere coord, if false as function of yade wall position"))
					((bool, FRONT_Boundary_MaxMin, 1,,"If true bounding sphere is added as function fo max/min sphere coord, if false as function of yade wall position"))
					((bool, BACK_Boundary_MaxMin, 1,,"If true bounding sphere is added as function fo max/min sphere coord, if false as function of yade wall position"))
					((bool, areaR2Permeability, 1,,"Use corrected formula for permeabilities calculation in flowboundingsphere (areaR2permeability variable)"))
					((bool, viscousShear, false,,"Compute viscous shear terms as developped by Donia Marzougui"))
					,,
					timingDeltas=shared_ptr<TimingDeltas>(new TimingDeltas);
					normal[wall_bottom].y()=1;
					normal[wall_top].y()=-1;
					normal[wall_left].x()=1;
					normal[wall_right].x()=-1;
					normal[wall_front].z()=-1;
					normal[wall_back].z()=1;
					,
					.def("imposeFlux",&FlowEngine::imposeFlux,(python::arg("pos"),python::arg("p")),"Impose incoming flux in boundary cell of location 'pos'.")
					.def("imposePressure",&FlowEngine::imposePressure,(python::arg("pos"),python::arg("p")),"Impose pressure in cell of location 'pos'. The index of the condition is returned (for multiple imposed pressures at different points).")
					.def("setImposedPressure",&FlowEngine::setImposedPressure,(python::arg("cond"),python::arg("p")),"Set pressure value at the point indexed 'cond'.")
					.def("clearImposedPressure",&FlowEngine::clearImposedPressure,"Clear the list of points with pressure imposed.")
					.def("getFlux",&FlowEngine::getFlux,(python::arg("cond")),"Get influx in cell associated to an imposed P (indexed using 'cond').")
					.def("getConstrictions",&FlowEngine::getConstrictions,"Get the list of constrictions (inscribed circle) for all finite facets.")
					.def("saveVtk",&FlowEngine::saveVtk,"Save pressure field in vtk format.")
					.def("AvFlVelOnSph",&FlowEngine::AvFlVelOnSph,(python::arg("Id_sph")),"Compute a sphere-centered average fluid velocity")
					.def("fluidForce",&FlowEngine::fluidForce,(python::arg("Id_sph")),"Return the fluid force on sphere Id_sph.")
					.def("fluidShearForce",&FlowEngine::fluidShearForce,(python::arg("Id_sph")),"Return the viscous shear force on sphere Id_sph.")
					.def("setBoundaryVel",&FlowEngine::setBoundaryVel,(python::arg("vel")),"Change velocity on top boundary.")
					.def("PressureProfile",&FlowEngine::PressureProfile,"Measure pore pressure in 6 equally-spaced points along the height of the sample")
					)
		DECLARE_LOGGER;
};

REGISTER_SERIALIZABLE(FlowEngine);

// #endif
