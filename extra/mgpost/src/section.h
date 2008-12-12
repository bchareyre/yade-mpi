/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#ifndef SECTION_H
#define SECTION_H 1

/* normale : (a,b,c) et distance d */
typedef struct
{
  double a;
  double b;
  double c;
  double d;
}
plan;

plan section = { 0.0, 0.0, 1.0, 0.0 };

double dist_section = 0.002;

#endif

