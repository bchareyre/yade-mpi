/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#define NB_PROFILES 10
#define NB_GRAINS_BY_PROFILES 100

int iprofile[NB_PROFILES][NB_GRAINS_BY_PROFILES];
int nbg_profile[NB_PROFILES];
double pos_profile[NB_PROFILES][6];
unsigned char ActiveProfile[NB_PROFILES]; 

/* calcul des deplacement cumules */
double *xbak, *ybak, *zbak, *Dst;

/* calcul des deformations */
int *vois, *nbvois;
double *epsXX, *epsYY;

