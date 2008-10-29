/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef BSS_SWEPT_SPHERE_LINE_SEGMENT_HPP
#define BSS_SWEPT_SPHERE_LINE_SEGMENT_HPP


#include<yade/core/InteractingGeometry.hpp>


class BssSweptSphereLineSegment : public InteractingGeometry
{
	public :
                Real radius, length;
		Vector3r position;
                Vector3r orientation; // This is a normalized vector
                
                
                BssSweptSphereLineSegment ();
                virtual ~BssSweptSphereLineSegment ();	
	
	protected :
		void registerAttributes();
	
                REGISTER_CLASS_INDEX(BssSweptSphereLineSegment,InteractingGeometry);
                REGISTER_CLASS_NAME(BssSweptSphereLineSegment);
	REGISTER_BASE_CLASS_NAME(InteractingGeometry);


};

REGISTER_SERIALIZABLE(BssSweptSphereLineSegment,false);

#endif //  BSS_SWEPT_SPHERE_LINE_SEGMENT_HPP

