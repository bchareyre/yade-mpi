#ifndef __TERRAINFROMWRL1FILEFACTORY_H__
#define __TERRAINFROMWRL1FILEFACTORY_H__

#include "GeometricalModelFactory.hpp"
#include <boost/shared_ptr.hpp>

class TerrainFromVRML1Factory : public GeometricalModelFactory
{
	// construction
	public : TerrainFromVRML1Factory ();
	public : ~TerrainFromVRML1Factory ();

	public : boost::shared_ptr<BodyGeometricalModel> createGeometricalModel();

};

REGISTER_FACTORABLE(TerrainFromVRML1Factory);

#endif // __TERRAINFROMWRL1FILEFACTORY_H__
