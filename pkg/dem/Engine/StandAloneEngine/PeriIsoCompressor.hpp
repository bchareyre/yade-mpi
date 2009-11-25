// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once

#include<yade/core/StandAloneEngine.hpp>

/* Simple engine for compressing random cloud of spheres until
it reaches given average stress and then goes back to given residual stress
*/
class PeriIsoCompressor: public StandAloneEngine{
	Real avgStiffness; Real maxDisplPerStep; Vector3r sumForces, sigma; 
	public:
	//! Stresses that should be reached, one after another
	vector<Real> stresses;
	//! Characteristic length, should be something like mean particle diameter (default -1=invalid value))
	Real charLen;
	//! Maximum body span in terms of bbox, to prevent periodic cell getting too small; is computed automatically at the beginning
	Real maxSpan;
	//! if actual unbalanced force is smaller than this number, the packing is considered stable (default 1e-4)
	Real maxUnbalanced, currUnbalanced;
	//! how often to recompute average stress, stiffness and unbalanced force (default 100)
	int globalUpdateInt;
	//! Where are we at in the process
	size_t state;
	//! python command to be run when reaching the last specified stress
	string doneHook;
	//! whether to exactly keep proportions of the cell
	bool keepProportions;
	void action(MetaBody*);
	PeriIsoCompressor(): avgStiffness(-1), maxDisplPerStep(-1), sumForces(Vector3r::ZERO), sigma(Vector3r::ZERO), charLen(-1), maxSpan(-1), maxUnbalanced(1e-4), currUnbalanced(-1), globalUpdateInt(20), state(0), keepProportions(true){}
	REGISTER_ATTRIBUTES(StandAloneEngine,(stresses)(charLen)(maxUnbalanced)(globalUpdateInt)(state)(doneHook)(maxSpan)(keepProportions));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(PeriIsoCompressor,StandAloneEngine);
};
REGISTER_SERIALIZABLE(PeriIsoCompressor);

