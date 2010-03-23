/*************************************************************************
*  Copyright (C) 2008 by Jérôme DURIEZ                                   *
*  duriez@geo.hmg.inpg.fr                                                *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

// #include<yade/pkg-common/LawFunctor.hpp>
#include<yade/core/GlobalEngine.hpp>
#include <set>
#include <boost/tuple/tuple.hpp>



class Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity : public GlobalEngine
{
	public :
// <<<<<<< TREE
		void action();
	YADE_CLASS_BASE_DOC_ATTRS(Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity,
// =======
// 		void action();
// 	YADE_CLASS_BASE_DOC_ATTRS(NormalInelasticityLaw,
// >>>>>>> MERGE-SOURCE
				  GlobalEngine,
				  "Contact law including cohesion, moment transfer and inelastic compression behaviour\n\n This contact Law is inspired by :yref:`CohesiveFrictionalContactLaw` (inspired itselve directly from the work of Plassiard & Belheine, see the corresponding articles in (Annual Report 2006) in http://geo.hmg.inpg.fr/frederic/Discrete_Element_Group_FVD.html for example).\n\n It allows so to set moments, cohesion, tension limit and (that's the difference) inelastic unloadings in compression between bodies. All that concerned brokenBodies (this flag and the erosionactivated one) and the useless 'iter' has been suppressed.\n\n The Relationsships corresponding are Ip2_2xCohFrictMat_NormalInelasticityPhys, where the rigidities, the friction angles (with their tan()), and the orientations of the interactions are calculated. No more cohesion and tension limits are computed for all the interactions.\n\n To use it you should also use :\n- :yref:`CohesiveFrictionalMat` for bodies, with *isCohesive* = 1 (A verifier ce dernier point)\n- :yref:`Ip2_2xCohFrictMat_NormalInelasticityPhys` (=> which involves interactions of :yref:`NormalInelasticityPhys` type)",
				  ((int,sdecGroupMask,1,"?"))
				  ((Real,coeff_dech,1.0,"=kn(unload) / kn(load)"))
				  ((bool,momentRotationLaw,true,"boolean, true=> computation of a torque (against relative rotation) exchanged between particles"))
				  ((bool,momentAlwaysElastic,false,"boolean, true=> the torque (computed only if momentRotationLaw !!) is not limited by a plastic threshold"))
				  );
};

REGISTER_SERIALIZABLE(Law2_ScGeom_NormalInelasticityPhys_NormalInelasticity);


