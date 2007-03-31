/*************************************************************************
*  Copyright (C) 2006 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef GLDRAW_INTERACTION_PHYSICS_FUNCTOR_HPP
#define GLDRAW_INTERACTION_PHYSICS_FUNCTOR_HPP

#include<yade/lib-multimethods/FunctorWrapper.hpp>
#include<yade/core/InteractionPhysics.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/EngineUnit1D.hpp>


#define RENDERS(name) 							\
	public : virtual string renders() const { return #name; };

class GLDrawInteractionPhysicsFunctor
	: public EngineUnit1D
	<
		void ,
		TYPELIST_5(const shared_ptr<InteractionPhysics>&, const shared_ptr<Interaction>&, const shared_ptr<Body>&, const shared_ptr<Body>&, bool) 
	>
{
	public : 
		virtual ~GLDrawInteractionPhysicsFunctor() {};
		virtual string renders() const { std::cerr << "unregistered gldraw class.\n"; throw; }; // FIXME - replace that with a nice exception
	
	REGISTER_CLASS_NAME(GLDrawInteractionPhysicsFunctor);
	REGISTER_BASE_CLASS_NAME(EngineUnit1D);
};

REGISTER_SERIALIZABLE(GLDrawInteractionPhysicsFunctor,false);

#endif //  GLDRAWGEOMETRICALMODELFUNCTOR_HPP


