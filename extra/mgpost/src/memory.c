/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

double **AllocDouble (int dim1, int dim2)
{
  int i;
  double **Table;

  Table = (double **) calloc ((size_t) dim1, sizeof (double));
  if (Table == NULL)
    fprintf (stderr, "Error, memory allocation\n");
  for (i = 0; i < dim1; i++)
    {
      Table[i] = (double *) calloc ((size_t) dim2, sizeof (double));
      if (Table == NULL)
	fprintf (stderr, "Error, memory allocation\n");
    }
  return Table;
}

int **AllocInt (int dim1, int dim2)
{
  int i;
  int **Table;

  Table = (int **) calloc ((size_t) dim1, sizeof (int));
  if (Table == NULL)
    fprintf (stderr, "Error, memory allocation\n");
  for (i = 0; i < dim1; i++)
    {
      Table[i] = (int *) calloc ((size_t) dim2, sizeof (int));
      if (Table == NULL)
	fprintf (stderr, "Error, memory allocation\n");
    }
  return Table;
}

void mgpallocs ()
{
  FILE *allocfile;
  char buf1[40];
  char buf2[40];

  allocfile = fopen ("./mgpalloc", "r");
  if (allocfile != NULL)
    {
      fprintf (stdout, "Reading mgpalloc... ");
      fflush (stdout);

      while (!feof (allocfile))
	{
	  fscanf (allocfile, "%s %s", &buf1[0], &buf2[0]);
	  if (!strcmp ((const char *) buf1, "nb_bodies"))
	    {
	      nbbodies = atoi (buf2);
	    }
	  if (!strcmp ((const char *) buf1, "nb_times"))
	    {
	      nbtimes = atoi (buf2);
	    }
	  if ((!strcmp ((const char *) buf1, "nb_contacts")) /* XXX */
             ||(!strcmp ((const char *) buf1, "nb_interactions_per_body")))
	    {
	      nbcontacts = atoi (buf2);
	    }
	  if ((!strcmp ((const char *) buf1, "nb_data")) /* XXX */
	     ||(!strcmp ((const char *) buf1, "nb_data_per_body")))
	    {
	      nbdatas = atoi (buf2);
	    }
	}
      fclose (allocfile);
      fprintf (stdout, "done\n");
    }
  else
    {
      fprintf (stderr, "Warning: cannot find mgpalloc\n");
      nbbodies = 20000;
      nbtimes = 1;
      nbcontacts = 12;
      nbdatas = 1;
      fprintf (stdout, "...................................\n");
      fprintf (stdout, "  DEFAULT ALLOCATIONS:\n");
      fprintf (stdout, "  nb_bodies = %d\n  nb_times = %d\n", nbbodies, nbtimes);
      fprintf (stdout, "  nb_interactions_per_body = %d\n  nb_datas_per_body = %d\n", nbcontacts, nbdatas);
      fprintf (stdout, "...................................\n");
    }

  fprintf (stdout, "allocate memory... ");
  fflush (stdout);
  
  mgp_time    = (double*)        calloc ((size_t) nbtimes, sizeof (double));
  datadistrib = (int*)           calloc ((size_t) nbbodies, sizeof (int));
  dataqty     = (unsigned char*)  calloc ((size_t) nbbodies, sizeof (unsigned char));
  datas       = (double*)        calloc ((size_t) (nbbodies * nbdatas), sizeof (double));
  discrim     = (unsigned char*) calloc ((size_t) nbbodies, sizeof (unsigned char));

  if (multifiles)
    {
      x_0 = (double*) calloc ((size_t) nbbodies, sizeof (double));
      y_0 = (double*) calloc ((size_t) nbbodies, sizeof (double));
      z_0 = (double*) calloc ((size_t) nbbodies, sizeof (double));
    }

if(more_forces)
{
  Contact = (TACT *) calloc((size_t) (nbbodies * nbcontacts), sizeof(TACT));
}
  
if(with_layers)
{
  layer = (unsigned int *) calloc((size_t) (nbbodies), sizeof(unsigned int));
  layers_defined = MG_FALSE; 
}

  /* Particle centers */
  x = AllocDouble (nbbodies, nbtimes);
  y = AllocDouble (nbbodies, nbtimes);
  z = AllocDouble (nbbodies, nbtimes);

  /* Particle velocities */
  vx = AllocDouble (nbbodies, nbtimes);
  vy = AllocDouble (nbbodies, nbtimes);
  vz = AllocDouble (nbbodies, nbtimes);

  /* Angular positions and velocities for 2D particles (todo: if !with_3D_rot... )*/
  rot  = AllocDouble (nbbodies, nbtimes);
  vrot = AllocDouble (nbbodies, nbtimes);

  /* Angular positions for 3D particles */
  /* 
  if (with_3D_rot)
   {
    Ra1 = AllocDouble (nbbodies, nbtimes);
    Ra2 = AllocDouble (nbbodies, nbtimes);
    Ra3 = AllocDouble (nbbodies, nbtimes);
    Rb1 = AllocDouble (nbbodies, nbtimes);
    Rb2 = AllocDouble (nbbodies, nbtimes);
    Rb3 = AllocDouble (nbbodies, nbtimes);
   }

  */

  /* Radii associated with any particle */
  radius = AllocDouble (nbbodies, nbtimes);

  bdyty = (unsigned char *) calloc ((size_t) nbbodies, sizeof (unsigned char));
  bdyclass = (unsigned char *) calloc ((size_t) nbbodies, sizeof (unsigned char));
  color = (unsigned char *) calloc((size_t) nbbodies, sizeof(unsigned char));

  nbneighbors = AllocInt (nbbodies, nbtimes);
  neighbor = AllocInt (nbbodies * nbcontacts, nbtimes);
  Crot = AllocDouble (nbbodies * nbcontacts, nbtimes);
  Vliq = AllocDouble (nbbodies * nbcontacts, nbtimes);


/*
 dataBody = AllocDouble (nbbodies, nbtimes, nbBodyInfos);
 dataInter = AllocDouble (nbbodies * nbcontacts, nbtimes, nbInterInfos);
 */

/*
  un = AllocDouble (nbbodies * nbcontacts, nbtimes);
  ut = AllocDouble (nbbodies * nbcontacts, nbtimes);
*/

  Fn = AllocDouble (nbbodies * nbcontacts, nbtimes);
  Ft = AllocDouble (nbbodies * nbcontacts, nbtimes);
  Fs = AllocDouble (nbbodies * nbcontacts, nbtimes);
  Vn = AllocDouble (nbbodies * nbcontacts, nbtimes);
  Vt = AllocDouble (nbbodies * nbcontacts, nbtimes);

  statut = AllocInt (nbbodies * nbcontacts, nbtimes);

  fprintf (stdout, "done\n");
}

void mgpfree ()
{
  fprintf (stdout, "cleaning memory... ");
  fflush (stdout);

  if(more_forces) free (Contact);

  
  free (mgp_time);
  free (datadistrib);
  free (dataqty);
  free (datas);
  free (x);
  free (y);
  free (z);
  free (vx);
  free (vy);
  free (vz);
  free (vrot);
  free (rot);
  free (radius);
  free (bdyty);
  free (bdyclass);
  free (color);
  free (nbneighbors);
  free (neighbor);
  free (Crot);
  free (Vliq);
  free (Fn);
  free (Ft);
  free (Fs);
  free (Vn);
  free (Vt);
  /* ... ? */

  fprintf (stdout, "done\n");
}

