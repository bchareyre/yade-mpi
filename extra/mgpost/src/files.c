/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

void 
make_mgpalloc_file()
{
  FILE           *f;

  f = fopen("./mgpalloc", "w");
  if (f == NULL) {
    fprintf(stderr, "Error when opening mgpalloc\n");
    exit(EXIT_FAILURE);
  }
  fprintf(f, "nb_times    1\n");
  fprintf(f, "nb_bodies   20000\n");
  fprintf(f, "nb_interactions_per_body  6\n");
  fprintf(f, "nb_data_per_body          1");

  fclose(f);
  fprintf(stdout, "File 'mgpalloc' created\n");
}

void 
make_mgconf_file()
{
  FILE           *f;

  f = fopen("./mgconf", "w");
  if (f == NULL)
    exit(EXIT_FAILURE);

  fprintf(f, "# Window's sizes\n");
  fprintf(f, "Wwin = 500\n");
  fprintf(f, "Hwin = 500\n\n");

  fprintf(f, "# colors\n");
  fprintf(f, "funRGB  = (1.0 , 1.0 , 1.0)\n");
  fprintf(f, "bgcolor  = white\n");
  fprintf(f, "fgcolor  = blue\n");
  fprintf(f, "fgcolor1 = green\n");
  fprintf(f, "fgcolor2 = red\n\n");

  fprintf(f, "# Grid\n");
  fprintf(f, "GridXYZ = (1.0e-2 , 1.0e-2 , 1.0e-2)\n\n");

  fprintf(f, "# Scale of colors\n");
  fprintf(f, "autoScaleColors = yes\n");
  fprintf(f, "valcMAX = 1.0e0\n");
  fprintf(f, "valcMIN = -1.0e0\n\n");

  fprintf(f, "# length of arrows for velocities\n");
  fprintf(f, "vlocyFactor = 1.0e0\n\n");

  fprintf(f, "# Mobil frame velocity\n");
  fprintf(f, "vlocyX = 0.0e0\n");
  fprintf(f, "vlocyY = 0.0e0\n");
  fprintf(f, "vlocyZ = 0.0e0\n\n");

  fclose(f);
  fprintf(stdout, "File 'mgconf' created\n");
}

void 
save_mgpview()
{
  FILE           *out;

  out = fopen("./mgpview", "w");

  fwrite(&W, sizeof(int), 1, out);
  fwrite(&H, sizeof(int), 1, out);
  fwrite(&distance, sizeof(double), 1, out);
  fwrite(&phi, sizeof(double), 1, out);
  fwrite(&theta, sizeof(double), 1, out);
  fwrite(&Xviewp, sizeof(int), 1, out);
  fwrite(&Yviewp, sizeof(int), 1, out);
  fwrite(&L0pos, sizeof(GLfloat), 3, out);
  fwrite(&L1pos, sizeof(GLfloat), 3, out);
  fwrite(&whitedif, sizeof(GLfloat), 3, out);

  fclose(out);
}

void 
load_mgpview()
{
  FILE           *in;

  if (!access("./mgpview", F_OK)) {
    in = fopen("./mgpview", "r");

    fread(&W, sizeof(int), 1, in);
    fread(&H, sizeof(int), 1, in);
    fread(&distance, sizeof(double), 1, in);
    fread(&phi, sizeof(double), 1, in);
    fread(&theta, sizeof(double), 1, in);
    fread(&Xviewp, sizeof(int), 1, in);
    fread(&Yviewp, sizeof(int), 1, in);
    fread(&L0pos, sizeof(GLfloat), 3, in);
    fread(&L1pos, sizeof(GLfloat), 3, in);
    fread(&whitedif, sizeof(GLfloat), 3, in);
    fclose(in);
  } else
    fprintf(stderr, "file 'mgpview' not found !\n");
}

void 
charger_CINfile()
{
  FILE    *cin_file;
  char     ligne[256];
  int      nbl;
  int      i;
  int      bdy_id;

  fprintf(stdout, "Data filename: %s\n", datafilename);
  fflush(stdout);
  cin_file = fopen(datafilename, "r");
  if (cin_file == NULL) {
    fprintf(stderr, "Probleme lors de l'ouverture du fichier '%s'\n", datafilename);
    exit(EXIT_FAILURE);
  }
  fgets(ligne, 256, cin_file);
  fprintf(stdout, "%s", ligne);

  fscanf(cin_file, "%d %lf", &nbl, &mgp_time[0]);

  nbel = 0;
  datapos = 0;

  for (i = 0; i < nbl; i++) {
    fscanf(cin_file, "%d", &bdy_id);
    switch (bdy_id) {
    case 0:

      fscanf(cin_file, "%lf %lf %lf %lf %lf %lf %lf",
	     &radius[nbel][0],
	     &x[nbel][0], &y[nbel][0], &rot[nbel][0],
	     &vx[nbel][0], &vy[nbel][0], &vrot[nbel][0]);

      dataqty[i] = 1;
      datadistrib[i] = datapos;
      datas[datapos++] = radius[nbel][0];

      mode2D = MG_TRUE;
      bdyty[i] = MGP_DISKx;
      bdyclass[i] = MGP_GRAIN;

      nbel++;
      break;

    case 1:

      fscanf(cin_file, "%lf %lf %lf %lf %*f %*f %*f %lf %lf %lf %*f %*f %*f",
	     &radius[nbel][0],
	     &x[nbel][0], &y[nbel][0], &z[nbel][0],
	     &vx[nbel][0], &vy[nbel][0], &vz[nbel][0]);

      dataqty[i] = 1;
      datadistrib[i] = datapos;
      datas[datapos++] = radius[nbel][0];

      mode2D = MG_FALSE;
      bdyty[i] = MGP_SPHER;
      bdyclass[i] = MGP_GRAIN;

      nbel++;
      break;

    case 10:

      dataqty[i] = 4;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %*f %lf %lf %lf",
	     &datas[datapos], &datas[datapos + 1], &datas[datapos + 2],
	     &datas[datapos + 3]);
      datas[datapos + 1] = datas[datapos + 1] * 0.5;
      datapos += 4;

      bdyty[i] = MGP_PICKT;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    case 100:

      dataqty[i] = 2;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %lf",
	     &datas[datapos], &datas[datapos + 1]);
      datapos += 2;

      bdyty[i] = MGP_MURX0;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    case 101:

      dataqty[i] = 2;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %lf",
	     &datas[datapos], &datas[datapos + 1]);
      datapos += 2;

      bdyty[i] = MGP_MURX1;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    case 102:

      dataqty[i] = 2;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %lf",
	     &datas[datapos], &datas[datapos + 1]);
      datapos += 2;

      bdyty[i] = MGP_MURY0;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    case 103:

      dataqty[i] = 2;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %lf",
	     &datas[datapos], &datas[datapos + 1]);
      datapos += 2;

      bdyty[i] = MGP_MURY1;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    case 104:

      dataqty[i] = 2;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %lf",
	     &datas[datapos], &datas[datapos + 1]);
      datapos += 2;

      bdyty[i] = MGP_MURZ0;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    case 105:

      dataqty[i] = 2;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %lf",
	     &datas[datapos], &datas[datapos + 1]);
      datapos += 2;

      bdyty[i] = MGP_MURZ1;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    case 300:
    case 301:

      dataqty[i] = 4;
      datadistrib[i] = datapos;
      fscanf(cin_file, "%lf %lf %lf",
	     &datas[datapos], &datas[datapos + 1], &datas[datapos + 2]);
      datas[datapos + 3] = 0.0;	/* MEMO c'est comme ca
				 * pour le moment !! */
      datapos += 4;

      bdyty[i] = MGP_CYLYx;
      bdyclass[i] = MGP_OBSTACLE;

      nbel++;
      break;

    default:

      fprintf(stderr, "corps inconnu : %d\n", bdy_id);
      break;
    }

  }

  fclose(cin_file);
  nb_state = 1;

}


void 
charger_HISfile()
{
  FILE *his_file;
  char token[60];
  int i=0; 

  fprintf(stdout, "Data filename: %s\n", datafilename);
  fflush(stdout);
  his_file = fopen(datafilename, "r");
  if (his_file == NULL) 
  {
    fprintf(stderr, "Probleme lors de l'ouverture du fichier '%s'\n", datafilename);
    exit(EXIT_FAILURE);
  }
  
  datapos = 0;
  nbel = 0;
  
  unsigned int inCluster = 0;
  unsigned int dec = 0; /* corps pas pris en compte pour le network */
  
  while (!feof(his_file))
  {
	fscanf(his_file, "%s", token);
	if (feof(his_file)) break;
	
	if (!strcmp((const char *) token,"Sample{") || !strcmp((const char *) token,"Sample3d{"))
	{	
		while (strcmp((const char *) token,"}") || inCluster == 1)
		{
                  fscanf(his_file, "%s", token); 
                  
                  if (!strcmp((const char *) token,"}"))
                  {
                    if (inCluster == 1) { inCluster = 0;fscanf(his_file, "%s", token);}
                  }
                  
                  if (!strcmp((const char *) token,"Cluster{"))
                  {
                    inCluster = 1;
                  }
                  
                  if (!strcmp((const char *) token,"rline"))
                  {
                    ++dec;
                  }
                  
                  if (!strcmp((const char *) token,"sphere"))
                  {
                    /* WARNING bricolage sur les coord */
                    fscanf(his_file, "%*d %lf %lf %lf %lf %*f %*f %*f %lf %lf %lf %*f %*f %*f",
                           &radius[nbel][0],
                           &x[nbel][0], &z[nbel][0], &y[nbel][0],
                           &vx[nbel][0], &vz[nbel][0], &vy[nbel][0]);
                    z[nbel][0] *= -1.0;
                    vz[nbel][0] *= -1.0;

                    dataqty[i]       = 1;
                    datadistrib[i]   = datapos;
                    datas[datapos++] = radius[nbel][0];

                    mode2D      = MG_FALSE;
                    bdyty[i]    = MGP_SPHER;
                    bdyclass[i] = MGP_GRAIN;
                
                    i++;
                    nbel++;
                  }
                  
                  if (!strcmp((const char *) token,"disk"))
                  {
                    fscanf(his_file, "%*d %lf %lf %lf %lf %lf %lf %lf",
                           &radius[nbel][0],
                           &x[nbel][0], &y[nbel][0], &rot[nbel][0],
                           &vx[nbel][0], &vy[nbel][0], &vrot[nbel][0]);

                    dataqty[i]       = 1;
                    datadistrib[i]   = datapos;
                    datas[datapos++] = radius[nbel][0];

                    mode2D      = MG_TRUE;
                    bdyty[i]    = MGP_DISKx;
                    bdyclass[i] = MGP_GRAIN;
		
                    i++;
                    nbel++;
                  }

				  if (!strcmp((const char *) token,"polyg"))
				  {
					int nbVertex = 0,v;
					double xnode,ynode;
					double dist=0.0,Rout=0.0;
					fscanf(his_file, "%*d %d", &nbVertex);

					dataqty[i]       = 1;
					datadistrib[i]   = datapos;

					for (v=0;v<nbVertex;++v)
					{
					  fscanf(his_file, "%lf %lf",&xnode,&ynode);
					  dist = sqrt(xnode*xnode+ynode*ynode);
					  if (dist>Rout) Rout=dist;
					  datas[datapos++] = xnode;
					  dataqty[i] += 1;
					  datas[datapos++] = ynode;
					  dataqty[i] += 1;
					}
					datas[datapos++] = Rout;
					radius[nbel][0] = Rout;

					fscanf(his_file, "%lf %lf %lf %lf %lf %lf",
							&x[nbel][0], &y[nbel][0], &rot[nbel][0],
							&vx[nbel][0], &vy[nbel][0], &vrot[nbel][0]);



					mode2D      = MG_TRUE;
					bdyty[i]    = MGP_POLYG;
					bdyclass[i] = MGP_GRAIN;

					i++;
					nbel++;
				  }
				  
				  if (!strcmp((const char *) token,"CellBiP"))
				  {
					fscanf(his_file, "%lf %lf %lf %lf",
							&hxx,&hxy,&hyx,&hyy);		
                    isBiP = MG_TRUE;
				  }	
		}
	}

        
	if (!strcmp((const char *) token,"Network{")) 
	{
		nbint = 0;
		unsigned int ip,jp;
		while (strcmp((const char *) token,"}"))
		{
		  fscanf(his_file, "%s", token); 
		
                  if (!strcmp((const char *) token,"dkdk")) 
                  {
                    fscanf(his_file, "%d %d",&ip,&jp);
                    nbneighbors[ip-dec][0] += 1;
					//bghty[nbint] == 0;
                    neighbor[nbint][0] = jp-dec+1;
                    fscanf(his_file, "%*f %*f %lf %lf %*f",&Fn[nbint][0],&Ft[nbint][0]);
                    nbint++;
                  }
				  /*
				  if (!strcmp((const char *) token,"dkdkP"))
				  {
					fscanf(his_file, "%d %d",&ip,&jp);
					nbneighbors[ip-dec][0] += 1;
					//bghty[nbint] == 1;
					neighbor[nbint][0] = jp-dec+1;
					fscanf(his_file, "%*f %*f %lf %lf %*f",&Fn[nbint][0],&Ft[nbint][0]);
					nbint++;
				  }
				  */
		}	
	}	
        
  }

  fclose(his_file);
  nb_state = 1;
}

void 
charger_geometrie()
{
  const char     *str;
  int             i = 0;
  xmlDocPtr       doc;
  xmlNodePtr      node, state_node, part_node, bdy_node;
  
  fprintf(stdout, "Data-file name: %s\n", datafilename);
  fflush(stdout);
  
  doc = (xmlDocPtr) xmlParseFile((const char *) datafilename);
  if (doc == NULL) {
    fprintf(stderr, "Error: %s does not exist or maybe is not wellformed!\n", datafilename);
    exit(EXIT_FAILURE);
  }
  fprintf(stdout, "Parsing... ");
  fflush(stdout);
  
#ifdef _LIBXML2
  node = xmlDocGetRootElement(doc);
#else
  node = doc->root;
#endif
  
  if (!strcmp((const char*)node->name, "mgpost")) {
    str = (const char *) xmlGetProp(node, (const xmlChar *) "mode");
    if (!strcmp(str, "2D")) {
      phi = -90;
      theta = 0;
      mode2D = MG_TRUE;
    }
    if (!strcmp(str, "3D")) {
      if (!multifiles) {
        phi = PHI_INIT;
        theta = THETA_INIT;
      }
      mode2D = MG_FALSE;
    }
#ifdef _LIBXML2
    node = node->children;
#else
    node = node->childs;
#endif
    
    nb_state = 0;
    nbel = 0;
    datapos = 0;
    nbint = 0;
    while (node != NULL) {
      if (!strcmp((const char *)node->name, "comment")) {
        
#ifdef _LIBXML2
        datatitre = (const char *) xmlNodeListGetString(doc, node->children, 1);
#else
        datatitre = (const char *) xmlNodeListGetString(doc, node->childs, 1);
#endif
        
      }
      
      if (!strcmp((const char *)node->name, "newcolor")) {
		if (nbcolgrp < nbcolgrpMaxi) nbcolgrp++;
        else break;
        strcpy(colorName[nbcolgrp-1],(const char *) xmlGetProp(node, (const xmlChar *) "name"));
        colIsShown[nbcolgrp-1] = MG_TRUE;
      }
      
      
      if (!strcmp((const char *)node->name, "state")) {
        if (multifiles) {
          str = (const char *) xmlGetProp(node, (const xmlChar *) "id");
          if (str != NULL) {
            state_id = atoi(str);
            state = 0;
          }
        }
        str = (const char *) xmlGetProp(node, (const xmlChar *) "time");
        if (str != NULL)
          mgp_time[nb_state] = atof(str);
        
        i = 0;
        nbint = 0;
        
#ifdef _LIBXML2
        state_node = node->children;
#else
        state_node = node->childs;
#endif
        
        while ((state_node != NULL)) {
          
#ifdef _LIBXML2
          part_node = state_node->children;
#else
          part_node = state_node->childs;
#endif
          
          while (part_node != NULL) {
            
            if (!strcmp((const char *)part_node->name, "DISKx")) {
              bdyty[i] = MGP_DISKx;
              bdyclass[i] = MGP_GRAIN;
              
              if (nb_state == 0) {
                dataqty[i] = 1;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "r");
              if (str != NULL) 
                {
                radius[i][nb_state] = atof(str);
                
                if (nb_state == 0)
                  datas[datapos++] = radius[i][nb_state];
                }
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "col");
              if (str != NULL) 
                {
                color[i] = atoi(str);
                }       
              
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              
              read_body_data(bdy_node, i, nb_state);
              i++;
              nbel = i;
              
            }
            if (!strcmp((const char *)part_node->name, "SPHER")) {
              bdyty[i] = MGP_SPHER;
              bdyclass[i] = MGP_GRAIN;
              
              if (nb_state == 0) {
                dataqty[i] = 1;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "r");
              if (str != NULL) {
                radius[i][nb_state] = atof(str);
                if (nb_state == 0)
                  datas[datapos++] = radius[i][nb_state];
              }
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "col");
              if (str != NULL) 
                {
                color[i] = atoi(str);
                } 
              
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              read_body_data(bdy_node, i, nb_state);
              i++;
              nbel = i;
              
            }
            if (!strcmp((const char *)part_node->name, "POLYG")) {
              bdyty[i] = MGP_POLYG;
              bdyclass[i] = MGP_GRAIN;
              
              if (nb_state == 0)
                datadistrib[i] = datapos;
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "r");
              if (str != NULL)
                radius[i][nb_state] = atof(str);
              
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              read_body_data(bdy_node, i, nb_state);
              
              i++;
              nbel = i;
            }
            if (!strcmp((const char *)part_node->name, "POLYE")) {
              bdyty[i] = MGP_POLYE;
              bdyclass[i] = MGP_GRAIN;
              
              if (nb_state == 0)
                datadistrib[i] = datapos;
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "r");
              if (str != NULL)
                radius[i][nb_state] = atof(str);
              
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              read_body_data(bdy_node, i, nb_state);
              
              i++;
              nbel = i;
            }
            if (!strcmp((const char *)part_node->name, "SEGMT")) {
              bdyty[i] = MGP_SEGMT;
              bdyclass[i] = MGP_OBSTACLE;
              
              if (nb_state == 0) {
                dataqty[i] = 1;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "l");
              if (str != NULL) {
                radius[i][nb_state] = atof(str);
                if (nb_state == 0)
                  datas[datapos++] = radius[i][nb_state];
              }
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              read_body_data(bdy_node, i, nb_state);
              
              i++;
              nbel = i;
            }
            if (!strcmp((const char *)part_node->name, "JONCx")) {
              bdyty[i] = MGP_JONCx;
              bdyclass[i] = MGP_OBSTACLE;
              
              if (nb_state == 0) {
                dataqty[i] = 2;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "ax1");
              if ((str != NULL) && (nb_state == 0))
                datas[datapos++] = atof(str);
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "ax2");
              if ((str != NULL) && (nb_state == 0))
                datas[datapos++] = atof(str);
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              read_body_data(bdy_node, i, nb_state);
              i++;
              nbel = i;
              
            }
            if ((!strcmp((const char *)part_node->name, "WALX0"))
                  || (!strcmp((const char *)part_node->name, "WALX1"))
                  || (!strcmp((const char *)part_node->name, "WALLX"))) {
              bdyty[i] = 100;	/* FIXME: ne concerver
              * que WALLX !! */
              bdyclass[i] = MGP_OBSTACLE;
              
              if (nb_state == 0) {
                dataqty[i] = 1;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "pos");
              if ((str != NULL) && (nb_state == 0)) {
                datas[datapos++] = atof(str);
                x[i][nb_state] = 0.0;
                y[i][nb_state] = 0.0;
                z[i][nb_state] = 0.0;
              }
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              i++;
              nbel = i;
              
            }
            if ((!strcmp((const char *)part_node->name, "WALY0"))
                  || (!strcmp((const char *)part_node->name, "WALY1"))
                  || (!strcmp((const char *)part_node->name, "WALLY"))) {
              bdyty[i] = 102;	/* FIXME: ne conserver
              * que WALLY !! */
              bdyclass[i] = MGP_OBSTACLE;
              
              if (nb_state == 0) {
                dataqty[i] = 1;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "pos");
              if ((str != NULL) && (nb_state == 0)) {
                datas[datapos++] = atof(str);
                x[i][nb_state] = 0.0;
                y[i][nb_state] = 0.0;
                z[i][nb_state] = 0.0;
              }
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              i++;
              nbel = i;
              
            }
            if (!strcmp((const char *)part_node->name, "MBRxx")) {
              bdyty[i] = MGP_MBR2D;
              bdyclass[i] = MGP_OBSTACLE;
              
              datadistrib[i] = datapos;
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "nbi");
              if ((str != NULL) && (nb_state == 0)) {
                datas[datapos] = atof(str);
                dataqty[i] = 2 * ((int) (datas[datapos]) + 1) + 1;
                datapos++;
                x[i][nb_state] = 0.0;
                y[i][nb_state] = 0.0;
                z[i][nb_state] = 0.0;
              }
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              
              while ((bdy_node != NULL)) {
                if (!strcmp((const char *)bdy_node->name, "Pmbr")) {
                  str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "x");
                  
                  if (str != NULL)
                    datas[datapos++] = atof(str);
                  str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "y");
                  
                  if (str != NULL)
                    datas[datapos++] = atof(str);
                  
                }
                bdy_node = bdy_node->next;
              }
              
              i++;
              nbel = i;
              
            }
            if (!strcmp((const char *)part_node->name, "PICKT")) {
              bdyty[i] = MGP_PICKT;
              bdyclass[i] = MGP_OBSTACLE;
              
              if (nb_state == 0) {
                dataqty[i] = 4;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "p");
              if ((str != NULL) && (nb_state == 0))
                datas[datapos++] = atof(str);
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "r");
              if ((str != NULL) && (nb_state == 0))
                datas[datapos++] = atof(str);
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "angle");
              if ((str != NULL) && (nb_state == 0))
                datas[datapos++] = atof(str);
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "ltot");
              if ((str != NULL) && (nb_state == 0))
                datas[datapos++] = atof(str);
              
              x[i][nb_state] = 0.0;
              y[i][nb_state] = 0.0;
              z[i][nb_state] = 0.0;
              
#ifdef _LIBXML2
              bdy_node = part_node->children;
#else
              bdy_node = part_node->childs;
#endif
              
              i++;
              nbel = i;
              
            }
            if ((!strcmp((const char *)part_node->name, "CYLY"))
                  || (!strcmp((const char *)part_node->name, "CYLYx"))) {
              bdyty[i] = MGP_CYLYx;
              bdyclass[i] = MGP_OBSTACLE;
              
              if (nb_state == 0) {
                dataqty[i] = 4;
                datadistrib[i] = datapos;
              }
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "r");
              if ((str != NULL) && (nb_state == 0))
                datas[datapos++] = atof(str);
              
              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "rayon");	/* XXX */
                                              if ((str != NULL) && (nb_state == 0))
                                              datas[datapos++] = atof(str);
                                              
                                              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "base");
                                              if ((str != NULL) && (nb_state == 0))
                                              datas[datapos++] = atof(str);
                                              
                                              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "l");
                                              if ((str != NULL) && (nb_state == 0))
                                              datas[datapos++] = atof(str);
                                              
                                              str = (const char *) xmlGetProp(part_node, (const xmlChar *) "pos");
                                              if ((str != NULL) && (nb_state == 0))
                                              datas[datapos++] = atof(str);
                                              
                                              x[i][nb_state] = 0.0;
                                              y[i][nb_state] = 0.0;
                                              z[i][nb_state] = 0.0;
                                              
#ifdef _LIBXML2
                                              bdy_node = part_node->children;
#else
                                              bdy_node = part_node->childs;
#endif
                                              
                                              i++;
                                              nbel = i;
                                              
            }

/* READING CONTACTS */

            if ((!strcmp((const char *)part_node->name, "DKDKx")) || (!strcmp((const char *)part_node->name, "SPSPx"))) {
  
  str = (const char *) xmlGetProp(part_node, (const xmlChar *) "j");
  if (str == NULL)
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "antac");
  
  if (str != NULL) {
		nbneighbors[nbel - 1][nb_state] += 1;
    
		neighbor[nbint][nb_state] = atoi(str);
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "stat");
		if (str != NULL)
		  statut[nbint][nb_state] = atoi(str);
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "rn");
		if (str != NULL)
		  Fn[nbint][nb_state] = atof(str);
		else
		  Fn[nbint][nb_state] = 0.0;
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "rt");
		if (str != NULL)
		  Ft[nbint][nb_state] = atof(str);
		else
		  Ft[nbint][nb_state] = 0.0;
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "rs");
		if (str != NULL)
		  Fs[nbint][nb_state] = atof(str);
		else
		  Fs[nbint][nb_state] = 0.0;
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "M");
		if (str != NULL)
		  Crot[nbint][nb_state] = atof(str);
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "vn");
		if (str != NULL)
		  Vn[nbint][nb_state] = atof(str);
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "vt");
		if (str != NULL)
		  Vt[nbint][nb_state] = atof(str);
    
		str = (const char *) xmlGetProp(part_node, (const xmlChar *) "Vliq");
		if (str != NULL)
		  Vliq[nbint][nb_state] = atof(str);
		else
		  Vliq[nbint][nb_state] = 0.0;
    
		nbint++;
  }
}
part_node = part_node->next;
          }
state_node = state_node->next;
        }
nb_state++;

      }
node = node->next;
    }
  }
xmlFreeDoc(doc);

if(more_forces)
{
  FILE * fp;
  datafilename[0] = 'f';
  datafilename[1] = 'o';
  datafilename[2] = 'r';
  datafilename[11] = '\0'; /* lecture gz pas possible !!! */
  fprintf(stdout,"->%s\n",datafilename);
  nbContacts=0;
  fp = fopen(datafilename,"r");
  if(fp)
    {
    unsigned int c; 
    fscanf(fp,"%d",&nbContacts);
    fprintf(stdout,"nbContacts = %d / %d\n",nbContacts,nbbodies * nbcontacts);
    for(c=0;c<nbContacts;++c)
      {
      fscanf(fp,"%lf %lf %lf  %lf %lf %lf  %lf %d %d %d",
             &Contact[c].x,&Contact[c].y,&Contact[c].z,
             &Contact[c].nx,&Contact[c].ny,&Contact[c].nz,
             &Contact[c].fn,&Contact[c].rank,&Contact[c].i,&Contact[c].j);  
      }
    fclose(fp);    
    }
  else
    {
    nbContacts = 0;
    fprintf(stderr,"Ya pas de fichier for.out.xxx !!!!\n");
    }
}

if(with_fluid)
{
  FILE * fp;
  datafilename[0] = 'f';
  datafilename[1] = 'l';
  datafilename[2] = 'u';
  fprintf(stdout,"->%s\n",datafilename);
  nbFluidCells=0;
  fp = fopen(datafilename,"r");
  if(fp)
    {
    unsigned int c; 
    fscanf(fp,"%d",&nbFluidCells);
    FluidCell = (FLUIDCELL *) calloc((size_t) (nbFluidCells), sizeof(FLUIDCELL));
    fprintf(stdout,"nbFluidCells = %d\n",nbFluidCells);
    for(c=0;c<nbFluidCells;++c)
      {
      fscanf(fp,"%lf %lf  %lf %lf  %lf %lf %lf",
             &FluidCell[c].x,&FluidCell[c].y,
             &FluidCell[c].delx,&FluidCell[c].dely,
             &FluidCell[c].ux,&FluidCell[c].uy,&FluidCell[c].p);  
      }
    fclose(fp);    
    }
  else
    {
    nbFluidCells = 0;
    with_fluid = MG_FALSE;
    fprintf(stderr,"Ya pas de fichier flu.out.xxx !!!!\n");
    }
}

if(with_layers && !layers_defined)
{
  unsigned int c;
  double hmin=y[0][0],hmax=y[0][0];
  for (c=1;c<nbel;++c) /* il faudrait verif qu'il s'agise bien d'un disque ou sphere ...*/
    {
    if (y[c][0] > hmax) hmax = y[c][0];
    if (y[c][0] < hmin) hmin = y[c][0];
    }
  for (c=0;c<nbel;++c)
    {
    layer[c] = (unsigned int)floor((y[c][0] - hmin)/(hmax-hmin)*(double)nbLayers);
    if (layer[c] >= nbLayers) layer[c] = nbLayers - 1;
    }
  
  layers_defined = MG_TRUE;
}

fprintf(stdout, "done\n");
fflush(stdout);
if ((nb_state > nbtimes) || (nbel > nbbodies) 
    || (nbint > (nbbodies * nbcontacts)) || (datapos > (nbbodies * nbdatas)) ) {
  fprintf(stdout, "Warning: there is not enought memory space allocated\n");
  fprintf(stdout, "         you must creat or modify the file 'mgpalloc'\n");
  fprintf(stdout, "...................................................\n");
  fprintf(stdout, "  wanted states : %d\tallocated: %d\n", nb_state, nbtimes);
  fprintf(stdout, "  wanted bodies : %d\tallocated: %d\n", nbel, nbbodies);
  fprintf(stdout, "  wanted interactions : %d\tallocated: %d\n", nbint, nbbodies * nbcontacts);
  fprintf(stdout, "  wanted data   : %d\tallocated: %d\n", datapos, nbbodies * nbdatas);
  fprintf(stdout, "...................................................\n");
}
findBoundaries(state, &xminB, &yminB, &zminB,
               &xmaxB, &ymaxB, &zmaxB);

}

void 
read_body_data(xmlNodePtr bdy_node, int bdy, int t)
{
  int             nbnodes = 0, nbfaces = 0;
  const char     *str;

  while ((bdy_node != NULL)) {
    if ((t == 0) && (!strcmp((const char *)bdy_node->name, "node"))) {
      nbnodes++;

      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "x");
      if (str != NULL) {
	datas[datapos++] = atof(str);
	dataqty[bdy] += 1;
      }
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "y");
      if (str != NULL) {
	datas[datapos++] = atof(str);
	dataqty[bdy] += 1;
      }
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "z");
      if (str != NULL) {
	datas[datapos++] = atof(str);
	dataqty[bdy] += 1;
      }
    }
    if ((t == 0) && (!strcmp((const char *)bdy_node->name, "face"))) {
      nbfaces++;

      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "n1");
      if (str != NULL) {
	datas[datapos++] = atof(str);
	dataqty[bdy] += 1;
      }
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "n2");
      if (str != NULL) {
	datas[datapos++] = atof(str);
	dataqty[bdy] += 1;
      }
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "n3");
      if (str != NULL) {
	datas[datapos++] = atof(str);
	dataqty[bdy] += 1;
      }
    }
    if (!strcmp((const char *)bdy_node->name, "position")) {
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "x");
      if (str != NULL)
	x[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "y");
      if (str != NULL)
	y[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "z");
      if (str != NULL)
	z[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "rot");
      if (str != NULL)
	rot[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "t1");	/* XXX */
      if (str != NULL)
	rot[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "t");	/* XXX */
      if (str != NULL)
	rot[bdy][t] = atof(str);
    }
    if (!strcmp((const char *)bdy_node->name, "velocity")) {
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "x");
      if (str != NULL)
	vx[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "y");
      if (str != NULL)
	vy[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "z");
      if (str != NULL)
	vz[bdy][t] = atof(str);

      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "v1");	/* XXX */
      if (str != NULL)
	vx[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "v2");	/* XXX */
      if (str != NULL)
	vy[bdy][t] = atof(str);
      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "v3");	/* XXX */
      if (str != NULL)
	vz[bdy][t] = atof(str);

      str = (const char *) xmlGetProp(bdy_node, (const xmlChar *) "rot");
      if (str != NULL)
	vrot[bdy][t] = atof(str);
    }
 
    bdy_node = bdy_node->next;
  }

  if ((t == 0) && (bdyty[bdy] == MGP_POLYE)) {
    datas[datapos++] = (double) nbnodes;
    datas[datapos++] = (double) nbfaces;
    dataqty[bdy] += 2;
  }
}


/* writetiff Copyright (c) Mark J. Kilgard, 1997. */
int writetiff(char *filename, char *description, int x, int y, int width, int height, int compression)
{
	#ifdef _WITH_TIFF
  TIFF           *file;
  GLubyte        *image, *p;
  int             i;

  file = TIFFOpen(filename, "w");
  if (file == NULL) {
    return 1;
  }
  image = (GLubyte *) malloc(width * height * sizeof(GLubyte) * 3);

  /*
   * OpenGL's default 4 byte pack alignment would leave extra bytes at
   * the end of each image row so that each full row contained a number
   * of bytes divisible by 4.  Ie, an RGB row with 3 pixels and 8-bit
   * componets would be laid out like "RGBRGBRGBxxx" where the last
   * three "xxx" bytes exist just to pad the row out to 12 bytes (12 is
   * divisible by 4). To make sure the rows are packed as tight as
   * possible (no row padding), set the pack alignment to 1.
   */
  glPixelStorei(GL_PACK_ALIGNMENT, 1);

  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, image);
  TIFFSetField(file, TIFFTAG_IMAGEWIDTH, (uint32) width);
  TIFFSetField(file, TIFFTAG_IMAGELENGTH, (uint32) height);
  TIFFSetField(file, TIFFTAG_BITSPERSAMPLE, 8);
  TIFFSetField(file, TIFFTAG_COMPRESSION, compression);
  TIFFSetField(file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL, 3);
  TIFFSetField(file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(file, TIFFTAG_ROWSPERSTRIP, 1);
  TIFFSetField(file, TIFFTAG_IMAGEDESCRIPTION, description);
  p = image;
  for (i = height - 1; i >= 0; i--) {
    if (TIFFWriteScanline(file, p, i, 0) < 0) {
      free(image);
      TIFFClose(file);
      return 1;
    }
    p += width * sizeof(GLubyte) * 3;
  }
  TIFFClose(file);
#endif
  return 0;
}



int writepng(const char *filename, char *description, int x, int y, int width, int height)
{
#ifdef _WITH_PNG
  int i;

  FILE *fp;
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep *rowp;
  GLubyte *glimage = NULL;

  fp = fopen(filename, "wb");
  if (!fp) return 1;
  
  glimage = (GLubyte *) malloc(width * height * sizeof(GLubyte) * 3);
  
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glReadPixels(x, y, width, height, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid *) glimage); 

  rowp = (png_bytep *)malloc(sizeof(png_bytep *) * height);
  if (!rowp) return 1;
  
  for (i = 0; i < height; i++) 
  {
    rowp[i] = (png_bytep)&glimage[3 * ((height - i - 1) * width)];              
  }

  png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                    NULL, NULL, NULL);
  if (!png_ptr) return 1;

  info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr) return 1;


  png_init_io(png_ptr, fp);
  png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
               PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
               PNG_FILTER_TYPE_DEFAULT);
  png_write_info(png_ptr, info_ptr);
  png_write_image(png_ptr, rowp);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  free(rowp);

  fflush(stdout);

  fclose(fp);
#endif
  return 0;
}


void 
exportCIN()
{
  int             i;
  int             type;

  no_warranty();

  cin_nbel = nbel;
  bd_cinInit();
  for (i = 0; i < nbel; i++) {
    type = bdyty[i];
    switch (type) {
    case MGP_DISKx:
      bd_cinDISKx(i, x[i][state] + xvec, y[i][state] + yvec, radius[i][state]);
      break;
    case MGP_SPHER:
      bd_cinSPHER(i, x[i][state] + xvec, y[i][state] + yvec, z[i][state] + zvec, radius[i][state]);
      break;
    case MGP_MURX0:
      bd_cinWALxx(i, datas[datadistrib[i]], "WALX0");
      break;
      /* case 101: */
      /* bd_cinWALxx (i, datas[datadistrib[i]], "WALX1"); */
      /* break; */
    case MGP_MURY0:
      bd_cinWALxx(i, datas[datadistrib[i]], "WALY0");
      break;
      /* case 103: */
      /* bd_cinWALxx (i, datas[datadistrib[i]], "WALY1"); */
      /* break; */
    default:
      break;
    }
  }
  bd_cinClose();

  fprintf(stdout, "WARNING : You should check the file 'bodies.cin' : body 100 may be 101, 102 > 103, 104 > 105\n");
}

void export_Rxy()
{
  int             i;
  /*int             type;*/
  FILE * out;
  unsigned int nd = 0;
  
  no_warranty();
  
  for (i = 0; i < nbel; i++) {
    if (bdyty[i] == MGP_DISKx) ++nd;
  }
  
  out = fopen("particles.Rxy","w");
  fprintf(out,"%d\n",nd);
  for (i = 0; i < nbel; i++) {
    if (bdyty[i] == MGP_DISKx) 
        fprintf(out,"%lf %lf %lf\n",radius[i][state],x[i][state] + xvec, y[i][state] + yvec);
  }
  fclose(out);
}

void export_SSPX()
{
  int             i;
  FILE * out;
  unsigned int nd = 0;
  
  no_warranty();
  
  for (i = 0; i < nbel; i++) {
    if (bdyty[i] == MGP_DISKx) ++nd;
  }
  
  out = fopen("deplacements.txt","w");
  // TODO cas 2d ou 3d ...
  fprintf(out,"C\t2\n");
  for (i = 0; i < nbel; i++) {
    if (bdyty[i] == MGP_DISKx) 
      fprintf(out,"%d\t%le\t%le\t%le\t%le\n",i ,x_0[i] + xvec, y_0[i] + yvec, x[i][state] + xvec, y[i][state] + yvec);
  }
  fclose(out);
}



