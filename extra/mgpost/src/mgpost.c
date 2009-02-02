/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

#include "./mgpost.h"

int main(int argc, char **argv)
{
  int             c = 0;
  char            titre[50];

  /* variable de chemin courant */
#ifdef _MACOSX
  char            chemin_courant[256];
#endif

#ifndef _MACOSX
  while (environ[c] != NULL) {
    if (!strncmp(environ[c], "MGPOST_EDITOR", 13))
      strcpy(mgpost_editor, strchr((const char *) environ[c], '=') + 1);

    if (!strncmp(environ[c], "MGPOST_HOME", 11))
      strcpy(mgpost_home, strchr((const char *) environ[c], '=') + 1);

    c++;
  }
#endif

  /* to remove some warning at compilation */
  sqrarg   = 0;
  dsqrarg  = 0;
  dmaxarg1 = 0;
  dmaxarg2 = 0;
  dminarg1 = 0;
  dminarg2 = 0;
  maxarg1  = 0;
  maxarg2  = 0;
  minarg1  = 0;
  minarg2  = 0;
  lmaxarg1 = 0;
  lmaxarg2 = 0;
  lminarg1 = 0;
  lminarg2 = 0;
  imaxarg1 = 0;
  imaxarg2 = 0;
  iminarg1 = 0;
  iminarg2 = 0;   
        
  /* echelle de couleurs */
  nb_val_couleurs = 63;
  for (c = 0; c <= 64; c++)
    gradc[c] = gradcol[c];
  for (c = 0; c <= 32; c++)
  {
    gradrouge[2*c].r   = gradc[32+c].r;
    gradrouge[2*c+1].r = gradc[32+c].r;
    gradrouge[2*c].v   = gradc[32+c].v;
    gradrouge[2*c+1].v = gradc[32+c].v;
    gradrouge[2*c].b   = gradc[32+c].b;
    gradrouge[2*c+1].b = gradc[32+c].b;

    gradbleu[2*c].r   = gradc[32-c].r;
    gradbleu[2*c+1].r = gradc[32-c].r;
    gradbleu[2*c].v   = gradc[32-c].v;
    gradbleu[2*c+1].v = gradc[32-c].v;
    gradbleu[2*c].b   = gradc[32-c].b;
    gradbleu[2*c+1].b = gradc[32-c].b;
  }
  
  /* couleurs de groupe */
  for (c=0;c<6;++c)
    strcpy(colorName[c],"Nothing");

  sprintf(num_file_format,"%%03d");

#ifdef _MACOSX
  /* obtention du chemin de travail courant */
  /* car argv[0] ne le contient pas */
  getcwd(chemin_courant, 256);
#endif

  glutInit(&argc, argv);

#ifdef _MACOSX
  chdir(chemin_courant);
#endif

  mgpost_init(argc, argv);

  sprintf(mgpost_string, "mgpost %s, 2001-2009 V.Richefeu <vincent.richefeu@hmg.inpg.fr>", MGP_VERSION);

  fprintf(stdout, "\n    mgpost version %s,\n", MGP_VERSION);
  fprintf(stdout, "    Vincent Richefeu\n");
  fprintf(stdout, "    mgpost comes with ABSOLUTELY NO WARRANTY.\n\n");

  fprintf(stdout, "    Compilation: %s at %s\n\n", __DATE__, __TIME__);

  bg_color  = mgblanc;
  fg_color  = mgnoir;
  fg_color1 = mgrouge;
  fg_color2 = mgbleu;

  param_init();
  mgpallocs();

  glutInitDisplayMode(GLUT_ACCUM | GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  glutInitWindowPosition(200, 0);
  glutInitWindowSize(W, H);

  if      (cin_mode) charger_CINfile();
  else if (his_mode) charger_HISfile();
  else               charger_geometrie();

  calcul_adim();
  centrer_repere();
  precalculs();
  findBoundaries(state, &xminB, &yminB, &zminB,
                 &xmaxB, &ymaxB, &zmaxB);

  if ((xmaxB - xminB) > (ymaxB - yminB)) 
  {
    distance = (xmaxB - xminB) * adim * 2.0;
    inc_cut = (xmaxB - xminB) * 0.1;
  } 
  else 
  {
    if ((ymaxB - yminB) > (zmaxB - zminB)) 
    {
      distance = (ymaxB - yminB) * adim * 2.0;
      inc_cut = (ymaxB - yminB) * 0.1;
    }
    else 
    {
      distance = (zmaxB - zminB) * adim * 2.0;
      inc_cut = (zmaxB - zminB) * 0.1;
    }
  }
  distance = (distance >= DISTANCE_MAX) ? DISTANCE_MAX : distance;


  if ((zmaxB - zminB) == 0.0) {	/* 2D */
    mode2D = MG_TRUE;
    L0pos[0] = 0.0;
    L0pos[1] = 0.0;
    L0pos[2] = -(xmaxB - xminB) * adim;
    L1pos[0] = 0.0;
    L1pos[1] = 0.0;
    L1pos[2] = (xmaxB - xminB) * adim;
  } else {
    L0pos[0] = xminB * adim * 2.0;
    L0pos[1] = yminB * adim * 2.0;
    L0pos[2] = zminB * adim * 2.0;
    L1pos[0] = xmaxB * adim * 2.0;
    L1pos[1] = ymaxB * adim * 2.0;
    L1pos[2] = zmaxB * adim * 2.0;
  }

  if(mode2D)
  {
    phi = -90;
    theta = 0;
  }
        
  strcpy(titre, "mgpost - ");

  if (multifiles)
    strcat(titre, "Multifiles Mode");
  else
    strcat(titre, datatitre);

  glutCreateWindow(titre);

  mgp_buildmenu();
  glutAttachMenu(GLUT_RIGHT_BUTTON);

  glClearColor(bg_color.r, bg_color.v, bg_color.b, 0.0f);
  glPointSize(1.0f);
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_LINE_SMOOTH);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0f, 1.0f, 0.1f, DISTANCE_MAX);
  glMatrixMode(GL_MODELVIEW);

  glShadeModel(GL_SMOOTH);

  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
  glEnable(GL_LIGHTING);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);

  glLightfv(GL_LIGHT0, GL_DIFFUSE, whitedif);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, whitedif);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);

  glEnable(GL_COLOR_MATERIAL);
  glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);

  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutSpecialFunc(specialKey);
  glutMouseFunc(souris);
  glutMotionFunc(mouvement);
  glutReshapeFunc(redim);
  glutIdleFunc(NULL);

  creatDisplayLists();

  rendu = disp_points;
  rendu_sup = NULL;
  disp_current = rendu;
  affgraph_func = NULL;

  processDialog = NULL;

  glutMainLoop();
  exit(EXIT_SUCCESS);
}

#include "./nrutil.c"		/* from Numerical Recipes */
#include "mgputils.c"
#include "memory.c"
#include "display.c"
#include "post.c"
#include "files.c"
#include "ihm.c"
#include "tools.c"
#include "display_funcs.c"
#include "dialog.c"
#include "tapio-K_export.c"

