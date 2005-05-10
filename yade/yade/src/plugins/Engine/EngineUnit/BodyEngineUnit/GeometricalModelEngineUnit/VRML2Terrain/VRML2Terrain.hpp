#ifndef __TERRAINFROMWRL1FILEFACTORY_H__
#define __TERRAINFROMWRL1FILEFACTORY_H__

#include <yade-common/GeometricalModelEngineUnit.hpp>

class VRML2Terrain : public GeometricalModelEngineUnit
{
	REGISTER_CLASS_NAME(VRML2Terrain);
};

REGISTER_SERIALIZABLE(VRML2Terrain,false);

#endif // __TERRAINFROMWRL1FILEFACTORY_H__
