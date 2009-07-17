/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/lib-base/yadeWm3Extra.hpp>
#include<yade/lib-serialization/Serializable.hpp>
#include<yade/lib-multimethods/Indexable.hpp>

class InteractionGeometry : public Serializable, public Indexable
{
	REGISTER_CLASS_NAME(InteractionGeometry);
	REGISTER_BASE_CLASS_NAME(Serializable Indexable);
	REGISTER_INDEX_COUNTER(InteractionGeometry);
	REGISTER_ATTRIBUTES(/* no base class*/,/*no attributes either*/); // keep this line here
};

REGISTER_SERIALIZABLE(InteractionGeometry);


