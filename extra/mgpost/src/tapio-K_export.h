/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

int cin_nbel = 0;

void bd_cinInit ();
void bd_cinDISKx (int id, double x, double y, double r);
void bd_cinSPHER (int id, double x, double y, double z, double r);
void bd_cinJONCx (int id, double x, double y, double ax1,double ax2, int flag);
void bd_cinWALxx (int id, double pos, const char * orient);
void bd_cinClose ();

