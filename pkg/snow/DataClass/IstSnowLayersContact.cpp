#include<yade/pkg-snow/IstSnowLayersContact.hpp>

IstSnowLayersContact::IstSnowLayersContact()
{
	createIndex();
}

IstSnowLayersContact::~IstSnowLayersContact()
{}
		
void IstSnowLayersContact::registerAttributes()
{
	SpheresContactGeometry::registerAttributes();
}

