/*************************************************************************
*  Copyright (C) 2009-2012 by Franck Lominé		                         *
*  franck.lomine@insa-rennes.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*                                                                        *
*************************************************************************/
#ifdef LBM_ENGINE

#pragma once
#include<yade/lib/serialization/Serializable.hpp>
#include<yade/lib/multimethods/Indexable.hpp>

class LBMbody:  public Serializable{
    public:
        virtual ~LBMbody();
        //Real radius(){return ext[0];}
        bool isBox(){if(type==1)return true; else return false;}
        bool isPtc(){if(type==2)return true; else return false;}
        void setAsPtc(){type=2;}
        void setAsBox(){type=1;}

    YADE_CLASS_BASE_DOC_ATTRS_CTOR(LBMbody,Serializable,
        "Body class for Lattice Boltzmann Method ",
        ((Vector3r,force,Vector3r::Zero(),,"Hydrodynamic force, need to be reinitialized (LB unit)"))
        ((Vector3r,fm,Vector3r::Zero(),,"Hydrodynamic force (LB unit) at t-0.5dt"))
        ((Vector3r,fp,Vector3r::Zero(),,"Hydrodynamic force (LB unit) at t+0.5dt"))
        ((Vector3r,momentum,Vector3r::Zero(),,"Hydrodynamic momentum,need to be reinitialized (LB unit)"))
        ((Vector3r,mm,Vector3r::Zero(),,"Hydrodynamic momentum (LB unit) at t-0.5dt"))
        ((Vector3r,mp,Vector3r::Zero(),,"Hydrodynamic momentum (LB unit) at t+0.5dt"))
        ((Vector3r,pos,Vector3r::Zero(),,"Position of body"))
        ((Vector3r,vel,Vector3r::Zero(),,"Velocity of body"))
        ((Vector3r,AVel,Vector3r::Zero(),,"Angular velocity of body"))
        ((Vector3r,Fh,Vector3r::Zero(),,"Hydrodynamical force on body"))
        ((Vector3r,Mh,Vector3r::Zero(),,"Hydrodynamical momentum on body"))
        ((Real,radius,-1000.,,"Radius of body (for sphere)"))
        ((bool,isEroded,false,,"Hydrodynamical force on body"))
        ((bool,saveProperties,false,,"To save properties of the body"))
        ((short int,type,-1,," "))
        ,
        );
};
REGISTER_SERIALIZABLE(LBMbody);

#endif //LBM_ENGINE
