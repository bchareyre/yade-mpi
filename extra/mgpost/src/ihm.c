/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

void mgpost_init(int argc, char **argv)
{
  int             i, c;
  char            namebuf[256];
  char            path[256];
  char            buf[256];
  
#ifndef _WINDOWS
  const char      pathSeparator = 47;
#else
  const char      pathSeparator = 92;
#endif
  
  if (argc == 1) {
    if (!access("./mgp.out.001", F_OK)) {
      nfile = 1;
      strcpy(datafilename, "mgp.out.001");
      strcpy(num_file_format,"%03d");
      fgziped = MG_FALSE;
      multifiles = MG_TRUE;
    } else if (!access("./mgp.out.001.gz", F_OK)) {
      nfile = 1;
      strcpy(datafilename, "mgp.out.001.gz");
      strcpy(num_file_format,"%03d");
      fgziped = MG_TRUE;
      multifiles = MG_TRUE;
    } else if (!access("./mgp.out.1", F_OK)) {
      nfile = 1;
      strcpy(datafilename, "mgp.out.1");
      strcpy(num_file_format,"%d");
      fgziped = MG_FALSE;
      multifiles = MG_TRUE; 
    } else {
      fprintf(stderr, "Cannot find a readable file such as 'mgp.out.001' or 'mgp.out.001.gz'\n");
      exit(EXIT_FAILURE);
    }
  }
  
  if (argc == 2) {
    if (strchr((const char *) argv[1], pathSeparator) != NULL) {
      strcpy(namebuf, strrchr((const char *) argv[1], pathSeparator) + 1);
      
      strcpy(path, argv[1]);
      strcpy(strrchr(path, pathSeparator), "\0");
      fprintf(stdout, "Working directory: %s\n", path);
      chdir((const char *) path);
    } else
      strcpy(namebuf, (const char *) argv[1]);
    
    
      if (!strncmp(namebuf, "mgp.out", 7)) {
        strcpy(buf, &namebuf[8]);
        buf[3] = 0;
        nfile = atoi(buf);
      
        strcpy(datafilename, (const char *) namebuf);
      
        if (strlen(namebuf) > 11)
          fgziped = MG_TRUE;
        else
          fgziped = MG_FALSE;
      
        multifiles = MG_TRUE;
      }

      strcpy(buf, &namebuf[strlen(namebuf) - 3]);
      buf[3] = 0;
    
      if (!strcmp(buf, "mgp")) {
        strcpy(datafilename, (const char *) namebuf);
        multifiles = MG_FALSE;
      }

      strcpy(buf, &namebuf[strlen(namebuf) - 6]);
      buf[6] = 0;
    
      if (!strcmp(buf, "mgp.gz")) {
        strcpy(datafilename, (const char *) namebuf);
        multifiles = MG_FALSE;
      }
  }
  i = 1;
  while (i < argc) {
    if (!strcmp(argv[i], "-v")) {
      
      fprintf(stdout, "\n    mgpost version %s,\n", MGP_VERSION);
      fprintf(stdout, "    Vincent Richefeu\n");
      fprintf(stdout, "    mgpost comes with ABSOLUTELY NO WARRANTY.\n\n");
      
      exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[i], "-files")) {
      make_mgpalloc_file();
      make_mgconf_file();
      exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[i], "-mgpalloc")) {
      make_mgpalloc_file();
      exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[i], "-mgconf")) {
      make_mgconf_file();
      exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[i], "-nformat")) {
      strcpy(num_file_format, (const char *) argv[i + 1]);
    }

    if (!strcmp(argv[i], "-num")) {
      if      (atoi(argv[i + 1]) == 3) strcpy(num_file_format,"%03d");
      else if (atoi(argv[i + 1]) == 4) strcpy(num_file_format,"%04d");
      else                             strcpy(num_file_format,"%d");
    }

    if (!strcmp(argv[i], "-i")) {
      strcpy(datafilename, (const char *) argv[i + 1]);
      cin_mode = MG_FALSE;
      his_mode = MG_FALSE;
      multifiles = MG_FALSE;
    }

    if (!strcmp(argv[i], "-cin")) {
      strcpy(datafilename, (const char *) argv[i + 1]);
      cin_mode = MG_TRUE;
      his_mode = MG_FALSE;
      multifiles = MG_FALSE;
    }

    if (!strcmp(argv[i], "-his")) {
      if (argv[i + 1] != NULL)
        nfile = atoi(argv[i + 1]);
      else
        nfile = 1;
      
      sprintf((char *) numfile, num_file_format, nfile);
      strncpy(datafilename, "spl_nwk_", 8);
      strcat(datafilename, (const char *) numfile);
      strcat(datafilename, ".his");
      /*strcpy(datafilename, (const char *) argv[i + 1]);*/
      his_mode = MG_TRUE;
      cin_mode = MG_FALSE;
      multifiles = MG_TRUE;
    }

    if (!strcmp(argv[i], "-polye")) {
      more_forces = MG_TRUE;
    }

    if (!strcmp(argv[i], "-fluid")) {
      with_fluid = MG_TRUE;
    }

    if (!strcmp(argv[i], "-layer")) {
      with_layers = MG_TRUE;
      
      if (argv[i + 1] != NULL)
        nbLayers = atoi(argv[i+1]);
      else
        nbLayers = 5;
      
    }

    if (!strcmp(argv[i], "-p")) {
      strcpy(pstfilename, (const char *) argv[i + 1]);
      
      cin_mode = MG_FALSE;
      fgziped = MG_FALSE;
      multifiles = MG_TRUE;
      
      mgpallocs();
      pstcom = (PSTCOM *) calloc(50, sizeof(PSTCOM));
      
      compil_pstcom();
      pst_evol(MG_FALSE);
      
      mgpfree();
      free(pstcom);
      
      exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[i], "-pz")) {
      strcpy(pstfilename, (const char *) argv[i + 1]);
      
      cin_mode = MG_FALSE;
      fgziped = MG_TRUE;
      multifiles = MG_TRUE;
      
      mgpallocs();
      pstcom = (PSTCOM *) calloc(50, sizeof(PSTCOM));
      
      compil_pstcom();
      pst_evol(MG_TRUE);
      
      mgpfree();
      free(pstcom);
      
      exit(EXIT_SUCCESS);
    }

    if (!strcmp(argv[i], "-mf")) {
      
      if (argv[i + 1] != NULL)
        nfile = atoi(argv[i + 1]);
      else
        nfile = 1;
      
      sprintf((char *) numfile, num_file_format, nfile);
      strncpy(datafilename, "mgp.out.", 8);
      strcat(datafilename, (const char *) numfile);
      fgziped = MG_FALSE;
      multifiles = MG_TRUE;
    }

    if (!strcmp(argv[i], "-mfz")) {
      
      if (argv[i + 1] != NULL)
        nfile = atoi(argv[i + 1]);
      else
        nfile = 1;
      
      sprintf((char *) numfile, num_file_format, nfile);
      strncpy(datafilename, "mgp.out.", 8);
      strcat(datafilename, (const char *) numfile);
      strcat(datafilename, ".gz");
      fgziped = MG_TRUE;
      multifiles = MG_TRUE;
    }

    if (!strcmp(argv[i], "-r0")) {
      middle_rep = MG_FALSE;
    }

    if (!strcmp(argv[i], "-dim")) {
      W = atoi(argv[i + 1]);
      H = atoi(argv[i + 2]);
    }

    if (!strcmp(argv[i], "-2d")) {
      phi = -90;
      theta = 0;
      mode2D = MG_TRUE;
    }

    if (!strcmp(argv[i], "-mono")) {
      for (c = 0; c <= nb_val_couleurs; c++)
        gradc[c] = gradmono[c];
    }

    if (!strcmp(argv[i], "-colormap")) {
      FILE * f;
      f = fopen ("colormap","r");
      if (f != NULL) 
      {
        fscanf(f,"%d",&nb_val_couleurs);
        if (nb_val_couleurs >= 80) nb_val_couleurs = 79; 
        for (c = 0; c <= nb_val_couleurs; c++)
        {
          fscanf(f,"%f %f %f",&(gradc[c].r),&(gradc[c].v),&(gradc[c].b));
        }
      }
      else
      {
        fprintf(stderr,"File colormap not found\n");
      }
    }   

    if (!strcmp(argv[i], "-section")) {
      section.a = atof(argv[i + 1]);
      section.b = atof(argv[i + 2]);
      section.c = atof(argv[i + 3]);
      section.d = atof(argv[i + 4]);
    }

    if (!strcmp(argv[i], "-dsec")) {
      dist_section = atof(argv[i + 1]);
    }

    i++;
  }
}

/* Read the file 'mgconf' */
void param_init()
{
  FILE           *rcfile;
  char           *mgtoken[64];
  int             nbtoken = 0;
  char           *s;
  char            clin[256];
  int             i;
  
  rcfile = fopen("./mgconf", "r");
  if (rcfile != NULL) {
    fprintf(stdout, "Reading mgconf ... ");
    while (!feof(rcfile)) {
      /* read one line */
      fgets(clin, 256, rcfile);
      if (feof(rcfile))
        break;
      
      /* decomposition of the line into tokens */
      s = (char *) strtok(clin, (const char *) delim);
      nbtoken = 0;
      mgtoken[0] = s;
      
      while (s != NULL) {
        mgtoken[nbtoken++] = s;
        s = (char *) strtok(NULL, (const char *) delim);
      }
      mgtoken[nbtoken] = 0;
      
      /* analyze of the tokens */
      i = 0;
      while (i < nbtoken) {
        if (!strncmp((const char *) mgtoken[i], "#", 1))
          break;
        if (!strncmp((const char *) mgtoken[i], "!", 1))
          break;
        
        if ((!strcmp((const char *) mgtoken[i], "Lfen"))
              || (!strcmp((const char *) mgtoken[i], "Wwin"))) {
          W = atoi(mgtoken[++i]);
              }
              if ((!strcmp((const char *) mgtoken[i], "Hfen"))
                    || (!strcmp((const char *) mgtoken[i], "Hwin"))) {
                H = atoi(mgtoken[++i]);
                    }
                    if (!strcmp((const char *) mgtoken[i], "2d")) {
                      if (!strcmp((const char *) mgtoken[++i], "yes")) {
                        phi = -90;
                        theta = 0;
                        mode2D = MG_TRUE;
                        glDisable(GL_DEPTH_TEST);
                      } else {
                        if (!multifiles) {
                          phi = PHI_INIT;
                          theta = THETA_INIT;
                        }
                        mode2D = MG_FALSE;
                        glEnable(GL_DEPTH_TEST);
                      }
                    }
                    if (!strcmp((const char *) mgtoken[i], "autoScaleColors")) {
                      if (!strcasecmp((const char *) mgtoken[++i], "yes"))
                        dynamic_scale = MG_TRUE;
                      else
                        dynamic_scale = MG_FALSE;
                    }
                    if (!strcmp((const char *) mgtoken[i], "GridXYZ")) {
                      grillX = atof(mgtoken[++i]);
                      grillY = atof(mgtoken[++i]);
                      grillZ = atof(mgtoken[++i]);
                    }
                    if ((!strcmp((const char *) mgtoken[i], "multiVit"))
                          || (!strcmp((const char *) mgtoken[i], "vlocyFactor"))) {
                      v_adi = atof(mgtoken[++i]);
                          }
                          if ((!strcmp((const char *) mgtoken[i], "vitX"))
                                || (!strcmp((const char *) mgtoken[i], "vlocyX"))) {
                            vxrep = atof(mgtoken[++i]);
                                }
                                if ((!strcmp((const char *) mgtoken[i], "vitY"))
                                      || (!strcmp((const char *) mgtoken[i], "vlocyY"))) {
                                  vyrep = atof(mgtoken[++i]);
                                      }
                                      if ((!strcmp((const char *) mgtoken[i], "vitZ"))
                                            || (!strcmp((const char *) mgtoken[i], "vlocyZ"))) {
                                        vzrep = atof(mgtoken[++i]);
                                            }
                                            if (!strcmp((const char *) mgtoken[i], "valcMAX")) {
                                              valc_rouge = atof(mgtoken[++i]);
                                            }
                                            if (!strcmp((const char *) mgtoken[i], "valcMIN")) {
                                              valc_bleu = atof(mgtoken[++i]);
                                            }
                                            if ((!strcmp((const char *) mgtoken[i], "funRVB"))
                                                  || (!strcmp((const char *) mgtoken[i], "funRGB"))) {
                                              mgfun.r = atof(mgtoken[++i]);
                                              mgfun.v = atof(mgtoken[++i]);
                                              mgfun.b = atof(mgtoken[++i]);
                                                  }
                                                  if (!strcmp((const char *) mgtoken[i], "bgcolor")) {
                                                    bg_color = select_color((const char *) mgtoken[++i]);
                                                  }
                                                  if (!strcmp((const char *) mgtoken[i], "fgcolor")) {
                                                    fg_color = select_color((const char *) mgtoken[++i]);
                                                  }
                                                  if (!strcmp((const char *) mgtoken[i], "fgcolor1")) {
                                                    fg_color1 = select_color((const char *) mgtoken[++i]);
                                                  }
                                                  if (!strcmp((const char *) mgtoken[i], "fgcolor2")) {
                                                    fg_color2 = select_color((const char *) mgtoken[++i]);
                                                  }
                                                  if (!strcmp((const char *) mgtoken[i], "section")) {
                                                    section.a = atof(mgtoken[++i]);
                                                    section.b = atof(mgtoken[++i]);
                                                    section.c = atof(mgtoken[++i]);
                                                    section.d = atof(mgtoken[++i]);
                                                  }
                                                  if (!strcmp((const char *) mgtoken[i], "dsec")) {
                                                    dist_section = atof(mgtoken[++i]);
                                                  }
                                                  i++;
      }
    }
    
    fclose(rcfile);
    
    fprintf(stdout, "done\n");
  } else {
    fprintf(stderr, "Warning: cannot find the file 'mgconf'\n");
  }
  
}

/* Convert a string to a color */
couleur select_color(const char *col)
{
  couleur         retcol = fg_color;
  
  if ((!strcmp(col, "bleu"))  || (!strcmp(col, "blue")))
    retcol = mgbleu;
  if ((!strcmp(col, "blanc")) || (!strcmp(col, "white")))
    retcol = mgblanc;
  if ((!strcmp(col, "vert"))  || (!strcmp(col, "green")))
    retcol = mgvert;
  if ((!strcmp(col, "noir"))  || (!strcmp(col, "black")))
    retcol = mgnoir;
  if ((!strcmp(col, "jaune")) || (!strcmp(col, "yellow")))
    retcol = mgjaune;
  if ((!strcmp(col, "rouge")) || (!strcmp(col, "red")))
    retcol = mgrouge;
  if (!strcmp(col, "fun"))
    retcol = mgfun;
  
  return retcol;
}

void processDialogF1()
{
  if (!strcmp(dialArea[2].state, "SELECTED"))
    afficheRepere = MG_TRUE;
  else
    afficheRepere = MG_FALSE;
  
  if (!strcmp(dialArea[3].state, "SELECTED"))
    afftime = MG_TRUE;
  else
    afftime = MG_FALSE;
  
  if (!strcmp(dialArea[4].state, "SELECTED"))
    orient = MG_TRUE;
  else
    orient = MG_FALSE;
  
  if (!strcmp(dialArea[5].state, "SELECTED"))
    bodies_numbers = MG_TRUE;
  else
    bodies_numbers = MG_FALSE;
  
  if (!strcmp(dialArea[6].state, "SELECTED"))
    affFuncname = MG_TRUE;
  else
    affFuncname = MG_FALSE;
  
  if (!strcmp(dialArea[7].state, "SELECTED"))
    dynamic_scale = MG_TRUE;
  else
    dynamic_scale = MG_FALSE;  
}

void processDialogF2()
{
  double val;
  
  val = atof(dialArea[3].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color2.r = val;
  val = atof(dialArea[4].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color2.v = val;
  val = atof(dialArea[5].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color2.b = val;
  
  val = atof(dialArea[7].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color1.r = val;
  val = atof(dialArea[8].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color1.v = val;
  val = atof(dialArea[9].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color1.b = val;
  
  val = atof(dialArea[11].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color.r = val;
  val = atof(dialArea[12].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color.v = val;
  val = atof(dialArea[13].label);
  if ((val >= 0.) && (val <= 1.))
    fg_color.b = val;
  
  val = atof(dialArea[15].label);
  if ((val >= 0.) && (val <= 1.))
    bg_color.r = val;
  val = atof(dialArea[16].label);
  if ((val >= 0.) && (val <= 1.))
    bg_color.v = val;
  val = atof(dialArea[17].label);
  if ((val >= 0.) && (val <= 1.))
    bg_color.b = val;
}


void processDialogF3()
{
  int val;
  
  val = atoi(dialArea[5].label);
  if ((val > 0) && (val < 100))
    nb_section1 = val;
  
  val = atoi(dialArea[3].label);
  if ((val > 0) && (val < 50))
    nb_section2 = val;
  
  val = atoi(dialArea[7].label);
  if ((val >= 1) && (val <= 3))
    modDirCon = val;
}

void processDialogF4()
{
  double val;
  
  val = atof(dialArea[3].label);
  if ((val > 0.) && (val < 1000.))
    Fcutlevel = val;
  
}

void processDialogF5()
{
  double val;
  int c;
  
  for (c=0;c<nbcolgrp;++c)
  {
    if (!strcmp(dialArea[2+c*4].state, "SELECTED"))
      colIsShown[c] = MG_TRUE;
    else
      colIsShown[c] = MG_FALSE;
    
    val = atof(dialArea[3+c*4].label);
    if ((val >= 0.) && (val <= 1.))
      rcolor[c] = val;
    val = atof(dialArea[4+c*4].label);
    if ((val >= 0.) && (val <= 1.))
      gcolor[c] = val;
    val = atof(dialArea[5+c*4].label);
    if ((val >= 0.) && (val <= 1.))
      bcolor[c] = val;
  }
}

void specialKey(int touche, int x, int y)
{
  char            str[40];
  int             c;
  
  switch (touche) {
    case GLUT_KEY_F1:
      
      dialogMode();
      
      openDialog(20, 50, 300, 180);

#ifdef _FR      
      creatCheckBox(30, 55,  "Repere global", afficheRepere);
      creatCheckBox(30, 75,  "Affichage State et Time", afftime);
      creatCheckBox(30, 95,  "Orientation des grains", orient);
      creatCheckBox(30, 115, "Numero des corps", bodies_numbers);
      creatCheckBox(30, 135, "Affichage Noms fonctions", affFuncname);
      creatCheckBox(30, 155, "Echelle adaptable", dynamic_scale);    
#else
      creatCheckBox(30, 55,  "Global Frame", afficheRepere);
      creatCheckBox(30, 75,  "Display State and Time", afftime);
      creatCheckBox(30, 95,  "Particle Orientation", orient);
      creatCheckBox(30, 115, "Body Numbers", bodies_numbers);
      creatCheckBox(30, 135, "Display Names of Functions", affFuncname);
      creatCheckBox(30, 155, "Dynamic Color-Scale", dynamic_scale);
#endif
      processDialog = processDialogF1;
      
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_F2:
      
      dialogMode();
      
      openDialog(10, 50, 390, 210);
      
      creatLabel(20, 70, "fg_color2 :");
      sprintf(str, "%1.2f", fg_color2.r);
      creatGetText(20, 55, (const char *) str);
      sprintf(str, "%1.2f", fg_color2.v);
      creatGetText(120, 55, (const char *) str);
      sprintf(str, "%1.2f", fg_color2.b);
      creatGetText(220, 55, (const char *) str);
      
      creatLabel(20, 110, "fg_color1 :");
      sprintf(str, "%1.2f", fg_color1.r);
      creatGetText(20, 95, (const char *) str);
      sprintf(str, "%1.2f", fg_color1.v);
      creatGetText(120, 95, (const char *) str);
      sprintf(str, "%1.2f", fg_color1.b);
      creatGetText(220, 95, (const char *) str);
      
      creatLabel(20, 150, "fg_color :");
      sprintf(str, "%1.2f", fg_color.r);
      creatGetText(20, 135, (const char *) str);
      sprintf(str, "%1.2f", fg_color.v);
      creatGetText(120, 135, (const char *) str);
      sprintf(str, "%1.2f", fg_color.b);
      creatGetText(220, 135, (const char *) str);
      
      creatLabel(20, 190, "bg_color :");
      sprintf(str, "%1.2f", bg_color.r);
      creatGetText(20, 175, (const char *) str);
      sprintf(str, "%1.2f", bg_color.v);
      creatGetText(120, 175, (const char *) str);
      sprintf(str, "%1.2f", bg_color.b);
      creatGetText(220, 175, (const char *) str);
      
      processDialog = processDialogF2;
      
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_F3:
      dialogMode();
      
      openDialog(10, 50, 390, 210);
      
      creatLabel(20, 70, "nb_section2 :");
      sprintf(str, "%d", nb_section2);
      creatGetText(20, 55, (const char *) str);
      
      creatLabel(20, 110, "nb_section1 :");
      sprintf(str, "%d", nb_section1);
      creatGetText(20, 95, (const char *) str);
      
      creatLabel(20, 150, "mode :");
      sprintf(str, "%d", modDirCon);
      creatGetText(20, 135, (const char *) str);
      
      processDialog = processDialogF3;
      
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_F4:
      dialogMode();
      
      openDialog(10, 50, 390, 210);
      
      creatLabel(20, 70, "normal force threshold:");
      sprintf(str, "%lg", Fcutlevel);
      creatGetText(20, 55, (const char *) str);
      
      /* 		creatLabel(20, 110, "nb_section1 :"); */
      /* 		sprintf(str, "%d", nb_section1); */
      /* 		creatGetText(20, 95, (const char *) str); */
      
      /* 		creatCheckBox(20, 150, "Diag. Pol. [X]contacts ; [ ]forces", modDirCon); */
      
      processDialog = processDialogF4;
      
      glutPostRedisplay();
      break;
 
    case GLUT_KEY_F5:
       
      dialogMode();   
      openDialog(10, 50, 390, 20+nbcolgrp*40+40);
    
      for(c=0;c<nbcolgrp;++c)
      {
        sprintf(str,"%s",colorName[c]);
        creatCheckBox(20, 70+c*40, (const char *) str, colIsShown[c]);
        sprintf(str, "%1.2f", rcolor[c]);
        creatGetText(20, 55+c*40, (const char *) str);
        sprintf(str, "%1.2f", gcolor[c]);
        creatGetText(120, 55+c*40, (const char *) str);
        sprintf(str, "%1.2f", bcolor[c]);
        creatGetText(220, 55+c*40, (const char *) str);
      }
               
               
      processDialog = processDialogF5;
      glutPostRedisplay();  
    
      break;
    
    case GLUT_KEY_F12:
      
      sectionActive = 1 - sectionActive;
      
      if (sectionActive)
        strcpy(quick_text, "Section enable");
      else
        strcpy(quick_text, "Section disable");
      
      disp_only_text = MG_TRUE;
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_LEFT:
      
      dist_section -= r_moy;
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_RIGHT:
      
      dist_section += r_moy;
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_UP:
      
      section.d += r_moy;
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_DOWN:
      
      section.d -= r_moy;
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_PAGE_DOWN:
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      cut_distance -= inc_cut;
      gluPerspective(45.0, 1.0, cut_distance, DISTANCE_MAX);
      glMatrixMode(GL_MODELVIEW);
      glutPostRedisplay();
      break;
      
    case GLUT_KEY_PAGE_UP:
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      cut_distance += inc_cut;
      gluPerspective(45.0, 1.0, cut_distance, DISTANCE_MAX);
      glMatrixMode(GL_MODELVIEW);
      glutPostRedisplay();
      break;
      
    default:
      break;
  }
  mgterminal = GL_TERMINAL;
}

/* keyboard */
void clavier(unsigned char touche, int ix, int iy)
{
  
  switch (touche) {
    case 'g':	/* erase graphic */
      
      affgraphic = 0;
      glutPostRedisplay();
      break;
      
      case 'q':		/* quit */
      
        mgpfree();
        exit(EXIT_SUCCESS);
      
        case 'r':		/* global frame ON/OFF */
      
          afficheRepere = 1 - afficheRepere;
          glutPostRedisplay();
          break;
      
          case 'v':		/* polyhedron shadding ON/OFF */
      
            shade_polye = 1 - shade_polye;
            glutPostRedisplay();
            break;
      
            case 't':		/* display time ON/OFF */
      
              afftime = 1 - afftime;
              glutPostRedisplay();
              break;
      
    case 'T':
      
      modeTrace = 1 - modeTrace;
      break;
      
      case 'c': /* swap color gradient between links and bodies */
      
        if (affgradcolor != affgradlinkcolor) {
          affgradcolor = 1 - affgradcolor;
          affgradlinkcolor = 1 - affgradcolor;
        } else
          affgradcolor = MG_TRUE;
          glutPostRedisplay();
          break;
      
          case 'o':		/* angular position of bodies ON/OFF */
      
            orient = 1 - orient;
            glutPostRedisplay();
            break;
      
            case 'n':		/* Bodies numbers */
      
              bodies_numbers = 1 - bodies_numbers;
              glutPostRedisplay();
              break;
      
              case 'p':		/* bodies positions */
      
                strcpy(funcname, "Body positions");
                rendu = disp_points;
                disp_current = rendu;
                affgradlinkcolor = MG_FALSE;
                glutPostRedisplay();
                break;
      
                case 'e':		/* boundaries */
      
                  strcpy(funcname, "Boundaries");
                  rendu = disp_boundaries;
                  disp_current = rendu;
                  glutPostRedisplay();
                  break;
      
                  case ' ':		/* Stop animation */
      
                    glutIdleFunc(NULL);
                    play_again = MG_FALSE;
                    sauve_anim = MG_FALSE;
                    break;
      
    case 's':
      
      antialiased = 1 - antialiased;
      glutPostRedisplay();
      break;
  
    case 'x':
    
      dynamic_scale = 1 - dynamic_scale;
      glutPostRedisplay();
      if (dynamic_scale)
        fprintf (stdout,"dynamic scale ON\n");
      else 
        fprintf (stdout,"dynamic scale OFF\n");
      break;
    
    case 'z':
      
      strcpy(funcname, "Obstacles");
      rendu = disp_obstacles;
      disp_current = rendu;
      glutPostRedisplay();
      break;
      
      case 'd':		/* Bodies shapes */
      
        strcpy(funcname, "Body shapes");
        rendu = disp_shapes;
        disp_current = rendu;
        affgradcolor = MG_FALSE;
        glutPostRedisplay();
        break;
      
    case 'f':
      
      v_adi -= 10.0;
      if (v_adi <= 0.0) v_adi = 1.;
      glutPostRedisplay();
      break;
      
    case 'F':
      
      v_adi += 10.0;
      glutPostRedisplay();
      break;
      
      case '!': /*** Sorties diverses ***/
      
	  /*
        strcpy(funcname, "#DEV TEST#     ");
        rendu = disp_quad_strain;
        if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
        glutPostRedisplay();
          */
      
      {
        int i,ii,k,anta;
        double px,py,pz;
        double unx,uny,unz,invnorm;
        FILE *out;
      
        out=fopen("PointForce","w");
      
        k=0;
        for (i = 0 ; i < nbel ; i++)
          for (ii = 0; ii < nbneighbors[i][state]; ii++)
        {
          anta = neighbor[k][state] - 1;
          
          if(Fn[k][state] != 0)
          {
            unx = x[i][state] - x[anta][state];
            uny = y[i][state] - y[anta][state];
            unz = z[i][state] - z[anta][state];
            invnorm=1./sqrt(unx*unx + uny*uny + unz*unz);
            
            px = x[i][state]-radius[i][state] * unx*invnorm;
            py = y[i][state]-radius[i][state] * uny*invnorm;
            pz = z[i][state]-radius[i][state] * unz*invnorm;
            
            fprintf(out,"%lg %lg %lg %lg\n",px,py,pz,
                    Fn[k][state]);
          }
          
          k++;
        }
          
        fclose(out);
      
      }
      
      break;
      
    case 'w':
      
      sauve_anim = MG_TRUE;
      
      fprintf(stdout, "Pictures saved in aniXXXX.tif\n");
      fprintf(stdout, "Format: %dx%d\n", W, H);
      if (multifiles) {
        play_again = MG_TRUE;
        glutIdleFunc(play_filetofile);
      } else
        glutIdleFunc(play);
        break;
      
        case 'h':		/* help */
      
          affiche_aide();
          break;
      
          case 'i':		/* some informations */
      
            if ((selectedBody < 0) || (selectedBody > nbel))
              affiche_infos();
            else
              display_infos_on_body(selectedBody);
            break;
      
    case '/':
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      cut_distance = distance;
      gluPerspective(45.0, 1.0, cut_distance, DISTANCE_MAX);
      glMatrixMode(GL_MODELVIEW);
      glutPostRedisplay();
      break;
      
    case '*':
      
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      cut_distance = 0.1;
      gluPerspective(45.0, 1.0, cut_distance, DISTANCE_MAX);
      glMatrixMode(GL_MODELVIEW);
      glutPostRedisplay();
      break;
      
      case '0':		/* initial state */
      
        state = 0;
      
        if (multifiles) {
          nfile = 0;
          next_state();
        }
        
        findBoundaries(state, &xminB, &yminB, &zminB,
                       &xmaxB, &ymaxB, &zmaxB);
      
        glutPostRedisplay();
        break;
      
        case '-':		/* previous state */
      
          if ((multifiles) && (nfile > 0))
            previous_state();
      
          if ((state > 0) && (!multifiles))
            state -= 1;
        
          findBoundaries(state, &xminB, &yminB, &zminB,
                         &xmaxB, &ymaxB, &zmaxB);
      
          glutPostRedisplay();
          break;
      
          case '+':		/* next state */
      
            if (multifiles)
              next_state();
      
            if ((state < nb_state - 1) && (!multifiles))
              state += 1;
        
            findBoundaries(state, &xminB, &yminB, &zminB,
                           &xmaxB, &ymaxB, &zmaxB);
      
            glutPostRedisplay();
            break;
      
            case '2':		/* 2D mode is forced */
      
              mode2D = MG_TRUE;
              /*glDisable(GL_DEPTH_TEST);*/
              phi = -90;
              theta = 0;
              glutPostRedisplay();
              break;
      
              case '3':		/* 3D mode is forced */
      
                mode2D = MG_FALSE;
                glEnable(GL_DEPTH_TEST);
                glutPostRedisplay();
                break;
      
    default:
      break;
  }
  mgterminal = GL_TERMINAL;
}

void traitmenu(int value)
{
  
  switch (value) 
  {
    case 0:
      
      mgpfree();
      exit(EXIT_SUCCESS);
      break;
      
    case 1:	
        
      phi = 0;
      theta = 0;
      glutPostRedisplay();
      break;
        
    case 2:	
          
      phi = -90;
      theta = 0;
      glutPostRedisplay();
      break;
             
    case 3:
                           
      phi = PHI_INIT;
      theta = THETA_INIT;
      glutPostRedisplay();
      break;
                
    case 4:		   
                        
      param_init();
      glutReshapeWindow(W, H);
      glutPostRedisplay();
      break;
                
    case 5:		    
                 
      if (multifiles) 
      {
        play_again = MG_TRUE;
        glutIdleFunc(play_filetofile);
      } else 
      { glutIdleFunc(play); }
      break;
                
    case 6:            
    {
      char            command[256];
      if (strcmp(mgpost_editor, "")) {
                  
#ifdef _MACOSX
        /* TODO test it */
        strcpy(command, "open -a ");	
        strcat(command, (const char *) mgpost_editor);
        strcat(command, " mgconf");
#endif
                         
#ifdef _WINDOWS
        /* TODO test it */
        strcpy(command, "$MGPOST_EDITOR mgconf"); 
#endif
                                
#ifdef _LINUX
        strcpy(command, "$MGPOST_EDITOR ./mgconf &");
#endif
                                
        system(command);
      } else fprintf(stdout, "You must define the environment variable MGPOST_EDITOR !\n");
    }
    break;

    case 7:
  
      previous_state();
      break;
  
    case 8:
  
      next_state();
      break;
  
    case 9:
  
      save_mgpview();
      break;
  
    case 10:
  
      load_mgpview();
      glutReshapeWindow(W, H);
      break;
  
    default:
      break;
  }
}

void traitsubmenu1(int value)
{
  
  switch (value) {
    case 0:
      
      bg_color = mgblanc;
      glutPostRedisplay();
      glClearColor(bg_color.r, bg_color.v, bg_color.b, 0.0);
      break;
      
    case 1:
      
      bg_color = mgfun;
      glutPostRedisplay();
      glClearColor(bg_color.r, bg_color.v, bg_color.b, 0.0);
      break;
      
    case 2:
      
      bg_color = mgnoir;
      glutPostRedisplay();
      glClearColor(bg_color.r, bg_color.v, bg_color.b, 0.0);
      break;
      
    case 3:
      
      if (alpha_color == 1.0)
        alpha_color = 0.3;
      else
        alpha_color = 1.0;
      break;
      
    case 4:
      
      sectionActive = 1 - sectionActive;
      break;
      
    case 5:
      
      fg_color = mgblanc;
      glutPostRedisplay();
      break;
      
    case 6:
      
      fg_color = mgbleu;
      glutPostRedisplay();
      break;
      
    case 7:
      
      fg_color = mgnoir;
      glutPostRedisplay();
      break;
      
    default:
      break;
  }
}

void traitsubmenu2 (int value)
{
  int             i;
  FILE           *outfile;
  
  switch (value) {
    case 0:
      
      strcpy(supfuncname, "None");
      rendu_sup = NULL;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 1:
      
      strcpy(supfuncname, "Visibility Network");
      rendu_sup = disp_network;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 2:
      
      strcpy(funcname, "Squared View");
      rendu = disp_discrim;
      disp_current = rendu;
      break;
    
    case 3:
      
      strcpy(supfuncname, "Polyhedron forces");
      rendu_sup = disp_special_forces_3d;
      /*rendu_sup = disp_special_forces_lines_3d;*/
      if (!affgradcolor) affgradlinkcolor = MG_TRUE;
      break;
      
    case 4:
      
      strcpy(supfuncname, "Tangential forces");
      sep_networks = MG_FALSE;
      
      if (mode2D)
        rendu_sup = disp_tangential_strength_2d;
      else
        rendu_sup = disp_tangential_strength_3d;
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;
      
    case 5:
      
      strcpy(supfuncname, "Normal Positive forces");
      sep_networks = MG_FALSE;
      
      if (mode2D)
        rendu_sup = disp_positive_normal_strength_2d;
      else
      {
        if (more_forces)
          rendu_sup = disp_positive_normal_strength_3d_v2; 
        else
          rendu_sup = disp_positive_normal_strength_3d;
      }
        
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;
      
    case 6:
      
      strcpy(supfuncname, "Normal Negative forces");
      sep_networks = MG_FALSE;
      
      if (mode2D)
        rendu_sup = disp_negative_normal_strength_2d;
      else
        rendu_sup = disp_negative_normal_strength_3d;
      
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;
    
    case 7:
    
      strcpy(supfuncname, "Compressive-Tensile forces");
    
      if (mode2D)
      {
        rendu_sup = disp_tensile_compressive_forces_2d;
        glEnable(GL_DEPTH_TEST);
      }
      else
        rendu_sup = disp_networks_pos_neg;
    
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;   
      
    case 8:
      
      strcpy(funcname, "Angular Velocities");
      rendu = disp_angular_vlocy_2d;
      disp_current = rendu;
      affgradcolor = MG_TRUE;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 9:
      
      strcpy(supfuncname, "Sticked Links");
      rendu_sup = disp_stick_links;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 10:
      
      strcpy(funcname, "None");
      rendu = NULL;
      disp_current = rendu;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 11:
      
      strcpy(funcname, "Velocity Magnitudes");
      rendu = disp_vlocy_magnitude;
      disp_current = rendu;
      affgradcolor = MG_TRUE;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 12:
      
      strcpy(supfuncname, "Velocity Field");
      rendu_sup = disp_vlocy_field;
      disp_current = rendu;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 13:
      
      strcpy(funcname, "Particle Outlines");
      rendu = disp_outline;
      disp_current = rendu;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 14:
      
      affgraphic = MG_TRUE;
      affgraph_func =plot_hist_fn;
      break;
      
    case 15:
      
      affiche_infos();
      break;
      
    case 16:
      
      affgraphic = MG_TRUE;      
      affgraph_func = plot_fn_vs_ft;
      break;
      
    case 17:
      
      strcpy(supfuncname, "Shear Velocities");
      rendu_sup = disp_shear_vlocy_2d;
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;
      
    case 18:
      
      tool_surf_vol();
      break;
    
    case 19:
    
      strcpy(funcname, "Family Colors");
      rendu = disp_colors;
      disp_current = rendu;
      affgradlinkcolor = MG_FALSE;
      break;
    
    case 20:
      
      strcpy(supfuncname, "Color-lines");
      rendu_sup = disp_force_colorlines;
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;
    
    case 21:
      
      strcpy(funcname, "Distance from Origine");
      rendu = disp_dist_ref;
      disp_current = rendu;
      affgradcolor = MG_TRUE;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 22:
      
      tool_anisotropy(state, 0, NULL);
      break;
      
    case 23:
      
      /*tool_contacts_direction();*/
      break;
      
    case 24:
      
      outfile = fopen("anisotropy.dat", "w");
      
      if (!multifiles)
        for (i = 0; i < nb_state; i++)
          tool_anisotropy(i, 1, outfile);
      else {
        int             save_nfile;
        
        save_nfile = nfile;
        
        nfile = 1;
        sprintf((char *) numfile, num_file_format, nfile);
        strcpy((char *) datanewfilename, "mgp.out.");
        strcat((char *) datanewfilename, (const char *) numfile);
        
        if (fgziped)
          strcat((char *) datanewfilename, ".gz");
        
        while (!access((const char *) datanewfilename, F_OK)) {
          strcpy(datafilename, (const char *) datanewfilename);
          nullifyCumulatedTabs();
          charger_geometrie();
          
          tool_anisotropy(0, 1, outfile);
          
          nfile++;
          sprintf((char *) numfile, num_file_format, nfile);
          strcpy((char *) datanewfilename, "mgp.out.");
          strcat((char *) datanewfilename, (const char *) numfile);
          if (fgziped)
            strcat((char *) datanewfilename, ".gz");
        }
        
        nfile = save_nfile - 1;
        next_state();
      }
        
      fclose(outfile);
      break;
      
    case 25:
      
      strcpy(supfuncname, "Torques");
      
      if (mode2D)
        rendu_sup = disp_torque_2d;
        
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
          
      break;
      
    case 26:
      
      info_gap();
      break;
    
    case 27:
    
      affgraphic = MG_TRUE;      
      affgraph_func = plot_granulo;   
      break;

    case 28:
      strcpy(supfuncname, "Cracks");
      rendu_sup = disp_cracks;
      affgradlinkcolor = MG_FALSE;
      break; 
     
    case 29:
      
      strcpy(supfuncname, "Local Liquid Volume");
      rendu_sup = disp_Vliq_3d;
      
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;
      
    case 30:
      
      affgraphic = MG_TRUE;
      
      if (mode2D)
        affgraph_func = plot_distri_TCT;
      else {
        if (modDirCon == 1) 
          affgraph_func = plot_hist_ny;
        else if (modDirCon == 2)
          affgraph_func = plot_ncloud;
        else if (modDirCon == 3)
          affgraph_func = plot_distri_TCT_3d;			
      }
      break;
      
    case 31:
      
      strcpy(supfuncname, "Separate Networks");
      sep_networks = MG_TRUE;
      
      if (mode2D)
        rendu_sup = disp_positive_normal_strength_2d;
      else
        rendu_sup = disp_positive_normal_strength_3d;
      
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
      break;
      
    case 32:
      
      strcpy(funcname, "Coordination Number");
      rendu = disp_coord_number;
      disp_current = rendu;
      affgradcolor = MG_TRUE;
      affgradlinkcolor = MG_FALSE;
      break;
      
    case 33:
      
      strcpy(supfuncname, "Forces");
      
      if (mode2D)
        rendu_sup = disp_strength_3d;	/* !!!! */
      else
        rendu_sup = disp_strength_3d;
      
      if (!affgradcolor)
        affgradlinkcolor = MG_TRUE;
        
      break;
    
    
    case 34:
    
      strcpy(funcname, "Geolayers");
      rendu = disp_geo_layers;
      disp_current = rendu;
      affgradcolor = MG_TRUE;
      affgradlinkcolor = MG_FALSE;
      break;
    
    case 35:
      
    {
      int i,k,l;
      FILE *f;
      
      f=fopen("forces.dat","w");
      
      k=0;
      for (i=0;i<nbel;i++)
        for (l=0;l<nbneighbors[i][state];l++)
      {
        if(Fn[k][state]!=0.0)
          fprintf(f,"%lg %lg %lg %lg\n",Fn[k][state],Ft[k][state],Fs[k][state],
                  sqrt(Ft[k][state]*Ft[k][state]+Fs[k][state]*Fs[k][state]));
        k++;
      }
          
      fclose(f);
    }
      
    break;
    
    case 36:
    
      strcpy(funcname, "Sizes");
      rendu = disp_sizes;
      disp_current = rendu;
      affgradcolor = MG_TRUE;
      affgradlinkcolor = MG_FALSE;
      break;
      
    default:
      break;
  }
  glutPostRedisplay();
  mgterminal = GL_TERMINAL;
}

void 
    traitsubmenu4(int value)
{
  
  switch (value) {
    
#ifdef _WITH_TIFF
    case 0:
    {  
      char fname[40];

      sprintf(fname,"shot%d.tif",++screenshot_counter);
      glutPostRedisplay();
      fprintf(stdout, "Picture saved in '%s'\n", fname);
      fprintf(stdout, "Format: %dx%d\n", W, H);
      writetiff(fname, mgpost_string, 0, 0, W, H, COMPRESSION_PACKBITS);
    }
    break;	  
#endif
                  
#ifdef _WITH_PNG
    case 9:
    {  
      char fname[40];

      sprintf(fname,"shot%d.png",++screenshot_counter);
      glutPostRedisplay();
      fprintf(stdout, "Picture saved in '%s'\n", fname);
      fprintf(stdout, "Format: %dx%d\n", W, H);
      writepng(fname, mgpost_string, 0, 0, W, H);
    }
    break;
#endif
      
    case 1:
      
      sauve_anim = MG_TRUE;
      
      fprintf(stdout, "Pictures saved in 'aniXXXX.tif'\n");
      fprintf(stdout, "Format: %dx%d\n", W, H);
      
      if (multifiles) {
        play_again = MG_TRUE;
        glutIdleFunc(play_filetofile);
      } else
        glutIdleFunc(play);
      
        break;
      
    case 2:
      
    {
      GLint           buffsize = 0, gl2ps_state = GL2PS_OVERFLOW;
      GLint           viewport[4];
      
      psfile = fopen("./shot.eps", "w");
      if (!psfile) {
        printf("Unable to open file 'shot.eps' for writing\n");
        exit(EXIT_FAILURE);
      }
      glGetIntegerv(GL_VIEWPORT, viewport);
      
      fprintf(stdout, "Exporting EPS... ");
      fflush(stdout);
      
      mgterminal = GL2PS_TERMINAL;
      
      while (gl2ps_state == GL2PS_OVERFLOW) {
        buffsize += 1024 * 1024;
        
        gl2psBeginPage(datatitre, mgpost_string, viewport,
                       GL2PS_EPS, GL2PS_BSP_SORT, GL2PS_SILENT |
                       GL2PS_OCCLUSION_CULL | GL2PS_BEST_ROOT,
                       GL_RGBA, 0, NULL, 0, 0, 0, buffsize,
                       psfile, NULL);
        
        if (affgraphic) {
          affgraph_func();
        } else {
          if (rendu != NULL)
            rendu();
          
          if (rendu_sup != NULL)
            rendu_sup();
        }
        
        gl2ps_state = gl2psEndPage();
      }
      fprintf(stdout, "done\n");
      fclose(psfile);
      
      mgterminal = GL_TERMINAL;
      
      fprintf(stdout, "Picture saved in 'shot.eps'\n");
    }
    break;
      
    case 3:
      
      fprintf(stderr, "pas disponible dans cette version !!\n");
      break;
      
    case 4:
      
      exportCIN();
      fprintf(stdout, "File 'export.cin' created\n");
      break;
      
    case 5:
      
      povfile = fopen("./shot.pov", "w");
      mgterminal = POV_TERMINAL;
      fprintf(povfile, "#include \"colors.inc\"\n");
      fprintf(povfile, "#include \"stones.inc\"\n\n");
      
      /*
      * MEMO pour le moment c'est pas top mais �a fonctionne... il
      * faudrait le reecrire plus proprement !
      */
      fprintf(povfile, "camera {\n  location <%g, %g, %g>\n",
              distance * cos((double) (90 - phi) * MG_DEG2RAD) * cos((double) (theta + 90) * MG_DEG2RAD) / adim,
              distance * sin((double) (90 - phi) * MG_DEG2RAD) / adim,
              -(distance * cos((double) (90 - phi) * MG_DEG2RAD) * sin((double) (theta + 90) * MG_DEG2RAD) / adim));
      
      fprintf(povfile, "  look_at <0, 0, 0>\n}\n\n");
      fprintf(povfile, "background { rgb <%g, %g, %g> }\n", bg_color.r, bg_color.v, bg_color.b);
      fprintf(povfile, "light_source { <%g, %g, %g> White }\n\n", L0pos[0] / adim, L0pos[1] / adim, -L0pos[2] / adim);
      fprintf(povfile, "light_source { <%g, %g, %g> White }\n\n", L1pos[0] / adim, L1pos[1] / adim, -L1pos[2] / adim);
      fprintf(povfile, "union {\n");
      
      rendu();
      
      fprintf(povfile, "texture {T_Stone8} // For Instance\n");
      fprintf(povfile, "}\n");
      
      fclose(povfile);
      mgterminal = GL_TERMINAL;
      
      
      fprintf(stdout, "Picture saved in 'shot.pov'\n");
      break;
      
    case 6:
      
      psfile = fopen("./shot.eps", "w");
      
      mgterminal = PS_TERMINAL;
      
      fprintf(psfile, "%%!PS-Adobe-3.0 EPSF-3.0\n");
      
      psZoom = (psdim * 29.5276) / (xmaxB - xminB);
      fprintf(psfile, "%%%%BoundingBox: 0 0 %d %d\n", (int) ((xmaxB - xminB) * psZoom) + 4,
              (int) ((ymaxB - yminB) * psZoom) + 4);
      
      fprintf(psfile, "%%%%Creator: %s\n", mgpost_string);
      fprintf(psfile, "%%%%Title: %s\n", datatitre);
      
      rendu();
      
      if (rendu_sup != NULL)
        rendu_sup();
        
      fclose(psfile);
      
      mgterminal = GL_TERMINAL;
      
      fprintf(stdout, "Picture saved in 'shot.eps'\n");
      break;
      
    case 7:
    
      export_Rxy();
      fprintf(stdout, "File 'particles.Rxy' created\n");
      break;     
      
    case 8:
    
      export_SSPX();
      fprintf(stdout, "File 'deplacements.txt' created\n");
      break; 
    
    default:
      break;
  }
}

void traitsubmenuFluid (int value)
{
  switch (value) 
  {
    case 0:
      
      rendu_fluid = NULL;
      break;
      
    case 1:
      
      rendu_fluid = disp_fluid_ux;
      break;
      
    case 2:
      
      rendu_fluid = disp_fluid_uy;
      break;
      
    case 3:
      
      rendu_fluid = disp_fluid_normeu;
      break;
      
    case 4:
      
      rendu_fluid = disp_fluid_p;
      break;

    default:
      break;
  }
}

void 
    processHits(GLint hits, GLuint buffer[])
{
  GLuint          choose = -1;
  
  if (hits > 0) {
    GLuint          depth = buffer[1];
    int             loop;
    
    choose = buffer[3];
    
    for (loop = 1; loop < hits; loop++) {
      
      if (buffer[loop * 4 + 1] < depth) {
        choose = buffer[loop * 4 + 3];
        depth = buffer[loop * 4 + 1];
      }
    }
  }
  selectedBody = (int) choose;
}

void 
    souris(int bouton, int state, int x, int y)
{
  GLuint          selectBuf[BUFSIZE];
  GLint           hits;
  GLint           viewport[4];
  
  modifiers = glutGetModifiers();
  
  if (modifiers == GLUT_ACTIVE_CTRL) {
    GLint nb_subdiv_sphere_backup = nb_subdiv_sphere;
    GLint nb_subdiv_sphere_2_backup = nb_subdiv_sphere_2;
    
    if (bouton != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
      return;
    
    glEnable(GL_DEPTH_TEST);
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    glSelectBuffer(BUFSIZE, selectBuf);
    (void) glRenderMode(GL_SELECT);
    renderMode = GL_SELECT;
    nb_subdiv_sphere = 4;
    nb_subdiv_sphere_2 = 3;
    
    glInitNames();
    glPushName(0);
    
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    
    gluPickMatrix((GLdouble) x, (GLdouble) (H - y), 4.0, 4.0, viewport);
    gluPerspective(45.0, 1.0, 0.1, DISTANCE_MAX);
    
    glMatrixMode(GL_MODELVIEW);
    rendu();
    
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    nb_subdiv_sphere = nb_subdiv_sphere_backup;
    nb_subdiv_sphere_2 = nb_subdiv_sphere_2_backup;
    
    hits = glRenderMode(GL_RENDER);
    renderMode = GL_RENDER;
    
    processHits(hits, selectBuf);
    
    glMatrixMode(GL_MODELVIEW);
    glutPostRedisplay();
  } else {
    if (bouton == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      rendu = disp_boundaries;
      b_gauche = 1;
      xprec = x;
      yprec = y;
    }
    if (bouton == GLUT_LEFT_BUTTON && state == GLUT_UP) {
      rendu = disp_current;
      glutPostRedisplay();
      b_gauche = 0;
    }
    if (bouton == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
      rendu = disp_boundaries;
      b_middle = 1;
      yprec = y;
    }
    if (bouton == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
      rendu = disp_current;
      glutPostRedisplay();
      b_middle = 0;
    }
  }
}

void 
    mouvement(int x, int y)
{
  if (b_gauche && (modifiers != GLUT_ACTIVE_SHIFT) && !mode2D) {
    theta += x - xprec;
    if (theta >= 360)
      while (theta >= 360)
        theta -= 360;
    phi += y - yprec;
    if (phi < 0)
      while (phi < 0)
        phi += 360;
    
    glutPostRedisplay();
    xprec = x;
    yprec = y;
  }
  if (b_gauche && (modifiers == GLUT_ACTIVE_SHIFT)) {
    Xviewp += (x - xprec);
    Yviewp -= (y - yprec);
    xprec = x;
    yprec = y;
    glutPostRedisplay();
  }
  if (b_middle) {
    distance -= ((float) (y - yprec));
    if (distance < 1.0)
      distance = 1.0;
    if (distance > DISTANCE_MAX)
      distance = DISTANCE_MAX;
    glutPostRedisplay();
    yprec = y;
  }
}

void 
    affiche_aide()
{
  
  fprintf(stdout, "+-------------------------------------------------+\n");
  fprintf(stdout, "| RACCOURCIS CLAVIER                              |\n");
  fprintf(stdout, "+-------------------------------------------------+\n");
  fprintf(stdout, "| ' ' : stop une animation                        |\n");
  fprintf(stdout, "| 'a' :                                           |\n");
  fprintf(stdout, "| 'b' :                                           |\n");
  fprintf(stdout, "| 'c' : bascule l'echelle de couleur entre corps  |\n");
  fprintf(stdout, "|       et interactions                           |\n");
  fprintf(stdout, "| 'd' : affichage des formes des corps            |\n");
  fprintf(stdout, "| 'e' : n'affiche que l'encombrement              |\n");
  fprintf(stdout, "| 'f' : reduction de la longueur des  vecteurs    |\n");
  fprintf(stdout, "| 'g' :                                           |\n");
  fprintf(stdout, "| 'h' : cette aide                                |\n");
  fprintf(stdout, "| 'i' : informations                              |\n");
  fprintf(stdout, "| 'j' :                                           |\n");
  fprintf(stdout, "| 'k' :                                           |\n");
  fprintf(stdout, "| 'l' :                                           |\n");
  fprintf(stdout, "| 'm' :                                           |\n");
  fprintf(stdout, "| 'n' :                                           |\n");
  fprintf(stdout, "| 'o' : orientation des corps                     |\n");
  fprintf(stdout, "| 'p' : positions des corps (points)              |\n");
  fprintf(stdout, "| 'q' : quitter                                   |\n");
  fprintf(stdout, "| 'r' : affichage du repere on/off                |\n");
  fprintf(stdout, "| 's' :                                           |\n");
  fprintf(stdout, "| 't' : affichage du temps on/off                 |\n");
  fprintf(stdout, "| 'u' :                                           |\n");
  fprintf(stdout, "| 'v' : ombrage des polyedres on/off              |\n");
  fprintf(stdout, "| 'w' :                                           |\n");
  fprintf(stdout, "| 'x' :                                           |\n");
  fprintf(stdout, "| 'y' :                                           |\n");
  fprintf(stdout, "| 'z' :                                           |\n");
  fprintf(stdout, "| '/' : couper a (0.0.0) parallement a l'ecran    |\n");
  fprintf(stdout, "| '*' : annule '/'                                |\n");
  fprintf(stdout, "| '0' : etat initial                              |\n");
  fprintf(stdout, "| '2' : mode 2D                                   |\n");
  fprintf(stdout, "| '3' : mode 3D                                   |\n");
  fprintf(stdout, "+-------------------------------------------------+\n");
  
}

void 
    mgp_buildmenu()
{
  int popupmenu, submenu1, submenu2, submenu3, submenu4, submenu5, submenu6;
  
#ifdef _FR
  submenu1 = glutCreateMenu(traitsubmenu1);
  glutAddMenuEntry("Fond blanc", 0);
  glutAddMenuEntry("Fond fun", 1);
  glutAddMenuEntry("Fond Noir", 2);
  glutAddMenuEntry("Traits blanc", 5);
  glutAddMenuEntry("Traits bleu", 6);
  glutAddMenuEntry("Traits Noir", 7);
  glutAddMenuEntry("Transparence ON/OFF", 3);
  glutAddMenuEntry("Section ON/OFF", 4);
        
  submenu2 = glutCreateMenu(traitsubmenu2);
  glutAddMenuEntry("Informations sur l'echantillon", 15);
  glutAddMenuEntry("Quadrillage de l'echantillon", 2);
  if (with_layers) glutAddMenuEntry("Couches geologiques", 34);
  glutAddMenuEntry("Champ de vitesses", 12);
  glutAddMenuEntry("Anisotropie", 22);
  glutAddMenuEntry("Evolution Anisotropie", 24);
  glutAddMenuEntry("Informations sur les gaps", 26);
  glutAddMenuEntry("Direction des contacts", 30);
  glutAddMenuEntry("fn vs ft", 16);
  glutAddMenuEntry("pdf(fn)", 14);
  glutAddMenuEntry("Granulometrie", 27);
  glutAddMenuEntry("Forces locales", 35);
        
  submenu3 = glutCreateMenu(traitsubmenu2);
  glutAddMenuEntry("Rien", 10);
  glutAddMenuEntry("Nombre de coordination", 32);
  glutAddMenuEntry("Contour", 13);
  glutAddMenuEntry("Couleurs", 19);
  glutAddMenuEntry("Tailles", 36);
  glutAddMenuEntry("Vitesses angulaires (2D)", 8);
  glutAddMenuEntry("Norme vitesses", 11);
  glutAddMenuEntry("Distance / reference", 21);
        
  submenu4 = glutCreateMenu(traitsubmenu2);
  glutAddMenuEntry("Rien", 0);
  glutAddMenuEntry("Liste de Verlet", 1);
  glutAddMenuEntry("Liens colles", 9);
  glutAddMenuEntry("Fissures", 28);
  glutAddMenuEntry("Forces normales sup. seuil", 31);
  glutAddMenuEntry("Forces", 33);
  glutAddMenuEntry("Forces des fichiers for.out.xxx", 3);
  glutAddMenuEntry("lignes de couleur pour fn", 20);
  glutAddMenuEntry("Forces en compression et en traction", 7);
  glutAddMenuEntry("Forces normales positives", 5);
  glutAddMenuEntry("Forces normales negatives", 6);
  glutAddMenuEntry("Forces tangentielles (val. abs.)", 4);
  glutAddMenuEntry("Efforts de couple", 25);
  glutAddMenuEntry("Vitesse relative tangente", 17);
  glutAddMenuEntry("Volumes des ponts liquides", 29);
        
  submenu5 = glutCreateMenu(traitsubmenu4);
  glutAddMenuEntry("Image TIFF", 0);
  glutAddMenuEntry("Serie d'images(TIFF)", 1);
  glutAddMenuEntry("Image EPS avec gl2ps", 2);
  glutAddMenuEntry("Image EPS (2D)", 6);
  glutAddMenuEntry("Fichier 'BODIES.DAT'", 3);
  glutAddMenuEntry("Fichier 'export.cin'", 4);
  glutAddMenuEntry("Fichier 'shot.pov'", 5);
  glutAddMenuEntry("Fichier 'particles.Rxy'", 7);
  glutAddMenuEntry("Fichier 'deplacement.txt'", 8); 
        
  submenu6 = glutCreateMenu(traitsubmenuFluid);
  glutAddMenuEntry("Rien", 0);
  glutAddMenuEntry("Vitesses horizontales", 1);
  glutAddMenuEntry("Vitesses verticales", 2);
  glutAddMenuEntry("Norme vitesses", 3);
  glutAddMenuEntry("Pressions", 4);
        
  popupmenu = glutCreateMenu(traitmenu);
  glutAddMenuEntry("defiler les etats", 5);
  if (multifiles) {
    glutAddMenuEntry("Etat Precedent", 7);
    glutAddMenuEntry("Etat Suivant", 8);
  }
  glutAddMenuEntry("Editer mgconf", 6);
  glutAddMenuEntry("Recharger mgconf", 4);
  glutAddMenuEntry("Sauver mgpview", 9);
  glutAddMenuEntry("Recharger mgpview", 10);
  glutAddMenuEntry("Vue 3D", 3);
  glutAddMenuEntry("Plan XY", 2);
  glutAddMenuEntry("Plan XZ", 1);
  glutAddSubMenu("Options", submenu1);
  glutAddSubMenu("Outils", submenu2);
  glutAddSubMenu("Rendu des corps", submenu3);
  glutAddSubMenu("Rendu des interactions", submenu4);
  glutAddSubMenu("Exportations", submenu5);
  if (with_fluid) glutAddSubMenu("Fluide", submenu6);
  glutAddMenuEntry("Quitter", 0);
    
#else	/* English */
    
  submenu1 = glutCreateMenu(traitsubmenu1);
  glutAddMenuEntry("White background", 0);
  glutAddMenuEntry("Custom background", 1);
  glutAddMenuEntry("Black background", 2);
  glutAddMenuEntry("White foreground", 5);
  glutAddMenuEntry("Blue foreground", 6);
  glutAddMenuEntry("Black foreground", 7);
  glutAddMenuEntry("Transparency ON/OFF", 3);
  glutAddMenuEntry("Section      ON/OFF", 4);
        
  submenu2 = glutCreateMenu(traitsubmenu2);
  glutAddMenuEntry("Sample informations", 15);
  glutAddMenuEntry("Squared sample", 2);
  if (with_layers) glutAddMenuEntry("Geologic layers", 34);
  glutAddMenuEntry("Velocity field", 12);
  glutAddMenuEntry("Anisotropy", 22);
  glutAddMenuEntry("Anisotropy evolution", 24);
  glutAddMenuEntry("Informations on gaps", 26);
  glutAddMenuEntry("Contacts direction", 30);
  glutAddMenuEntry("fn vs ft", 16);
  glutAddMenuEntry("pdf(fn)", 14);
  glutAddMenuEntry("Granulometry", 27);
  glutAddMenuEntry("Local forces", 35);
                
  submenu3 = glutCreateMenu(traitsubmenu2);
  glutAddMenuEntry("Nothing", 10);
  glutAddMenuEntry("Coordination Number", 32);
  glutAddMenuEntry("Outline", 13);
  glutAddMenuEntry("Colors", 19);
  glutAddMenuEntry("Sizes", 36);
  glutAddMenuEntry("Angular Velocity (2D)", 8);
  glutAddMenuEntry("Velocity magnitude", 11);
  glutAddMenuEntry("Cumulated displacements", 21);
        
  submenu4 = glutCreateMenu(traitsubmenu2);
  glutAddMenuEntry("Nothing", 0);
  glutAddMenuEntry("Verlet list", 1);
  glutAddMenuEntry("Sticks links", 9);
  glutAddMenuEntry("Cracks", 28);
  glutAddMenuEntry("Strong/weak Networks", 31);
  glutAddMenuEntry("Forces", 33);
  glutAddMenuEntry("Forces from files for.out.xxx", 3);
  glutAddMenuEntry("Color lines for normal force", 20);
  glutAddMenuEntry("Compressive and tensile force", 7);
  glutAddMenuEntry("Positives normal force", 5);
  glutAddMenuEntry("Negatives normal force", 6);
  glutAddMenuEntry("Tangentials force", 4);
  glutAddMenuEntry("Moment", 25);
  glutAddMenuEntry("Tangential relative velocity", 17);
  glutAddMenuEntry("Liquid bond Volume", 29);
        
  submenu5 = glutCreateMenu(traitsubmenu4);
#ifdef _WITH_TIFF
  glutAddMenuEntry("Picture TIFF", 0);
#endif
#ifdef _WITH_PNG
  glutAddMenuEntry("Picture PNG", 9);
#endif        
  glutAddMenuEntry("List of pictures TIFF", 1);
  glutAddMenuEntry("Picture EPS using gl2ps", 2);
  glutAddMenuEntry("Picture EPS (2D)", 6);
  glutAddMenuEntry("File 'BODIES.DAT'", 3);
  glutAddMenuEntry("File 'export.cin'", 4);
  glutAddMenuEntry("File 'shot.pov'", 5);
  glutAddMenuEntry("File 'particles.Rxy'", 7);
  glutAddMenuEntry("File 'deplacement.txt'", 8);        
        
  submenu6 = glutCreateMenu(traitsubmenuFluid);
  glutAddMenuEntry("Nothing", 0);
  glutAddMenuEntry("Horizontal velocity", 1);
  glutAddMenuEntry("Vertical velocity", 2);
  glutAddMenuEntry("Velocity magnitude", 3);
  glutAddMenuEntry("Pressure", 4);
        
  popupmenu = glutCreateMenu(traitmenu);
  glutAddMenuEntry("Run", 5);
  if (multifiles) {
    glutAddMenuEntry("Previous state", 7);
    glutAddMenuEntry("Next state", 8);
  }
  glutAddMenuEntry("Edit mgconf", 6);
  glutAddMenuEntry("Reload mgconf", 4);
  glutAddMenuEntry("Save mgpview", 9);
  glutAddMenuEntry("Reload mgpview", 10);
  glutAddMenuEntry("3D view", 3);
  glutAddMenuEntry("XY plan", 2);
  glutAddMenuEntry("XZ plan", 1);
  glutAddSubMenu("Options", submenu1);
  glutAddSubMenu("Tools", submenu2);
  glutAddSubMenu("Body rendering", submenu3);
  glutAddSubMenu("Interaction rendering", submenu4);
  glutAddSubMenu("Export", submenu5);
  if (with_fluid) glutAddSubMenu("Fluide", submenu6);
  glutAddMenuEntry("Quit", 0);
#endif
    
}


