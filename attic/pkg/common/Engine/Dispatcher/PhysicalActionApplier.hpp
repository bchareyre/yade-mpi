/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once


#include<yade/core/Dispatcher.hpp>
#include<yade/lib-multimethods/DynLibDispatcher.hpp>

#include<yade/pkg-common/PhysicalActionApplierUnit.hpp>

class Body;
class PhysicalActionApplier: public Dispatcher1D<PhysicalParameters,PhysicalActionApplierUnit,void,TYPELIST_3(const shared_ptr<PhysicalParameters>&,const Body*, Scene*)>{
	public: virtual void action();
	REGISTER_CLASS_AND_BASE(PhysicalActionApplier,Dispatcher1D);
};
REGISTER_SERIALIZABLE(PhysicalActionApplier);

