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
#include<yade/core/Dispatcher.hpp>

class InteractionGeometry : public Serializable, public Indexable
{
	YADE_CLASS_BASE_DOC_ATTRS_CTOR_PY(InteractionGeometry,Serializable,"Geometrical configuration of interaction",
		/*no attrs*/,
		/*ctor*/,
		/*py*/
		YADE_PY_TOPINDEXABLE(InteractionGeometry)
	);
	REGISTER_INDEX_COUNTER(InteractionGeometry);
};

REGISTER_SERIALIZABLE(InteractionGeometry);


