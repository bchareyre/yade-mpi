/***************************************************************************
 *   Copyright (C) 2005 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__
#define __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/InteractionSolver.hpp>
#include <yade/yade-common/Force.hpp>
#include <yade/yade-common/Momentum.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

class MassSpringBody2RigidBodyLaw : public InteractionSolver
{
	public : MassSpringBody2RigidBodyLaw ();
	private : shared_ptr<PhysicalAction> actionForce;	
	private : shared_ptr<PhysicalAction> actionMomentum;
	public  : int sdecGroupMask;
	public  : int springGroupMask;

	public : void registerAttributes();

	public : void calculateForces(Body* body);
	
	REGISTER_CLASS_NAME(MassSpringBody2RigidBodyLaw);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

REGISTER_SERIALIZABLE(MassSpringBody2RigidBodyLaw,false);

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#endif // __MASSSPRINGBODY2RIGIDBODYNAMICENGINE_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
