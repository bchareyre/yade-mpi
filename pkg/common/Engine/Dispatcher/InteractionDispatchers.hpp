// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/GlobalEngine.hpp>
#include<yade/pkg-common/InteractionGeometryDispatcher.hpp>
#include<yade/pkg-common/InteractionPhysicsDispatcher.hpp>
#include<yade/pkg-common/LawDispatcher.hpp>

class InteractionDispatchers;
shared_ptr<InteractionDispatchers> InteractionDispatchers_ctor_lists(const std::vector<shared_ptr<InteractionGeometryFunctor> >& gff, const std::vector<shared_ptr<InteractionPhysicsFunctor> >& pff, const std::vector<shared_ptr<LawFunctor> >& cff);

class InteractionDispatchers: public GlobalEngine {
	bool alreadyWarnedNoCollider;
	public:
		InteractionDispatchers();
		virtual void action(Scene*);
		shared_ptr<InteractionGeometryDispatcher> geomDispatcher;
		shared_ptr<InteractionPhysicsDispatcher> physDispatcher;
		shared_ptr<LawDispatcher> lawDispatcher;
		YADE_CLASS_BASE_ATTRS_PY(InteractionDispatchers,GlobalEngine,(geomDispatcher)(physDispatcher)(lawDispatcher),
			.def("__init__",python::make_constructor(InteractionDispatchers_ctor_lists))
			//.def_readonly("geomDispatcher",&InteractionDispatchers::geomDispatcher)
			//.def_readonly("physDispatcher",&InteractionDispatchers::physDispatcher)
			//.def_readonly("lawDispatcher",&InteractionDispatchers::lawDispatcher);
		);
		DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(InteractionDispatchers);

