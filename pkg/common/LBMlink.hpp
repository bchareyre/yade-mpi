/*************************************************************************
*  Copyright (C) 2009-2012 by Franck Lominé		                         *
*  franck.lomine@insa-rennes.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v3 or later. See file LICENSE for details. *
*                                                                        *
*************************************************************************/
#ifdef LBM_ENGINE

#pragma once
#include<yade/lib/serialization/Serializable.hpp>
#include<yade/lib/multimethods/Indexable.hpp>

class LBMlink: public Serializable{
    public:
        void ReinitDynamicalProperties();
        virtual ~LBMlink();

    YADE_CLASS_BASE_DOC_ATTRS_CTOR(LBMlink,Serializable,
        "Link class for Lattice Boltzmann Method ",
        ((int,sid,-1,,"Solid node identifier "))
        ((int,fid,-1,,"Fluid node identifier "))
        ((short int,i,-1,,"direction index of the link"))
        ((int,nid1,-1,,"fixed node identifier"))
        ((int,nid2,-1,,"fixed node identifier or -1 if node points outside"))
        ((short int,idx_sigma_i,-1,,"sigma_i direction index  (Fluid->Solid)"))
        ((bool,isBd,false,,"True if it is a boundary link"))
        ((bool,PointingOutside,false,,"True if it is a link pointing outside to the system (from a fluid or solid node)"))
        ((Vector3r,VbMid,Vector3r::Zero(),,"Velocity of boundary at midpoint"))
        ((Vector3r,DistMid,Vector3r::Zero(),,"Distance between middle of the link and mass center of body"))
        ((Real,ct,0.,,"Coupling term in modified bounce back rule")),
        );
};
REGISTER_SERIALIZABLE(LBMlink);

#endif //LBM_ENGINE
