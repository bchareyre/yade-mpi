/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#define DK8_BUFDIM 10
char * DK8(double D);

void bd_bodiesInit ();
void bd_bodiesDISKx (int id, double x, double y, double r);
void bd_bodiesSPHER (int id, double x, double y, double z, double r);
void bd_bodiesJONCx (int id, double x, double y, double ax1,double ax2, int flag);
void bd_bodiesWALxx (int id, double pos, const char * orient);
void bd_bodiesClose ();

