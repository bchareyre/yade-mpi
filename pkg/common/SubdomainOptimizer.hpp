// © 2010 Václav Šmilauer <eu@doxos.eu>
#pragma once

#ifdef YADE_SUBDOMAINS

#include<yade/core/Body.hpp>
#include<yade/core/BodyContainer.hpp>
#include<yade/pkg/common/PeriodicEngines.hpp>

class SubdomainOptimizer: public PeriodicEngine{
	struct SplitPlane{
		int ax;   // axis number
		Real lim; // limit coordinate on axis ax 
		int add;  // number of subdomain to add, if coordinate is above lim
		int aboveSplit; // number of particles above the split, used to compute equilibrium
		Real concentration;
		int dist; // previous distance from the equilibirum (in number of particles)
		int axCount; // how many planes are on this axis, to know which proportion of particles to take
		int axNth; // at which position we are on this axis
		SplitPlane(int _ax, Real _lim, int _add, Real _concentration, Real _axCount, Real _axNth): ax(_ax), lim(_lim), add(_add), aboveSplit(-1), concentration(_concentration), dist(0), axCount(_axCount), axNth(_axNth) {}
	};
	string div; // string describing how to divide; initially empty, set according to number of subdomains
	vector<SplitPlane> splits;
	Vector3r mn,mx; // corners of the scene's bounding box
	int nParticles;
	void updateSceneBbox();
	void initializeSplitPlanes();
	void adjustSplitPlanes();
public:
	virtual void action();
	YADE_CLASS_BASE_DOC_ATTRS_CTOR(SubdomainOptimizer,PeriodicEngine,"Assigns :yref:`bodies <Omega.bodies>` to spatially-defined subdomains, which increase performance with OpenMP builds -- each OpenMP thread is responsible for processing one subdomain (if the respective loop is parallelized). Using subdomains is not mandatory, though without them, OpenMP-enabled builds are not parallelized.\n\nIn the first run, subdomains are created by finding planes splitting the simulation domain the desired number of subdomains (2…8). Later, due to particle motion, particles within one subdomain might not be spatially close, and subdomains assignments should be renewed; the interval at which this engine should be run terefore depends on the nature of simulation.\n\n.. note:: Constructor changes default values of :yref:`initRun <PeriodicEngine::initRun>` to ``True`` and :yref:`iterPeriod <PeriodicEngine.iterPeriod>` to 500, to make it suitable for running without setting further arguments.",
		((bool,colorize,true,,"Change :yref:`colors <Shape.color>` of particles to show subdomain assignment visually."))
		((string,axesOrder,"",,"Order of axes that determine intial splitting planes orientation; if not set, they are determined from total extents of the simulation (from the longest to the shortest one). Must be combination of *x*, *y*, *z*."))
		// ((string,div,"",,"Initial division string; "))
	, /*ctor*/
		initRun=true; iterPeriod=500;
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(SubdomainOptimizer);

#endif /* YADE_SUBDOMAINS */
