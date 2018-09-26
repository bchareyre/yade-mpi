/*************************************************************************
*  Copyright (C) 2018 by Robert Caulk <rob.caulk@gmail.com>  		 *
*  Copyright (C) 2018 by Bruno Chareyre <bruno.chareyre@hmg.inpg.fr>     *
*									 *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
/* This engine is under active development. Experimental only */

#pragma once 
#include<core/PartialEngine.hpp>
#include<core/State.hpp>
#include<pkg/dem/ScGeom.hpp>
#include<pkg/common/Dispatching.hpp>

#ifdef FLOW_ENGINE
//#include<pkg/pfv/FlowEngine.hpp>
#include<lib/triangulation/Tesselation.h>
#include<lib/triangulation/FlowBoundingSphere.hpp>
#include "FlowEngine_FlowEngineT.hpp"
#include<pkg/dem/TesselationWrapper.hpp>
#endif

// This is how to turn a body thermal without data loss. Should be done in a loop by a single function, ofc.
// Yade [10]: s=sphere((0,0,1),100)
// Yade [11]: s.state.vel=(3,3,3)
// Yade [12]: thState = ThermalState()
// Yade [13]: thState.updateAttrs(s.state.dict())
// Yade [14]: s.state=thState
// Yade [15]: s.state.tmp
//  ->  [15]: 0.0
// Yade [16]: s.state.vel
//  ->  [16]: Vector3(3,3,3)

// Shorter yet strictly equivalent
// Yade [21]: s.state=ThermalState().__setstate__( s.state.__getstate__())



class ThermalState: public State {
	public:
		virtual ~ThermalState();
		// State is declared boost::noncopyable, so copy constructor seems nearly impossible. The solution is to update inherited attributes using python as show in preamble
// 		ThermalState& operator= (const State& source) : State(source) {};//FIXME Thermal.cpp:9:33: error: use of deleted function ‘State& State::operator=(const State&)’

		
		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ThermalState,State,"preliminary",
		/*attributes*/
		((Real,temp,0,,"temperature of the body"))
		((Real,U,0,,"internal energy of the body"))
		((Real,Cp,0,,"internal energy of the body"))
		((Real,k,0,,"thermal conductivity of the body"))
		,
		/* extra initializers */
		,
		/* ctor */ createIndex();
		,
		/* py */
	);
	REGISTER_CLASS_INDEX(ThermalState,State);
};

REGISTER_SERIALIZABLE(ThermalState);

typedef TemplateFlowEngine_FlowEngineT<FlowCellInfo_FlowEngineT,FlowVertexInfo_FlowEngineT> FlowEngineT;

class ThermalEngine : public PartialEngine
{
	public:
		typedef FlowEngineT::Tesselation					Tesselation;
		typedef FlowEngineT::RTriangulation					RTriangulation;
		typedef FlowEngineT::FiniteCellsIterator				FiniteCellsIterator;
		typedef FlowEngineT::CellHandle						CellHandle;
		typedef FlowEngineT::VertexHandle                    			VertexHandle;



	public:
		double fluidK;
		Scene* scene;
		bool setInternalEnergy; //initialize the internal energy of the particles

		virtual ~ThermalEngine();
		virtual void action();
		void makeThermalState();
		void initializeInternalEnergy();
		void computeSolidFluidFluxes();
		void computeNewTemperatures();
		void computeVertexSphericalArea(
	TemplateFlowEngine_FlowEngineT<FlowCellInfo_FlowEngineT,FlowVertexInfo_FlowEngineT>* flow);
		void computeFlux(CellHandle& cell, const shared_ptr<Body>& b, const double surfaceArea);
		void computeSolidSolidFluxes();

		YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(ThermalEngine,PartialEngine,"preliminary",
		/*attributes*/
		,
		/* extra initializers */
		,
		/* ctor */
		setInternalEnergy=true;
		,
		/* py */
	)
	DECLARE_LOGGER;

	
};
REGISTER_SERIALIZABLE(ThermalEngine);
//
	
