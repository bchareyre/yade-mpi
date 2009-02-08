// 2009 © Václav Šmilauer <eudoxos@arcig.cz>
#pragma once
#include<yade/core/MetaEngine2D.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>
#include<yade/core/Interaction.hpp>
#include<yade/pkg-common/ConstitutiveLaw.hpp>

class ConstitutiveLawDispatcher:
	public MetaEngine2D <
		InteractionGeometry, // 1st base classe for dispatch
		InteractionPhysics,  // 2nd base classe for dispatch
		ConstitutiveLaw,     // functor base class
		void,                // return type
		TYPELIST_4(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*, MetaBody*),
		false                // autosymmetry
	>{
		public:
		virtual void action(MetaBody*);
		REGISTER_CLASS_AND_BASE(ConstitutiveLawDispatcher,MetaEngine2D);
};
REGISTER_SERIALIZABLE(ConstitutiveLawDispatcher);

