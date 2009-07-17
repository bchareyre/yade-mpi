/*************************************************************************
*  Copyright (C) 2004 by Janek Kozicki                                   *
*  cosurgi@mail.berlios.de                                               *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/core/InteractionSolver.hpp>

/*! 
 * \brief This class exists for the sole purpose of calculating force and
 * momentum acting on two interacting tetrahedrons, where interaction data is
 * stored inside InteractionOfMyTetrahedron
 *
 * Those example calculations are very simple:
 *
 *  - the force is just a sum of all normals multiplied by penetrationDepth
 *    multiplied by stiffness (Hooke's Law ;)
 *
 *  - the momentum is just a force multiplied by the arm's length of the acting
 *    force. Arm's length is a projection of distance of contact point to the
 *    tetrahedron's center on direction perpendicular to the acting force.
*/

class MyTetrahedronLaw : public InteractionSolver
{
	public :
		MyTetrahedronLaw();
		void action(MetaBody*);

	REGISTER_CLASS_NAME(MyTetrahedronLaw);
	REGISTER_BASE_CLASS_NAME(InteractionSolver);
	REGISTER_ATTRIBUTES(InteractionSolver,/* */);
};

REGISTER_SERIALIZABLE(MyTetrahedronLaw);


