#include "ConstitutiveLaw.hpp"

void ConstitutiveLaw::action(Body* b)
{
	this->respondToInteractions(b);
}
