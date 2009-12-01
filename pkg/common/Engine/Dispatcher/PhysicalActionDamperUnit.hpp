/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/PhysicalParameters.hpp>
#include<yade/core/Body.hpp>
#include<yade/core/MetaBody.hpp>
#include<yade/core/Functor.hpp>

class PhysicalActionDamperUnit: public Functor1D<void,TYPELIST_3(const shared_ptr<PhysicalParameters>&,const Body*, MetaBody*)>{
	REGISTER_CLASS_AND_BASE(PhysicalActionDamperUnit,Functor1D);
	REGISTER_ATTRIBUTES(Functor,/* nothing here */);
	public: virtual ~PhysicalActionDamperUnit();
	protected:
	/* We are friend of BexContainer. These functions can be used safely provided that bex is NEVER read after being modified. */
	Vector3r getForceUnsynced (body_id_t id, MetaBody* rb){ return rb->bex.getForceUnsynced (id);}
	Vector3r getTorqueUnsynced(body_id_t id, MetaBody* rb){ return rb->bex.getTorqueUnsynced(id);}
};
REGISTER_SERIALIZABLE(PhysicalActionDamperUnit);
