/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/


/* FORMAT D'EXPORTATION POUR tapio-K et K-viar
 * V. Richefeu
 */

void
bd_cinInit ()
{
  outfile = fopen ("export.cin", "w");
  
  fprintf(outfile,"Exported from mgpost\n");
  fprintf(outfile,"%d 0.0\n", cin_nbel);
 
}

void
bd_cinDISKx (int id, double x, double y, double r)
{
  fprintf(outfile,"0\n");
  fprintf(outfile,"%lg %lg %lg 0 0 0 0\n", r, x, y);
}

void
bd_cinSPHER (int id, double x, double y, double z, double r)
{
  fprintf(outfile,"1\n");
  fprintf(outfile,"%lg %lg %lg %lg 0 0 0 0 0 0 0 0 0\n", r, x, y, z);
}

void
bd_cinJONCx (int id, double x, double y, double ax1, double ax2, int flag)
{
  fprintf (stderr, "JONCx non disponible dans le format tapio-K\n");
}

void
bd_cinWALxx (int id, double pos, const char * orient)
{
  int norient = 100;  

  if (!strncmp(orient,"WALX0", 5)) norient = 100;
  if (!strncmp(orient,"WALX1", 5)) norient = 101;
  if (!strncmp(orient,"WALY0", 5)) norient = 102;
  if (!strncmp(orient,"WALY1", 5)) norient = 103;
  if (!strncmp(orient,"WALZ0", 5)) norient = 104;
  if (!strncmp(orient,"WALZ1", 5)) norient = 105;

  fprintf(outfile,"%d\n", norient);
  fprintf(outfile,"%lg 0\n", pos);
}

void
bd_cinClose ()
{
  fprintf(outfile,"\n");
  fclose (outfile);

  fprintf (stdout, "Le fichier 'builded.cin' a ete cree\n");
  fprintf (stdout, "ATTENTION : vous devez surement changer la seconde ligne en : %d 0\n", cin_nbel);
}

