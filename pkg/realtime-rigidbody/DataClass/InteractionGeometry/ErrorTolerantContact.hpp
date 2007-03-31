/*************************************************************************
*  Copyright (C) 2004 by Olivier Galizzi                                 *
*  olivier.galizzi@imag.fr                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef ERRORTOLERANTCONTACTMODEL_HPP
#define ERRORTOLERANTCONTACTMODEL_HPP

#include<yade/core/InteractionGeometry.hpp>
#include <Wm3Vector3.h>
#include<yade/lib-base/yadeWm3.hpp>

class ErrorTolerantContact : public InteractionGeometry
{
	public :
		vector<pair<Vector3r,Vector3r> > closestPoints;
		Vector3r o1p1;
		Vector3r o2p2;
		Vector3r normal;

	//public : Vector3r t;	
	//public : Vector3r s;
	//public : Vector3r l;
	//public : Real nu;
	//public : std::pair<t_Vertex,t_Vertex> idVertex;
	//public : t_ConnexionType type;
	
	// construction
		ErrorTolerantContact ();
		virtual ~ErrorTolerantContact ();

	protected :
		virtual void postProcessAttributes(bool deserializing);
	public :
		void registerAttributes();

	REGISTER_CLASS_NAME(ErrorTolerantContact);
	REGISTER_BASE_CLASS_NAME(InteractionGeometry);
	
	REGISTER_CLASS_INDEX(ErrorTolerantContact,InteractionGeometry);
};

REGISTER_SERIALIZABLE(ErrorTolerantContact,false);

#endif // ERRORTOLERANTCONTACTMODEL_HPP
