#include "GeometricalModel.hpp"


void GeometricalModel::registerAttributes()
{
	REGISTER_ATTRIBUTE(diffuseColor);
	REGISTER_ATTRIBUTE(wire);
	REGISTER_ATTRIBUTE(visible);
	REGISTER_ATTRIBUTE(shadowCaster);
}

