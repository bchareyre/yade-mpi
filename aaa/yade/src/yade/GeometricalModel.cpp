#include "GeometricalModel.hpp"

GeometricalModel::GeometricalModel () : Serializable()
{
}

GeometricalModel::~GeometricalModel ()
{

}

void GeometricalModel::registerAttributes()
{
	REGISTER_ATTRIBUTE(diffuseColor);
	REGISTER_ATTRIBUTE(wire);
	REGISTER_ATTRIBUTE(visible);
}

void GeometricalModel::processAttributes()
{
	REGISTER_ATTRIBUTE(diffuseColor);
	REGISTER_ATTRIBUTE(wire);
	REGISTER_ATTRIBUTE(visible);
}
