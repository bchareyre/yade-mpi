#include "GeometricalModel.hpp"

GeometricalModel::GeometricalModel () : Serializable()
{
}

GeometricalModel::~GeometricalModel ()
{

}

void GeometricalModel::postProcessAttributes(bool)
{
}

void GeometricalModel::registerAttributes()
{
	REGISTER_ATTRIBUTE(diffuseColor);
	REGISTER_ATTRIBUTE(wire);
	REGISTER_ATTRIBUTE(visible);
}

