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
	void action(Scene*);
	PeriIsoCompressor(): avgStiffness(-1), maxDisplPerStep(-1), sumForces(Vector3r::ZERO), sigma(Vector3r::ZERO), charLen(-1), maxSpan(-1), maxUnbalanced(1e-4), currUnbalanced(-1), globalUpdateInt(20), state(0), keepProportions(true){}
	REGISTER_ATTRIBUTES(StandAloneEngine,(stresses)(charLen)(maxUnbalanced)(globalUpdateInt)(state)(doneHook)(maxSpan)(keepProportions));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(PeriIsoCompressor,StandAloneEngine);
};
REGISTER_SERIALIZABLE(PeriIsoCompressor);
/* Engine for independently controlling stress or strain in periodic simulations.

strainStress contains absolute values for the controlled quantity, and stressMask determines
meaning of those values (0 for strain, 1 for stress): e.g. ( 1<<0 | 1<<2 ) = 1 | 4 = 5 means that
strainStress[0] and strainStress[2] are stress values, and strainStress[1] is strain.
*/
class PeriTriaxController: public StandAloneEngine{
	public:
	//! Desired stress or strain values (depending on stressMask)
	Vector3r goal;
	//! mask determining strain/stress (0/1) meaning for goal components
	int stressMask;
	//! Maximum strain rate of the periodic cell
	Real maxStrainRate;
	//! maximum unbalanced force (defaults to 1e-4)
	Real maxUnbalanced;
	//! Absolute stress tolerance (1e3)
	Real absStressTol;
	//! Relative stress tolerance (3e-5)
	Real relStressTol;
	//! Damping of cell resizing (0=perfect control, 1=no control at all); see also TriaxialStressController::wallDamping.
	Real growDamping;
	//! how often to recompute average stress, stiffness and unbalaced force (defaults to 100)
	int globUpdate;
	//! python command to be run when the desired state is reached
	string doneHook;
	//! reference cell size (set automatically; if refSize[0]<0 (initial default), the current size is referenced)
	Vector3r refSize;
	//! maximum body dimension (set automatically)
	Vector3r maxBodySpan;


	//! average stresses, updated at every step (only every globUpdate steps recomputed from interactions)
	Vector3r stress;
	//! cell strain, updated at every step
	Vector3r strain;
	//! average stiffness, updated at every step (only every globUpdate steps recomputed from interactions)
	Vector3r stiff;
	//! current unbalanced force (updated every globUpdate)
	Real currUnbalanced;
	//! previous cell grow
	Vector3r prevGrow;

	void action(Scene*);
	void strainStressStiffUpdate();
	PeriTriaxController(): goal(Vector3r::ZERO),stressMask(0),maxStrainRate(1.),maxUnbalanced(1e-4),absStressTol(1e3),relStressTol(3e-5),growDamping(.25),globUpdate(100),refSize(Vector3r(-1,-1,-1)),maxBodySpan(Vector3r(-1,-1,-1)),stress(Vector3r::ZERO),strain(Vector3r::ZERO),stiff(Vector3r::ZERO),currUnbalanced(-1),prevGrow(Vector3r::ZERO){}
	REGISTER_ATTRIBUTES(StandAloneEngine,(goal)(stressMask)(maxStrainRate)(maxUnbalanced)(absStressTol)(relStressTol)(growDamping)(globUpdate)(doneHook)(refSize)(stress)(strain)(stiff));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(PeriTriaxController,StandAloneEngine);
};
REGISTER_SERIALIZABLE(PeriTriaxController);
