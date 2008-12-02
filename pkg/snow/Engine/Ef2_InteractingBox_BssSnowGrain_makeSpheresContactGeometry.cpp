/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@berlios.de                                                    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include"Ef2_InteractingBox_BssSnowGrain_makeSpheresContactGeometry.hpp"
#include<yade/pkg-dem/SpheresContactGeometry.hpp>
#include<yade/pkg-common/InteractingSphere.hpp>
#include<yade/pkg-common/InteractingBox.hpp>

#include<yade/lib-base/yadeWm3Extra.hpp>



bool Ef2_InteractingBox_BssSnowGrain_makeSpheresContactGeometry::go(
		const shared_ptr<InteractingGeometry>& cm1,
		const shared_ptr<InteractingGeometry>& cm2,
		const Se3r& se31,
		const Se3r& se32,
		const shared_ptr<Interaction>& c)
{
	bool result = g.go(cm1,cm2,se32,se32,c);
	return result;
}


bool Ef2_InteractingBox_BssSnowGrain_makeSpheresContactGeometry::goReverse(	const shared_ptr<InteractingGeometry>& cm1,
						const shared_ptr<InteractingGeometry>& cm2,
						const Se3r& se31,
						const Se3r& se32,
						const shared_ptr<Interaction>& c)
{
	bool result = g.goReverse(cm1,cm2,se32,se32,c);
	return result;
}

YADE_PLUGIN();

