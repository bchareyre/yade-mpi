#include "TerrainFromVRML1Factory.hpp"
#include "Terrain.hpp"

TerrainFromVRML1Factory::TerrainFromVRML1Factory() : GeometricalModelFactory()
{

}

TerrainFromVRML1Factory::~TerrainFromVRML1Factory()
{

}

boost::shared_ptr<GeometricalModel> TerrainFromVRML1Factory::createGeometricalModel()
{
	return boost::shared_ptr<GeometricalModel>(new Terrain);
}

