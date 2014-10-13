 
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
//when you want it compiled, you can pass -DDFNFLOW to cmake, or just uncomment the following line
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
	double saturation;//the saturation of single pore (will be used in imbibition)
	bool isImbibition;//Flag for marking pore unit which contains NW-W interface (used by Thomas)
	double trapCapP;//for calculating the pressure of trapped pore, cell->info().p() = pressureNW- trapCapP. OR cell->info().p() = pressureW + trapCapP
	std::vector<double> poreThroatRadius;
	double poreBodyRadius;
	double poreBodyVolume;
	int windowsID;//a temp cell info for experiment comparison(used by chao)
	double solidLine [4][4];//the length of intersecting line between sphere and facet. [i][j] is for sphere facet "i" and sphere facetVertices[i][j]. Last component for 1/sumLines in the facet(used by chao).
	
	TwoPhaseCellInfo (void)
	{
		isWRes = true; isNWRes = false; isTrapW = false; isTrapNW = false;
		saturation = 1.0;
		isImbibition = false;
		trapCapP = 0;
		poreThroatRadius.resize(4, 0);
		poreBodyRadius = 0;
		poreBodyVolume = 0;
		windowsID = 0;
		for (int k=0; k<4;k++) for (int l=0; l<3;l++) solidLine[k][l]=0;
	}
	
};

class TwoPhaseVertexInfo : public FlowVertexInfo_TwoPhaseFlowEngineT {
	public:
	//same here if needed
};

typedef TemplateFlowEngine_TwoPhaseFlowEngineT<TwoPhaseCellInfo,TwoPhaseVertexInfo> TwoPhaseFlowEngineT;
REGISTER_SERIALIZABLE(TwoPhaseFlowEngineT);

class TwoPhaseFlowEngine : public TwoPhaseFlowEngineT
{
		double totalCellVolume;
	public :
	//We can overload every functions of the base engine to make it behave differently
	//if we overload action() like this, this engine is doing nothing in a standard timestep, it can still have useful functions
	virtual void action() {};
	
	//If a new function is specific to the derived engine, put it here, else go to the base TemplateFlowEngine
	//if it is useful for everyone
	void fancyFunction(Real what);
	void initializeCellIndex();
	void computePoreBodyVolume();	

	YADE_CLASS_BASE_DOC_ATTRS_INIT_CTOR_PY(TwoPhaseFlowEngine,TwoPhaseFlowEngineT,"documentation here",
	((double,surfaceTension,0.0728,,"Water Surface Tension in contact with air at 20 Degrees Celsius is: 0.0728(N/m)"))
	((bool,initialWetting,true,,"Initial wetting saturated (=true) or non-wetting saturated (=false)"))
	((bool, isPhaseTrapped,true,,"If True, both phases can be entrapped by the other, which would correspond to snap-off. If false, both phases are always connected to their reservoirs, thus no snap-off."))
	
	,/*TwoPhaseFlowEngineT()*/,
	,
	.def("fancyFunction",&TwoPhaseFlowEngine::fancyFunction,(boost::python::arg("what")=0),"test function")
	)
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(TwoPhaseFlowEngine);

#endif //TwoPhaseFLOW
 
