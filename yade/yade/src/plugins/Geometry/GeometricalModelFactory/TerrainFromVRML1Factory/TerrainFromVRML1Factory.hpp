#ifndef __TERRAINFROMWRL1FILEFACTORY_H__
#define __TERRAINFROMWRL1FILEFACTORY_H__

#include "BodyGeometricalModelFactoryFunctor.hpp"
#include <boost/shared_ptr.hpp>

class TerrainFromVRML1Factory : public BodyGeometricalModelFactoryFunctor
{
	// construction
	public : TerrainFromVRML1Factory ();
	public : ~TerrainFromVRML1Factory ();

	public : boost::shared_ptr<BodyGeometricalModel> createGeometricalModel();

};

REGISTER_FACTORABLE(TerrainFromVRML1Factory);

#endif // __TERRAINFROMWRL1FILEFACTORY_H__
