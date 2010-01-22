// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/Dispatcher.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/LawFunctor.hpp>

class LawDispatcher:
	public Dispatcher2D <
		InteractionGeometry, // 1st base classe for dispatch
		InteractionPhysics,  // 2nd base classe for dispatch
		LawFunctor,     // functor base class
		void,                // return type
		TYPELIST_4(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*, Scene*),
		false                // autosymmetry
	>{
		public:
		virtual void action(Scene*);
	YADE_CLASS_BASE_DOC_ATTRS_PY(LawDispatcher,Dispatcher,"Dispatcher for applying constitutive laws on interaction.",/* no attrs */,
		YADE_PY_DISPATCHER(LawDispatcher)
	);
	DECLARE_LOGGER;
};
REGISTER_SERIALIZABLE(LawDispatcher);

