#include <yade/pkg-dem/CapillaryParameters.hpp>

CapillaryParameters::CapillaryParameters()
{
	meniscus = false;
	Vmeniscus = 0;
	CapillaryPressure = 0;
	Fcap = Vector3r(0,0,0);
	Delta1 = 0;
	Delta2 = 0;
	fusionNumber = 0;
	currentIndexes[0]=0;
	currentIndexes[1]=0;
	currentIndexes[2]=0;
	currentIndexes[3]=0;
}

CapillaryParameters::~CapillaryParameters()
{
}

// void CapillaryParameters::postProcessAttributes(bool)
// {
// 
// }


