#ifndef __TERRAINFROMWRL1FILEFACTORY_H__
#define __TERRAINFROMWRL1FILEFACTORY_H__

#include "GeometricalModelFunctor.hpp"
#include <boost/shared_ptr.hpp>

class VRML2TerrainFunctor : public GeometricalModelFunctor
{

	public : void go(int);

};

REGISTER_FACTORABLE(VRML2TerrainFunctor);

#endif // __TERRAINFROMWRL1FILEFACTORY_H__
