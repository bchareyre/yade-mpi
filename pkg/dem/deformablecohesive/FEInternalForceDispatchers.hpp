/*************************************************************************
*  Copyright (C) 2013 by Burak ER  burak.er@btu.edu.tr                   *
*									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <core/Shape.hpp>
#include <lib/base/Math.hpp>
#include <core/Interaction.hpp>
#include <core/Scene.hpp>
#include <core/State.hpp>
#include <core/Shape.hpp>
#include <core/IGeom.hpp>
#include <core/IPhys.hpp>
#include <core/Functor.hpp>
#include <core/Dispatcher.hpp>
#include <pkg/common/Aabb.hpp>
#include <pkg/dem/deformablecohesive/DeformableElement.hpp>


/*Functor of Internal Force Calculation*/

class InternalForceFunctor: public Functor2D<
	/*dispatch types*/ Shape,
					   Material,
	/*return type*/    void ,
	/*argument types*/ TYPELIST_3(const shared_ptr<Shape>&,const shared_ptr<Material>&,const shared_ptr<Body>&)
>{
	public: virtual ~InternalForceFunctor();
	YADE_CLASS_BASE_DOC(InternalForceFunctor,Functor,"Functor for creating/updating :yref:`Body::bound`.");
};
REGISTER_SERIALIZABLE(InternalForceFunctor);

/*Dispatcher of Internal Force Functors*/
class InternalForceDispatcher: public Dispatcher2D<
	/* functor type*/ InternalForceFunctor
>{
	public:
		virtual void action();
	//	void explicitAction(const shared_ptr<DeformableElement>& deformableelement,const Body* bdy);

		//virtual bool isActivated(){ return activated; }
	DECLARE_LOGGER;
	YADE_DISPATCHER2D_FUNCTOR_DOC_ATTRS_CTOR_PY(InternalForceDispatcher,InternalForceFunctor,/*doc is optional*/,/*attrs*/,/*ctor*/,/*py*/);
};
REGISTER_SERIALIZABLE(InternalForceDispatcher);

