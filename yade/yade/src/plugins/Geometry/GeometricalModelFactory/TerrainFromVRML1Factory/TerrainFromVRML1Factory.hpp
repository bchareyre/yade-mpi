#ifndef __TERRAINFROMWRL1FILEFACTORY_H__
#define __TERRAINFROMWRL1FILEFACTORY_H__

#include "GeometricalModelFactoryFunctor.hpp"
#include <boost/shared_ptr.hpp>

class TerrainFromVRML1Factory : public GeometricalModelFactoryFunctor
{
	// construction
	public : TerrainFromVRML1Factory ();
	public : ~TerrainFromVRML1Factory ();

	public : boost::shared_ptr<GeometricalModel> createGeometricalModel();

};

REGISTER_FACTORABLE(TerrainFromVRML1Factory);

#endif // __TERRAINFROMWRL1FILEFACTORY_H__
