#include "TerrainFromVRML1Factory.hpp"
#include "Terrain.hpp"

TerrainFromVRML1Factory::TerrainFromVRML1Factory() : BodyGeometricalModelFactoryFunctor()
{

}

TerrainFromVRML1Factory::~TerrainFromVRML1Factory()
{

}

boost::shared_ptr<BodyGeometricalModel> TerrainFromVRML1Factory::createGeometricalModel()
{
	return boost::shared_ptr<BodyGeometricalModel>(new Terrain);
}

