/*************************************************************************
*  Copyright (C) 2009-2012 by Franck Lominé		                         *
*  franck.lomine@insa-rennes.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*                                                                        *
*************************************************************************/
#ifdef LBM_ENGINE

#include "LBMnode.hpp"

YADE_PLUGIN((LBMnode));
LBMnode::~LBMnode(){};

void LBMnode::MixteBC(string lbmodel,Real density, Vector3r U, string where){
    Real rhoVx=density*U.x();
    Real rhoVy=density*U.y();
    if(!strcmp(lbmodel.c_str(), "d2q9" )){
        if(!strcmp(where.c_str(), "Xm" )){
            f[1]=f[3]+(2./3.)*rhoVx;
            f[5]=f[7]-0.5*(f[2]-f[4])+(1./6.)*rhoVx+ 0.5*rhoVy;
            f[8]=f[6]+0.5*(f[2]-f[4])+(1./6.)*rhoVx- 0.5*rhoVy;
            }
        else if(!strcmp(where.c_str(), "Xp" )){
            f[3]=f[1]-(2./3.)*rhoVx;
            f[7]=f[5]+0.5*(f[2]-f[4])-(1./6.)*rhoVx - 0.5*rhoVy;
            f[6]=f[8]-0.5*(f[2]-f[4])-(1./6.)*rhoVx + 0.5*rhoVy;
            }
        else if(!strcmp(where.c_str(), "Ym" )){
            f[2]=f[4]+(2./3.)*rhoVy;
            f[5]=f[7]-0.5*(f[1]-f[3])+0.5*rhoVx + (1./6.)*rhoVy;
            f[6]=f[8]+0.5*(f[1]-f[3])-0.5*rhoVx + (1./6.)*rhoVy;
            }
        else if(!strcmp(where.c_str(), "Yp" )){
            f[4]=f[2]-(2./3.)*rhoVy;
            f[7]=f[5]+0.5*(f[1]-f[3])-0.5*rhoVx- (1./6.)*rhoVy;
            f[8]=f[6]-0.5*(f[1]-f[3])+0.5*rhoVx -(1./6.)*rhoVy;
            }
        else if(!strcmp(where.c_str(), "XmYmZp" )){
            f[1]=f[3]+(2./3.)*rhoVx;
            f[2]=f[4]+(2./3.)*rhoVy;
            f[5]=f[7] + (1./6.)*density*(U.x()+U.y());
            f[6]=0.5*(density*(1.-U.x() -(2./3.)*U.y())-f[0]-2.*(f[3]+f[4]+f[7]));
            f[8]=0.5*(density*(1.-(2./3.)*U.x() -U.y())-f[0]-2.*(f[3]+f[4]+f[7]));
            }
        else if(!strcmp(where.c_str(), "XmYpZp" )){
            f[1]=f[3]+(2./3.)*rhoVx;
            f[4]=f[2]-(2./3.)*rhoVy;
            f[5]=0.5*(density*(1.-(2./3.)*U.x()+U.y())-f[0]-2.*(f[2]+f[3]+f[6]));
            f[7]=0.5*(density*(1.-U.x()+(2./3.)*U.y())-f[0]-2.*(f[2]+f[3]+f[6]));
            f[8]=f[6]+(1./6.)*density*(U.x()-U.y());
            }
        else if(!strcmp(where.c_str(), "XpYmZp" )){
            f[2]=f[4]+(2./3.)*rhoVy;
            f[3]=f[1]-(2./3.)*rhoVx;
            f[5]=0.5*(density*(1.+U.x()-(2./3.)*U.y())-f[0]-2.*(f[1]+f[4]+f[8]));
            f[6]=f[8]-(1./6.)*density*(U.x()-U.y());
            f[7]=0.5*(density*(1.+(2./3.)*U.x()-U.y())-f[0]-2.*(f[1]+f[4]+f[8]));
            }
        else if(!strcmp(where.c_str(), "XpYpZp" )){
            f[3]=f[1]-(2./3.)*rhoVx;
            f[4]=f[2]-(2./3.)*rhoVy;
            f[6]=0.5*(density*(1.+(2./3.)*U.x()+U.y())-f[0]-2.*(f[1]+f[2]+f[5]));
            f[7]=f[5]-(1./6.)*density*(U.x()+U.y());
            f[8]=0.5*(density*(1.+U.x()+(2./3.)*U.y())-f[0]-2.*(f[1]+f[2]+f[5]));
            }
        else {exit(-1);}
    }else {exit(-1);}
    return;
}


bool LBMnode::checkIsNewObstacle(){
    if(isObstacle){
        if(!wasObstacle) {isNewObstacle=true;wasObstacle=true;}
        else {isNewObstacle=false;wasObstacle=true;}
        return(isNewObstacle);
    } else return(false);
}

bool LBMnode::checkIsNewFluid(){
    if(!isObstacle){
        if(wasObstacle) {isNewFluid=true;wasObstacle=false;}
        else {isNewFluid=false;wasObstacle=false;}
        return(isNewFluid);
    } else return(false);
}

void LBMnode::DispatchBoundaryConditions(int SizeNx,int SizeNy,int SizeNz){
    applyBC                  =false;
    applyXmBC                =false;
    applyYmXmBC              =false;
    applyYpXmBC              =false;
    applyXpBC                =false;
    applyYmXpBC              =false;
    applyYpXpBC              =false;
    applyYpBC                =false;
    applyYmBC                =false;
    if((i==0)&&(j>0)&&(j<SizeNy-1)) {applyXmBC        =true; applyBC=true;}
    if((i==0)&&(j==0)) {applyYmXmBC                   =true; applyBC=true;}
    if((i==0)&&(j==SizeNy-1)) {applyYpXmBC            =true; applyBC=true;}
    if((i==SizeNx-1)&&(j>0)&&(j<SizeNy-1)) {applyXpBC =true; applyBC=true;}
    if((i==SizeNx-1)&&(j==0)) {applyYmXpBC            =true; applyBC=true;}
    if((i==SizeNx-1)&&(j==SizeNy-1)) {applyYpXpBC     =true; applyBC=true;}
    if((i>0)&&(i<SizeNx-1)&&(j==0)) {applyYmBC        =true; applyBC=true;}
    if((i>0)&&(i<SizeNx-1)&&(j==SizeNy-1)) {applyYpBC =true; applyBC=true;}
return;
}


void LBMnode::SetCellIndexesAndPosition(int indI, int indJ, int indK){
    i=indI;
    j=indJ;
    k=indK;
    posb=Vector3r((Real) indI,(Real) indJ,(Real) indK);
    return;
}

#endif //LBM_ENGINE
