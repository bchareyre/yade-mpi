#include "TerrainFromVRML1Factory.hpp"
#include "Terrain.hpp"

TerrainFromVRML1Factory::TerrainFromVRML1Factory() : GeometricalModelFactory()
{

}
	
TerrainFromVRML1Factory::~TerrainFromVRML1Factory()
{

}
void TerrainFromVRML1Factory::processAttributes()
{

}

void TerrainFromVRML1Factory::registerAttributes()
{

}

shared_ptr<GeometricalModel> TerrainFromVRML1Factory::createGeometricalModel()
{
	return shared_ptr<GeometricalModel>(new Terrain);
}

