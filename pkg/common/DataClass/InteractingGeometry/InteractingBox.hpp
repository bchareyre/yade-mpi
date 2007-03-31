/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef INTERACTION_BOX_HPP
#define INTERACTION_BOX_HPP


#include<yade/core/InteractingGeometry.hpp>


class InteractingBox : public InteractingGeometry
{
	public :
		Vector3r extents;

		InteractingBox ();
		virtual ~InteractingBox ();	
	
	protected :
		void registerAttributes();
	
	REGISTER_CLASS_INDEX(InteractingBox,InteractingGeometry);
	REGISTER_CLASS_NAME(InteractingBox);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);


};

REGISTER_SERIALIZABLE(InteractingBox,false);

#endif //  INTERACTION_BOX_HPP

