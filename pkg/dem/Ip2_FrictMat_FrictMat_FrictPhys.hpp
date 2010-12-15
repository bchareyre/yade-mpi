/*************************************************************************
*  Copyright (C) 2007 by Bruno CHAREYRE                                 *
*  bruno.chareyre@hmg.inpg.fr                                        *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#pragma once

#include<yade/pkg/common/Dispatching.hpp>
#include<yade/pkg/common/MatchMaker.hpp>

class Ip2_FrictMat_FrictMat_FrictPhys: public IPhysFunctor{
	public:
		virtual void go(const shared_ptr<Material>& b1,
			const shared_ptr<Material>& b2,
			const shared_ptr<Interaction>& interaction);
	FUNCTOR2D(FrictMat,FrictMat);
	YADE_CLASS_BASE_DOC_ATTRS(Ip2_FrictMat_FrictMat_FrictPhys,IPhysFunctor,"Create a :yref:`FrictPhys` from two :yref:`FrictMats<FrictMat>`. The compliance of one sphere under symetric point loads is defined here as 1/(E.r), with E the stiffness of the sphere and r its radius, and corresponds to a compliance 1/(2.E.r)=1/(E.D) from each contact point. The compliance of the contact itself will be the sum of compliances from each sphere, i.e. 1/(E.D1)+1/(E.D2) in the general case, or 1/(E.r) in the special case of equal sizes. Note that summing compliances corresponds to an harmonic average of stiffnesss, which is how kn is actually computed in the :yref:`Ip2_FrictMat_FrictMat_FrictPhys` functor.\n\nThe shear stiffness ks of one sphere is defined via the material parameter :yref:`ElastMat::poisson`, as ks=poisson*kn, and the resulting shear stiffness of the interaction will be also an harmonic average.",
		((shared_ptr<MatchMaker>,frictAngle,,,"Instance of :yref:`MatchMaker` determining how to compute interaction's friction angle. If ``None``, minimum value is used."))
	);
};
REGISTER_SERIALIZABLE(Ip2_FrictMat_FrictMat_FrictPhys);


