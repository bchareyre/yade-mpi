/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

// #include<yade/core/GlobalEngine.hpp> // a remplacer par :
#include<yade/pkg-common/LawFunctor.hpp>
#include<yade/pkg-dem/ScGeom.hpp>
#include<yade/pkg-dem/NormalInelasticityPhys.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>



class Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity : public LawFunctor
{
	public :
		virtual void go(shared_ptr<InteractionGeometry>&, shared_ptr<InteractionPhysics>&, Interaction*);

	FUNCTOR2D(ScGeom,NormalInelasticityPhys);

	YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity,
				LawFunctor,
				"Contact law including cohesion, moment transfer and inelastic compression behaviour\n\n For what concerns moment transfer it is inspired indirectly by the work of Plassiard & Belheine, see the corresponding articles in [DeghmReport2006]_ for example).\n\n It allows moreover to set inelastic unloadings in compression between bodies.\n\n The Relationsships corresponding are Ip2_2xNormalInelasticMat_NormalInelasticityPhys, where the rigidities, the friction angles (with their tan()), and the orientations of the interactions are calculated.\n\n To use it you should also use :\n- :yref:`NormalInelasticMat` \n- :yref:`Ip2_2xNormalInelasticMat_NormalInelasticityPhys` (=> which involves interactions of :yref:`NormalInelasticityPhys` type).\n\n The effects of this law are illustrated in scripts/NormalInelasticityTest.py",
				((bool,momentRotationLaw,true,"boolean, true=> computation of a torque (against relative rotation) exchanged between particles"))
				((bool,momentAlwaysElastic,false,"boolean, true=> the torque (computed only if momentRotationLaw !!) is not limited by a plastic threshold"))
				);
	
};

REGISTER_SERIALIZABLE(Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity);

