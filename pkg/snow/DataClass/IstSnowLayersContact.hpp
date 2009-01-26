// 2009 Janek Kozicki <cosurgi@berlios.de>

#pragma once

#include<yade/core/InteractionGeometry.hpp>
#include<yade/pkg-dem/SpheresContactGeometry.hpp>

class IstSnowLayersContact : public SpheresContactGeometry
{
	public:
		IstSnowLayersContact();
		virtual ~IstSnowLayersContact();

	protected :
		virtual void registerAttributes();

	REGISTER_CLASS_NAME(IstSnowLayersContact);
	REGISTER_BASE_CLASS_NAME(SpheresContactGeometry);
	REGISTER_CLASS_INDEX(IstSnowLayersContact,SpheresContactGeometry);
};

REGISTER_SERIALIZABLE(IstSnowLayersContact);

