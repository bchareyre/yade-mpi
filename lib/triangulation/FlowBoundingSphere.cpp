/*************************************************************************
*  Copyright (C) 2010 by Emanuele Catalano <catalano@grenoble-inp.fr>    *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
#ifdef FLOW_ENGINE

#include "yade/lib/triangulation/FlowBoundingSphere.hpp"

namespace CGT {
Vecteur PeriodicCellInfo::gradP;
Vecteur PeriodicCellInfo::hSize[3];
Vecteur PeriodicCellInfo::deltaP;
}


//Forcing instanciation of the template to avoid linkage problems 
typedef CGT::FlowBoundingSphere<FlowTesselation> FlowBoundingSphere;
FlowBoundingSphere ex;
#ifdef LINSOLV
typedef CGT::FlowBoundingSphereLinSolv<FlowBoundingSphere> FlowBoundingSphereLinSolv;
FlowBoundingSphereLinSolv exls;
#endif


#endif //FLOW_ENGINE

