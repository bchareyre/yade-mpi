#include "VRML2TerrainFunctor.hpp"
#include "Terrain.hpp"

VRML2TerrainFunctor::VRML2TerrainFunctor() : GeometricalModelFunctor()
{

}

VRML2TerrainFunctor::~VRML2TerrainFunctor()
{

}

boost::shared_ptr<GeometricalModel> VRML2TerrainFunctor::createGeometricalModel()
{
	return boost::shared_ptr<GeometricalModel>(new Terrain);
}

