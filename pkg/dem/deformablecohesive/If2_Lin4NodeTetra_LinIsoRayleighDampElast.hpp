/*************************************************************************
*  Copyright (C) 2013 by Burak ER  burak.er@btu.edu.tr                   *
*									 									 *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once
#include <core/Shape.hpp>
#include <lib/base/Math.hpp>
#include <core/Interaction.hpp>
#include <core/Scene.hpp>
#include <core/State.hpp>
#include <core/Shape.hpp>
#include <core/IGeom.hpp>
#include <core/IPhys.hpp>
#include <core/Functor.hpp>
#include <core/Dispatcher.hpp>
#include <pkg/common/Aabb.hpp>
#include <pkg/dem/deformablecohesive/FEInternalForceDispatchers.hpp>
#include <pkg/dem/deformablecohesive/Lin4NodeTetra.hpp>
#include <pkg/dem/deformablecohesive/LinElastMat.hpp>



class If2_Lin4NodeTetra_LinIsoRayleighDampElast : public InternalForceFunctor
{
	public :

		virtual void go(const shared_ptr<Shape>&,const shared_ptr<Material>&,const shared_ptr<Body>&);
		virtual ~If2_Lin4NodeTetra_LinIsoRayleighDampElast();
		FUNCTOR2D(Lin4NodeTetra,LinIsoRayleighDampElastMat);

		YADE_CLASS_BASE_DOC(If2_Lin4NodeTetra_LinIsoRayleighDampElast,InternalForceFunctor,"Apply internal forces of the tetrahedral element using lumped mass theory")
};
REGISTER_SERIALIZABLE(If2_Lin4NodeTetra_LinIsoRayleighDampElast);


