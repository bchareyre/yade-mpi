#include "InelastCohFrictPhys.hpp"


void InelastCohFrictPhys::SetBreakingState()
{	
	cohesionBroken = true;
	normalAdhesion = 0;
	shearAdhesion = 0;
}

InelastCohFrictPhys::~InelastCohFrictPhys()
{
}
YADE_PLUGIN((InelastCohFrictPhys));