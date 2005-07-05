#include "InteractionSolver.hpp"

void InteractionSolver::action(Body* b)
{
	this->calculateForces(b);
}
