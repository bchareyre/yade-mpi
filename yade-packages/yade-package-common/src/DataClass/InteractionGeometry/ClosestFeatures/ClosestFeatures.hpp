/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef CLOSESTSFEATURES_HPP
#define CLOSESTSFEATURES_HPP

#include <vector>

#include <yade/yade-core/InteractionGeometry.hpp>
#include <Wm3Vector3.h>
#include <yade/yade-lib-base/yadeWm3.hpp>

class ClosestFeatures : public InteractionGeometry
{
	public :
		std::vector<std::pair<Vector3r,Vector3r> > closestsPoints;
		std::vector<int> verticesId;
		
		ClosestFeatures();
		virtual ~ClosestFeatures();

	REGISTER_CLASS_NAME(ClosestFeatures);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);

};

REGISTER_SERIALIZABLE(ClosestFeatures,false);

#endif // __CLOSESTSFEATURES_H__

