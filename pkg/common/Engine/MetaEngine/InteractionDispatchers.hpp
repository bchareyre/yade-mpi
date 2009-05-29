// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/StandAloneEngine.hpp>
#include<yade/pkg-common/InteractionGeometryMetaEngine.hpp>
#include<yade/pkg-common/InteractionPhysicsMetaEngine.hpp>
#include<yade/pkg-common/ConstitutiveLawDispatcher.hpp>

class InteractionDispatchers: public StandAloneEngine {
	bool alreadyWarnedNoCollider;
	public:
		InteractionDispatchers();
		virtual void action(MetaBody*);
		shared_ptr<InteractionGeometryMetaEngine> geomDispatcher;
		shared_ptr<InteractionPhysicsMetaEngine> physDispatcher;
		shared_ptr<ConstitutiveLawDispatcher> constLawDispatcher;
		REGISTER_CLASS_AND_BASE(InteractionDispatchers,StandAloneEngine);
		REGISTER_ATTRIBUTES(StandAloneEngine,
			(geomDispatcher)
			(physDispatcher)
			(constLawDispatcher)
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InteractionDispatchers);
