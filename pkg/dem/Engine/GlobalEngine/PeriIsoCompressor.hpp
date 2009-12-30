// 2009 © Václav Šmilauer <eudoxos@arcig.cz> 

#pragma once

#include<yade/core/GlobalEngine.hpp>

/* Simple engine for compressing random cloud of spheres until
it reaches given average stress and then goes back to given residual stress
*/
class PeriIsoCompressor: public GlobalEngine{
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
	REGISTER_ATTRIBUTES(GlobalEngine,(stresses)(charLen)(maxUnbalanced)(globalUpdateInt)(state)(doneHook)(maxSpan)(keepProportions));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(PeriIsoCompressor,GlobalEngine);
};
REGISTER_SERIALIZABLE(PeriIsoCompressor);
/* Engine for independently controlling stress or strain in periodic simulations.

strainStress contains absolute values for the controlled quantity, and stressMask determines
meaning of those values (0 for strain, 1 for stress): e.g. ( 1<<0 | 1<<2 ) = 1 | 4 = 5 means that
strainStress[0] and strainStress[2] are stress values, and strainStress[1] is strain.

See scripts/test/periodic-triax.py for a simple example.

*/
class PeriTriaxController: public GlobalEngine{
	public:
	//! For broken constitutive laws, normalForce and shearForce on interactions are in the reverse sense
	//! see https://bugs.launchpad.net/yade/+bug/493102
	bool reversedForces;
	//! Desired stress or strain values (depending on stressMask)
	Vector3r goal;
	//! mask determining strain/stress (0/1) meaning for goal components
	int stressMask;
	//! Maximum strain rate of the periodic cell
	Vector3r maxStrainRate;
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
	//! maximum body dimension (set automatically)
	Vector3r maxBodySpan;


	//! average stresses, updated at every step (only every globUpdate steps recomputed from interactions)
	Vector3r stress;
	//! cell strain, updated at every step
	Vector3r strain;
	//! cell strain rate, updated at every step
	Vector3r strainRate;
	//! average stiffness, updated at every step (only every globUpdate steps recomputed from interactions)
	Vector3r stiff;
	//! current unbalanced force (updated every globUpdate)
	Real currUnbalanced;
	//! previous cell grow
	Vector3r prevGrow;

	void action(Scene*);
	void strainStressStiffUpdate();
	PeriTriaxController(): reversedForces(false),goal(Vector3r::ZERO),stressMask(0),maxStrainRate(Vector3r(1,1,1)),maxUnbalanced(1e-4),absStressTol(1e3),relStressTol(3e-5),growDamping(.25),globUpdate(5),maxBodySpan(Vector3r(-1,-1,-1)),stress(Vector3r::ZERO),strain(Vector3r::ZERO),strainRate(Vector3r::ZERO),stiff(Vector3r::ZERO),currUnbalanced(-1),prevGrow(Vector3r::ZERO){}
	REGISTER_ATTRIBUTES(GlobalEngine,(reversedForces)(goal)(stressMask)(maxStrainRate)(maxUnbalanced)(absStressTol)(relStressTol)(growDamping)(globUpdate)(doneHook)(stress)(strain)(strainRate)(stiff));
	DECLARE_LOGGER;
	REGISTER_CLASS_AND_BASE(PeriTriaxController,GlobalEngine);
};
REGISTER_SERIALIZABLE(PeriTriaxController);

//TODO :(1) isotropic compression with mean stress/strain (preserve aspect ration like in other engines)
//	(2) check vs. maxStrainRate? Not sure it is needed in that case
class PeriController: public GlobalEngine{
	public:
	//! For broken constitutive laws, normalForce and shearForce on interactions are in the reverse sense
	//! see https://bugs.launchpad.net/yade/+bug/493102
		bool reversedForces;
	//! Desired stress or strain values (depending on stressMask)
		Vector3r goal;
	//! mask determining strain/stress (0/1) meaning for goal components
		int stressMask;
	//! Maximum strain rate of the periodic cell
		Vector3r maxStrainRate;
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
	//! maximum body dimension (set automatically)
		Vector3r maxBodySpan;


	//! average stresses, updated at every step (only every globUpdate steps recomputed from interactions)
		Vector3r stress;
	//! cell strain, updated at every step
		Vector3r strain;
	//! cell strain rate, updated at every step
		Vector3r strainRate;
	//! average stiffness, updated at every step (only every globUpdate steps recomputed from interactions)
		Vector3r stiff;
	//! current unbalanced force (updated every globUpdate)
		Real currUnbalanced;
	//! previous cell grow
		Vector3r prevGrow;

		void action(Scene*);
		Matrix3r stressStrainUpdate();
		PeriController(): reversedForces(false),goal(Vector3r::ZERO),stressMask(0),maxStrainRate(Vector3r(1,1,1)),maxUnbalanced(1e-4),absStressTol(1e3),relStressTol(3e-5),growDamping(.5),globUpdate(5),maxBodySpan(Vector3r(-1,-1,-1)),stress(Vector3r::ZERO),strain(Vector3r::ZERO),strainRate(Vector3r::ZERO),stiff(Vector3r::ZERO),currUnbalanced(-1),prevGrow(Vector3r::ZERO){}
		REGISTER_ATTRIBUTES(GlobalEngine,(reversedForces)(goal)(stressMask)(maxStrainRate)(maxUnbalanced)(absStressTol)(relStressTol)(growDamping)(globUpdate)(doneHook)(stress)(strain)(strainRate)(stiff));
		DECLARE_LOGGER;
		REGISTER_CLASS_AND_BASE(PeriController,GlobalEngine);
};
REGISTER_SERIALIZABLE(PeriController);


