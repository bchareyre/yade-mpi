#ifndef __TERRAINFROMWRL1FILEFACTORY_H__
#define __TERRAINFROMWRL1FILEFACTORY_H__

#include "GeometricalModelFactory.hpp"

class TerrainFromVRML1Factory : public GeometricalModelFactory
{	
	// construction
	public : TerrainFromVRML1Factory ();
	public : ~TerrainFromVRML1Factory ();
	
	public : shared_ptr<GeometricalModel> createGeometricalModel();
	
	public : void processAttributes();
	public : void registerAttributes();
	REGISTER_CLASS_NAME(TerrainFromVRML1Factory);

};

REGISTER_CLASS(TerrainFromVRML1Factory,false);

#endif // __TERRAINFROMWRL1FILEFACTORY_H__
