/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/* ATTENTION ne pas utiliser ces fonctions sans les comprendre car
 * certaines sont bricolees
 * V. Richefeu
 */

void 
compil_pstcom()
{
	FILE           *pst_file;
	char            clin[256];
	char           *mgtoken[64];
	int             nbtoken = 0;
	char           *s;

	int             icom = 0, nprof = 0;

	fprintf(stdout, "Fichier Commandes de post-traitement : %s\n", pstfilename);
	pst_file = fopen((const char *) pstfilename, "r");

	if (pst_file == NULL) {
		fprintf(stderr, "Probleme d'ouverture du fichier %s\n", pstfilename);
		exit(EXIT_FAILURE);
	}
	while (!feof(pst_file)) {
		/* reading one line */
		fgets(clin, 256, pst_file);
		if (feof(pst_file))
			break;

		/* decomposition of the line */
		s = (char *) strtok(clin, (const char *) delim);
		nbtoken = 0;

		while (s != NULL) {
			mgtoken[nbtoken++] = s;
			s = (char *) strtok(NULL, (const char *) delim);
		}
		mgtoken[nbtoken] = 0;

		/* analize of the line */

		if (!strcmp((const char *) mgtoken[0], "profile")) {
			nprof = atoi(mgtoken[1]);
			ActiveProfile[nprof] = MG_TRUE;
			pos_profile[nprof][0] = atof(mgtoken[2]);
			pos_profile[nprof][1] = atof(mgtoken[3]);
			pos_profile[nprof][2] = atof(mgtoken[4]);
			pos_profile[nprof][3] = atof(mgtoken[5]);
			pos_profile[nprof][4] = atof(mgtoken[6]);
			pos_profile[nprof][5] = atof(mgtoken[7]);
		}
		if (!strcmp((const char *) mgtoken[1], "direction_contacts_3d")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 1;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, " STATE = %g\n", pstcom[icom].param[0]);

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_nombre_coordination")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 0;

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_compacite_casag")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 0;

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_compacite_casag_corrige")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 0;

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_nbc_glissants")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 1;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, " SEUIL = %g\n", pstcom[icom].param[0]);

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_aniso_z")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 0;
      
			icom++;
		}    
		if (!strcmp((const char *) mgtoken[1], "distance_parcours")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 2;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, " STATE0= %g\n", pstcom[icom].param[0]);
			pstcom[icom].param[1] = atof(mgtoken[3]);
			fprintf(stdout, " STATE1= %g\n", pstcom[icom].param[1]);

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "deformation_cumulee")) {	/* EN TRAVAUX */
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 2;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, " STATE0= %g\n", pstcom[icom].param[0]);
			pstcom[icom].param[1] = atof(mgtoken[3]);
			fprintf(stdout, " STATE1= %g\n", pstcom[icom].param[1]);

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_indice_vides")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 4;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, "     X = %g\n", pstcom[icom].param[0]);
			pstcom[icom].param[1] = atof(mgtoken[3]);
			fprintf(stdout, "     Y = %g\n", pstcom[icom].param[1]);
			pstcom[icom].param[2] = atof(mgtoken[4]);
			fprintf(stdout, "     Z = %g\n", pstcom[icom].param[2]);
			pstcom[icom].param[3] = atof(mgtoken[5]);
			fprintf(stdout, "     R = %g\n", pstcom[icom].param[3]);

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_distance_murs_Y")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 0;

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "evol_interp_cumul_moy")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 0;

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "prof_position")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 1;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, "N°PROF = %d\n", (int) pstcom[icom].param[0]);

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "prof_deplacement")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 1;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, "N°PROF = %d\n", (int) pstcom[icom].param[0]);

			icom++;
		}
		if (!strcmp((const char *) mgtoken[1], "prof_deplacement_X")) {
			fprintf(stdout, "-------- %d\n", icom);
			strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
			fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
			strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
			fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
			pstcom[icom].nbparam = 1;
			pstcom[icom].param[0] = atof(mgtoken[2]);
			fprintf(stdout, "N°PROF = %d\n", (int) pstcom[icom].param[0]);

			icom++;
		}
		
		if (!strcmp((const char *) mgtoken[1], "contrainte")) 
		  {
		    fprintf(stdout, "-------- %d\n", icom);
		    strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
		    fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
		    strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
		    fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
		    pstcom[icom].nbparam = 4;
		    pstcom[icom].param[0] = atof(mgtoken[2]);
		    pstcom[icom].param[1] = atof(mgtoken[3]);
		    pstcom[icom].param[2] = atof(mgtoken[4]);
		    pstcom[icom].param[3] = atof(mgtoken[5]);
		    fprintf(stdout, "  DATA = %lg, %lg, %lg, %lg\n",
			    pstcom[icom].param[0],pstcom[icom].param[1],
			    pstcom[icom].param[2],pstcom[icom].param[3]);
		    icom++;
		  }

		if (!strcmp((const char *) mgtoken[1], "compacite_local")) 
		  {
		    fprintf(stdout, "-------- %d\n", icom);
		    strcpy((char *) pstcom[icom].file, (const char *) mgtoken[0]);
		    fprintf(stdout, "  FILE = %s\n", pstcom[icom].file);
		    strcpy((char *) pstcom[icom].name, (const char *) mgtoken[1]);
		    fprintf(stdout, "  NAME = %s\n", pstcom[icom].name);
		    pstcom[icom].nbparam = 4;
		    pstcom[icom].param[0] = atof(mgtoken[2]);
		    pstcom[icom].param[1] = atof(mgtoken[3]);
		    pstcom[icom].param[2] = atof(mgtoken[4]);
		    pstcom[icom].param[3] = atof(mgtoken[5]);
		    fprintf(stdout, "  DATA = %lg, %lg, %lg, %lg\n",
			    pstcom[icom].param[0],pstcom[icom].param[1],
			    pstcom[icom].param[2],pstcom[icom].param[3]);
		    icom++;
		  }
	}

	nbcom = icom;

	fclose(pst_file);

}

void 
pst_evol(unsigned char iscompressed)
{
	int             i, j, ii, k, ifile, c, ip, igrain;
	int             nprof;
	double          LX, LY, LZ, DL, DP, D;
	FILE           *out[50];
	double          f1 = 0.0, f2 = 0.0, f3 = 0.0, f4 = 0.0, f5 = 0.0;

	for (c = 0; c < nbcom; c++) {
		out[c] = fopen(pstcom[c].file, "w");
	}

	ifile = 1;
	sprintf((char *) numfile, num_file_format, ifile);
	strcpy((char *) datanewfilename, "mgp.out.");
	strcat((char *) datanewfilename, (const char *) numfile);

	if (iscompressed)
		strcat((char *) datanewfilename, ".gz");

	while (!access((const char *) datanewfilename, F_OK)) {
		strcpy(datafilename, (const char *) datanewfilename);
		nullifyCumulatedTabs();
		charger_geometrie();

		/* contruction des profiles ... */
		if (ifile == 1) {
			precalculs();

			for (c = 0; c < NB_PROFILES; c++)
				if (ActiveProfile[c] != 0) {
					for (i = 0; i < nbel; i++) {

						LX = x[i][state] - pos_profile[c][0];
						LY = y[i][state] - pos_profile[c][1];
						LZ = z[i][state] - pos_profile[c][2];

						DL = sqrt(LX * LX + LY * LY + LZ * LZ);
						DP = LX * pos_profile[c][3] + LY * pos_profile[c][4] + LZ * pos_profile[c][5];

						D = sqrt(DL * DL - DP * DP);

						if (D <= radius[i][state]) {
							iprofile[c][nbg_profile[c]] = i;
							nbg_profile[c] += 1;
						}
					}
				}
		}
		/* boucle de post-traitement */

		for (c = 0; c < nbcom; c++) {
			if (!strcmp((const char *) pstcom[c].name, "evol_nbc_glissants")) {
				double          seuil = pstcom[c].param[0];
				double          indic = 99.0;
				int             nbcg = 0, nbc = 0;

				k = 0;
				for (i = 0; i < nbel; i++) {
					for (ii = 0; ii < nbneighbors[i][state]; ii++) {
						j = neighbor[k][state] - 1;

						if (Fn[k][state] >= 0.0) {
							
							indic = sqrt(Ft[k][state] * Ft[k][state] + Fs[k][state] * Fs[k][state]) / Fn[k][state];
							if (indic >= seuil)
								nbcg += 1;
							nbc++;
						}
						k++;
					}
				}

				fprintf(out[c], "%lg %d %d %lg\n", mgp_time[state], nbcg, nbc, (double) nbcg / (double) nbc);

			}

      if (!strcmp((const char *) pstcom[c].name, "evol_aniso_z")) 
        {
        double Z;
        unsigned int Nc=0,N=0;
        
        /* calcul de z */
				k = 0;
				for (i = 0; i < nbel; i++) 
          {
          if (bdyty[i] < 2) N++; /* si ce n'est pas une paroi*/
					for (ii = 0; ii < nbneighbors[i][state]; ii++) 
            {
						j = neighbor[k][state] - 1;
            
            if(bdyty[i] < 2 && Fn[k][state]>0.0) Nc++; 
            /* rq: seules les particules voient les parois */
            
						k++;
            }
          }
        
        Z = 2.0 * (double)Nc/(double)N;
        
        /* tenseur de texture */
        double          pijx, pijy, pijz;
        double          inv_norm_pij;
        double          n1, n2, n3;
        float         **Fabric;
        double          a1, a2, a3;
        float          *d;
        float           p;
        float         **v;
        int             nrot;
        
        Fabric = matrix(1, 3, 1, 3);
        v = matrix(1, 3, 1, 3);
        d = vector(1, 3);
        
        for (i = 1; i <= 3; i++)
          for (j = 1; j <= 3; j++)
            Fabric[i][j] = 0.0;
        
        k=0;
        for (i = 0; i < nbel; ++i)
          {
          if (bdyty[i] < 2) N++;
          for (ii = 0; ii < nbneighbors[i][state]; ++ii) 
            {
            j = neighbor[k][state] - 1;
            if(bdyty[j] < 2 && Fn[k][state] > 0.) 
              {
              pijx = x[i][state] - x[j][state];
              pijy = y[i][state] - y[j][state];
              pijz = z[i][state] - z[j][state];
              
              inv_norm_pij = 1.0 / sqrt(pijx * pijx + pijy * pijy + pijz * pijz);
              n1 = pijx * inv_norm_pij;
              n2 = pijy * inv_norm_pij;
              n3 = pijz * inv_norm_pij;
              
              Fabric[1][1] += n1 * n1;
              Fabric[1][2] += n1 * n2;
              Fabric[1][3] += n1 * n3;
              Fabric[2][2] += n2 * n2;
              Fabric[2][3] += n2 * n3;
              Fabric[3][3] += n3 * n3;
              }
            ++k;
            }
          }
        
        /* Symetrie du tenseur */
        Fabric[2][1] = Fabric[1][2];
        Fabric[3][1] = Fabric[1][3];
        Fabric[3][2] = Fabric[2][3];
        
        /* division par nb de contact */
        for (i = 1; i <= 3; i++)
          for (j = 1; j <= 3; j++)
            Fabric[i][j] /= (double)Nc;
        
        
        /* calcul des valeurs et vecteurs propres */
        jacobi(Fabric, 3, d, v, &nrot);
        
        /* Classement du plus grand au plus petit */
        for (i = 1; i <= 3; i++) {
          p = d[k = i];
          for (j = i + 1; j <= 3; j++)
            if (d[j] >= p)
              p = d[k = j];
          if (k != i) {
            d[k] = d[i];
            d[i] = p;
            for (j = 1; j <= 3; j++) {
              p = v[j][i];
              v[j][i] = v[j][k];
              v[j][k] = p;
            }
          }
        }
        
        /* valeurs caracteristiques */
        a1 = 2.0 * (d[1] - d[2]);
        a2 = 2.0 * (d[1] - d[3]);
        a3 = 2.0 * (d[2] - d[3]);  
        
        /* sockage des valeurs */
				fprintf(out[c], "%lg %lg %lg %lg %lg\n", mgp_time[state], Z, a1, a2, a3);
        
        }

			if (!strcmp((const char *) pstcom[c].name, "compacite_local"))
			  {
			    double Xv,Yv,Zv,Rv;
			    double dist,h;
			    double Vs=0.0,Comp;

			    Xv = pstcom[c].param[0];
			    Yv = pstcom[c].param[1];
			    Zv = pstcom[c].param[2];
			    Rv = pstcom[c].param[3];

			    for (i = 0; i < nbel; i++)
			      if (bdyty[i]==MGP_SPHER)
			      {
				dist = sqrt( pow(x[i][state]-Xv,2.0)
                                          +  pow(y[i][state]-Yv,2.0)
                                          +  pow(z[i][state]-Zv,2.0) );
				if (dist<=Rv)
				  {
				    h = (dist-radius[i][state])-Rv;
				    if (h<0.0) h=0.0;

				    Vs += ( (4.0/3.0)*M_PI*pow(radius[i][state],3.0) 
				      - ((M_PI*h*h*h)/6.0 + (M_PI*(2.0*radius[i][state]*h-h*h)*h)/2.0) );
				  }

			      }

			    Comp = Vs/((4.0/3.0)*M_PI*Rv*Rv*Rv);
			    fprintf(out[c], "%lg %lg\n", mgp_time[state], Comp);
			  }


			/*contrainte dans une sphere*/

			if (!strcmp((const char *) pstcom[c].name, "contrainte")) 
			  {
			    double          OiX, OiY, OiZ;
			    double          OjX, OjY, OjZ;
			    double          di, dj;
			    double          Xv, Yv, Zv, Rv_2;
			    double          LijX, LijY, LijZ;
			    double          unX = 0.0, unY = 0.0, unZ = 0.0;
			    double          inv_norme, inv_normeUs;
			    double          usX = 0.0, usZ = 0.0;
			    double          utX = 0.0, utY = 0.0, utZ = 0.0;
			    double          Fx, Fy, Fz;
			    double          Sxx, Sxy, Sxz, Syx, Syy,
			      Syz, Szx, Szy, Szz;
			    double          invV, Rv;

			    Xv = pstcom[c].param[0];
			    Yv = pstcom[c].param[1];
			    Zv = pstcom[c].param[2];
			    Rv_2 = pstcom[c].param[3] * pstcom[c].param[3];

			    Rv = pstcom[c].param[3];
			    invV = 3.0 / (4.0 * 3.14159 * Rv * Rv * Rv);

			    fprintf(stdout,"%lg %lg %lg %lg\n",Xv,Yv,Zv,Rv);

			    k = 0;
			    Sxx = Sxy = Sxz = Syx = Syy = Syz = Szx = Szy = Szz = 0.0;

			    for (i = 0; i < nbel; i++) 
			      if (bdyty[i]==MGP_SPHER)
				{
				  for (ii = 0; ii < nbneighbors[i][state]; ii++) {
				    j = neighbor[k][state] - 1;

				    OiX = x[i][state];
				    OiY = y[i][state];
				    OiZ = z[i][state];
				    OjX = x[j][state];
				    OjY = y[j][state];
				    OjZ = z[j][state];

				    di = (Xv - OiX) * (Xv - OiX) + (Yv - OiY) * (Yv - OiY) + (Zv - OiZ) * (Zv - OiZ);
				    dj = (Xv - OjX) * (Xv - OjX) + (Yv - OjY) * (Yv - OjY) + (Zv - OjZ) * (Zv - OjZ);

				    if ((di < Rv_2) && (dj < Rv_2)) {

				      LijX = OjX - OiX;
				      LijY = OjY - OiY;
				      LijZ = OjZ - OiZ;

				      inv_norme = 1.0 / sqrt(LijX * LijX + LijY * LijY + LijZ * LijZ);

				      /* n = OjOi / |OjOi| */
				      unX = -LijX * inv_norme;
				      unY = -LijY * inv_norme;
				      unZ = -LijZ * inv_norme;

				      if ((unX != 0.0) || (unZ != 0.0)) {
					/*
					 * s=Y^n
					 * (donc
					 * usY=0)
					 */
					usX = unZ;
					usZ = -unX;

					inv_normeUs = 1 / sqrt(usX * usX + usZ * usZ);

					usX = usX * inv_normeUs;
					usZ = usZ * inv_normeUs;

					/* t = s ^ n */
					utX = -usZ * unY;
					utY = unX * usZ - usX * unZ;
					utZ = usX * unY;
				      } else {
					if (unY == 1.0) {
					  usX = 0.0;
					  usZ = 1.0;
					  utX = -1.0;
					  utY = 0.0;
					  utZ = 0.0;
					}
					if (unY == -1.0) {
					  usX = 0.0;
					  usZ = 1.0;
					  utX = 1.0;
					  utY = 0.0;
					  utZ = 0.0;
					}
				      }

				      Fx = Fn[k][state] * unX + Ft[k][state] * utX + Fs[k][state] * usX;
				      Fy = Fn[k][state] * unY + Ft[k][state] * utY;
				      Fz = Fn[k][state] * unZ + Ft[k][state] * utZ + Fs[k][state] * usZ;

				      Sxx += Fx * LijX;
				      Sxy += Fx * LijY;
				      Sxz += Fx * LijZ;
				      Syx += Fy * LijX;
				      Syy += Fy * LijY;
				      Syz += Fy * LijZ;
				      Szx += Fz * LijX;
				      Szy += Fz * LijY;
				      Szz += Fz * LijZ;


				    }
				    k++;
				  }
				}



			    Sxx = invV * Sxx;
			    Sxy = invV * Sxy;
			    Sxz = invV * Sxz;
			    Syx = invV * Syx;
			    Syy = invV * Syy;
			    Syz = invV * Syz;
			    Szx = invV * Szx;
			    Szy = invV * Szy;
			    Szz = invV * Szz;

			    fprintf(out[c], "%lg %lg %lg %lg %lg %lg %lg %lg %lg %lg\n", mgp_time[state],
				    Sxx, Sxy, Sxz, Syx, Syy, Syz, Szx, Szy, Szz);

			  }
			/* fin contraintes */


			if (!strcmp((const char *) pstcom[c].name, "deformation_cumulee")) {	/* EN TRAVAUX */

				if (ifile == (int) pstcom[c].param[0]) {
					double          dn, Halo = 0.2e-3;

					xbak = (double *) calloc((size_t) nbel, sizeof(double));
					ybak = (double *) calloc((size_t) nbel, sizeof(double));
					zbak = (double *) calloc((size_t) nbel, sizeof(double));
					epsXX = (double *) calloc((size_t) nbel, sizeof(double));
					epsYY = (double *) calloc((size_t) nbel, sizeof(double));

					vois = (int *) calloc((size_t) nbel * 30, sizeof(int));
					nbvois = (int *) calloc((size_t) nbel, sizeof(int));

					k = 0;
					for (i = 0; i < nbel; i++) {
						nbvois[i] = 0;
						for (j = 0; j < nbel; j++) {
							dn = sqrt(pow(x[i][state] - x[j][state], 2.0) +
								  pow(y[i][state] - y[j][state], 2.0) +
								  pow(z[i][state] - z[j][state], 2.0))
								- (radius[i][state] + radius[j][state]);

							if (dn <= Halo) {
								vois[k] = j;
								nbvois[i] += 1;

								k++;
							}
						}
					}


					for (i = 0; i < nbel; i++) {
						xbak[i] = x[i][0];
						ybak[i] = y[i][0];
						zbak[i] = z[i][0];
					}
				}
				if ((ifile > (int) pstcom[c].param[0]) && (ifile <= (int) pstcom[c].param[1])) {
					double          DnX, DnY /* , DnZ */ ;

					k = 0;
					for (i = 0; i < nbel; i++) {

						for (ii = 0; ii < nbvois[i] /* [state] */ ; ii++) {
							j = vois[k] /* [state] - corri */ ;

							DnX = (x[j][state] - xbak[j]) - (x[i][state] - xbak[i]);
							DnY = (y[j][state] - ybak[j]) - (y[i][state] - ybak[i]);
							/*
							 * DnZ = (z[j][state]
							 * - zbak[j]) -
							 * (z[i][state] -
							 * zbak[i]);
							 */

							if (nbvois[i] /* [state] */ >0) {
								epsXX[i] += 3.0 * (DnX * (x[j][state] - x[i][state])) / nbvois[i];
								epsYY[i] += 3.0 * (DnY * (y[j][state] - y[i][state])) / nbvois[i];
							}
							/* if(nbvois[j]>0) */
							/* { */
							/*
							 * epsXX[j] +=
							 * 3.0*(DnX*(x[i][stat
							 * e]-x[j][state]))/
							 * nbneighbors[j][stat
							 * e];
							 */
							/*
							 * epsYY[j] +=
							 * 3.0*(DnY*(y[i][stat
							 * e]-y[j][state]))/
							 * nbneighbors[j][stat
							 * e];
							 */
							/* } */


							k++;
						}



						xbak[i] = x[i][state];
						ybak[i] = y[i][state];
						zbak[i] = z[i][state];

					}

				}
				if (ifile == (int) pstcom[c].param[1]) {
					fprintf(out[c], "<?xml version=\"1.0\"?>\n <mgpost mode=\"3D\">\n  <state id=\"0\" time=\"0\">\n");
					fprintf(out[c], "   <body>\n");
					for (i = 0; i < nbel; i++) {
						fprintf(out[c], "    <SPHER id=\"%d\" r=\"%lg\">\n", i + 1, radius[i][state]);
						fprintf(out[c], "     <position x=\"%lg\" y=\"%lg\" z=\"%lg\"/>\n", x[i][state], y[i][state], z[i][state]);
						fprintf(out[c], "     <Strain xx=\"%lg\" yy=\"%lg\"/>\n", epsXX[i], epsYY[i]);
						fprintf(out[c], "    </SPHER>\n");
					}
					fprintf(out[c], "   </body>\n  </state>\n </mgpost>\n");

					free(xbak);
					free(ybak);
					free(zbak);
					free(epsXX);
					free(epsYY);
					free(vois);
					free(nbvois);
				}
			}
			if (!strcmp((const char *) pstcom[c].name, "distance_parcours")) {

				if (ifile == (int) pstcom[c].param[0]) {
					xbak = (double *) calloc((size_t) nbel, sizeof(double));
					ybak = (double *) calloc((size_t) nbel, sizeof(double));
					zbak = (double *) calloc((size_t) nbel, sizeof(double));
					Dst = (double *) calloc((size_t) nbel, sizeof(double));

					for (i = 0; i < nbel; i++) {
						xbak[i] = x[i][0];
						ybak[i] = y[i][0];
						zbak[i] = z[i][0];
					}
				}
				if ((ifile > (int) pstcom[c].param[0]) && (ifile <= (int) pstcom[c].param[1])) {
					for (i = 0; i < nbel; i++) {
						Dst[i] += sqrt(pow((x[i][state] - xbak[i]), 2.0)
							       + pow((y[i][state] - ybak[i]), 2.0)
							       + pow((z[i][state] - zbak[i]), 2.0));

						xbak[i] = x[i][state];
						ybak[i] = y[i][state];
						zbak[i] = z[i][state];

					}

				}
				if (ifile == (int) pstcom[c].param[1]) {
					fprintf(out[c], "<?xml version=\"1.0\"?>\n <mgpost mode=\"3D\">\n  <state id=\"0\" time=\"0\">\n");
					fprintf(out[c], "   <body>\n");
					for (i = 0; i < nbel; i++) {
						fprintf(out[c], "    <SPHER id=\"%d\" r=\"%lg\">\n", i + 1, radius[i][state]);
						fprintf(out[c], "     <position x=\"%lg\" y=\"%lg\" z=\"%lg\"/>\n", x[i][state], y[i][state], z[i][state]);
						/*
						 * pour le moment on met ca
						 * dans strain !  On verra ce
						 * qu'on en fait par la suite
						 */
						fprintf(out[c], "     <Strain xx=\"%lg\"/>\n", Dst[i]);
						fprintf(out[c], "    </SPHER>\n");
					}
					fprintf(out[c], "   </body>\n  </state>\n </mgpost>\n");

					free(xbak);
					free(ybak);
					free(zbak);
					free(Dst);
				}
			}
			if (!strcmp((const char *) pstcom[c].name, "direction_contacts_3d")) {
				if (ifile == (int) pstcom[c].param[0]) {
					double          lijX = 0.0, lijY = 0.0,
					                lijZ = 0.0, norme = 0.0;
					double          d_angle_alpha = 0.0,
					                d_angle_beta = 0.0;
					int             Prob[50][50];
					double          alpha = 0.0, beta = 0.0;
					int             nbdiv_alpha = 5,
					                nbdiv_beta = 12;

					d_angle_alpha = (M_PI * 0.5) / (double) nbdiv_alpha;
					d_angle_beta = (M_PI * 2.0) / (double) nbdiv_beta;

					for (i = 0; i < nbdiv_alpha; i++)
						for (j = 0; j < nbdiv_beta; j++)
							Prob[i][j] = 0;

					k = 0;
					for (i = 0; i < nbel; i++) {
						for (ii = 0; ii < nbneighbors[i][state]; ii++) {
							j = neighbor[k][state] - 1;

							lijX = x[j][state] - x[i][state];
							lijY = y[j][state] - y[i][state];
							lijZ = z[j][state] - z[i][state];

							if (lijY > 0.0) {
								lijX = -lijX;
								lijY = -lijY;
								lijZ = -lijZ;
							}
							norme = sqrt(lijX * lijX + lijY * lijY + lijZ * lijZ);

							if (norme > (radius[i][state] + radius[j][state])) {
								k++;
								continue;
							}
							alpha = fabs(asin(lijY / norme));


							norme = sqrt(lijX * lijX + lijZ * lijZ);
							if (lijZ > 0.0)
								beta = acos(lijX / norme);

							if (lijZ < 0.0)
								beta = 2.0 * M_PI - acos(lijX / norme);

							if (lijZ == 0.0) {
								if (lijX == 1.0)
									beta = 0.0;
								else
									beta = M_PI;
							}
							Prob[(int) (alpha / d_angle_alpha)][(int) (beta / d_angle_beta)] += 1;

							k++;
						}
					}

					norme = 0.0;
					for (i = 0; i < nbdiv_alpha; i++)
						for (j = 0; j < nbdiv_beta; j++)
							norme += (double) Prob[i][j];

					for (i = 0; i < nbdiv_alpha; i++) {
						alpha = (double) i *d_angle_alpha;

						for (j = 0; j < nbdiv_beta; j++) {

							beta = (double) j *d_angle_beta;

							fprintf(out[c], "%g %g %g\n", (double) Prob[i][j] / norme * cos(alpha) * cos(beta),
								(double) Prob[i][j] / norme * sin(alpha),
								(double) Prob[i][j] / norme * cos(alpha) * sin(beta));
						}

						fprintf(out[c], "%g %g %g\n", (double) Prob[i][0] / norme * cos(alpha),
							(double) Prob[i][0] / norme * sin(alpha), 0.0);
						fprintf(out[c], "\n");
					}

				}
			}
			if (!strcmp((const char *) pstcom[c].name, "prof_position")) {
				for (ip = 0; ip < nbg_profile[(int) pstcom[c].param[0]]; ip++) {
					igrain = iprofile[(int) pstcom[c].param[0]][ip];
					fprintf(out[c], "%g %g %g\n", x[igrain][state], y[igrain][state], z[igrain][state]);
				}
				fprintf(out[c], "\n\n");
			}
			if (!strcmp((const char *) pstcom[c].name, "prof_deplacement")) {
				for (ip = 0; ip < nbg_profile[(int) pstcom[c].param[0]]; ip++) {
					nprof = (int) pstcom[c].param[0];
					igrain = iprofile[nprof][ip];

					LX = x[igrain][state] - pos_profile[nprof][0];
					LY = y[igrain][state] - pos_profile[nprof][1];
					LZ = z[igrain][state] - pos_profile[nprof][2];

					DP = LX * pos_profile[nprof][3] + LY * pos_profile[nprof][4] + LZ * pos_profile[nprof][5];

					DL = pow((x[igrain][state] - x_0[igrain]), 2.0)
						+ pow((y[igrain][state] - y_0[igrain]), 2.0)
						+ pow((z[igrain][state] - z_0[igrain]), 2.0);

					fprintf(out[c], "%g %g\n", DP, DL);
				}
				fprintf(out[c], "\n\n");
			}
			if (!strcmp((const char *) pstcom[c].name, "prof_deplacement_X")) {
				for (ip = 0; ip < nbg_profile[(int) pstcom[c].param[0]]; ip++) {
					nprof = (int) pstcom[c].param[0];
					igrain = iprofile[nprof][ip];

					LX = x[igrain][state] - pos_profile[nprof][0];
					LY = y[igrain][state] - pos_profile[nprof][1];
					LZ = z[igrain][state] - pos_profile[nprof][2];

					DP = LX * pos_profile[nprof][3] + LY * pos_profile[nprof][4] + LZ * pos_profile[nprof][5];
					DL = x[igrain][state] - x_0[igrain];

					fprintf(out[c], "%g %g\n", DP, DL);
				}
				fprintf(out[c], "\n\n");
			}
			if (!strcmp((const char *) pstcom[c].name, "evol_interp_cumul_moy")) {
				double          pijx, pijy, pijz;
				double          dist, dn;
				double          ic = 0.0, icm = 0.0;
				int             nbc = 0;

				k = 0;
				for (i = 0; i < nbel; i++) {

					for (ii = 0; ii < nbneighbors[i][state]; ii++) {
						j = neighbor[k][state] - 1;

						pijx = x[j][state] - x[i][state];
						pijy = y[j][state] - y[i][state];
						pijz = z[j][state] - z[i][state];

						dist = sqrt(pijx * pijx + pijy * pijy + pijz * pijz);
						dn = dist - radius[i][state] - radius[j][state];

						if (dn < 0.0) {
							ic -= dn;
							nbc++;
						}
						k++;
					}
				}

				if (nbc > 0)
					icm = ic / nbc;

				fprintf(out[c], "%lg %lg %lg\n", mgp_time[state], ic, icm);

			}
			if (!strcmp((const char *) pstcom[c].name, "evol_distance_murs_Y")) {
				f1 = f2 = 0.0;
				for (i = 0; i < nbel; i++) {
					if (bdyty[i] == 102)
						f1 = datas[datadistrib[i]];
					if (bdyty[i] == 103)
						f2 = datas[datadistrib[i]];
				}

				fprintf(out[c], "%g %g\n", mgp_time[state], f2 - f1);
			}

			if (!strcmp((const char *) pstcom[c].name, "evol_nombre_coordination")) 
			  {
			    double fmoy_pos=0.0,fmoy_neg=0.0;
			    int nf_pos=0,nf_neg=0;
			    double zs_pos=0.0,zw_pos=0.0;
			    double zs_neg=0.0,zw_neg=0.0;

			    k = 0;
			    for (i = 0; i < nbel; i++) 
			      {
				for (ii = 0; ii < nbneighbors[i][state]; ii++) 
				  {
				    j = neighbor[k][state] - 1;

				    if (Fn[k][state] > 0.0) 
				      {
					fmoy_pos += Fn[k][state];
					(nf_pos)++;
				      }

				    if (Fn[k][state] < 0.0) 
				      {
					fmoy_neg += Fn[k][state];
					(nf_neg)++;
				      }

				    k++;
				  }
			      }

			    fmoy_pos = fmoy_pos / (double)nf_pos;
			    fmoy_neg = fmoy_neg / (double)nf_neg;

			    k = 0;
			    for (i = 0; i < nbel; i++) 
			      {
				for (ii = 0; ii < nbneighbors[i][state]; ii++) 
				  {
				    j = neighbor[k][state] - 1;

				    if (Fn[k][state] >= fmoy_pos) 
				      {
					zs_pos += 1.0;
				      }

				    if (Fn[k][state] < fmoy_pos && Fn[k][state] > 0.0) 
				      {
					zw_pos += 1.0;
				      }

				    if (Fn[k][state] > fmoy_neg && Fn[k][state] < 0.0) 
				      {
					zw_neg += 1.0;
				      }

				    if (Fn[k][state] <= fmoy_neg) 
				      {
					zs_neg += 1.0;
				      }

				    k++;
				  }
			      }

			    zs_pos = 2.0 * zs_pos / (double) nbel;
			    zw_pos = 2.0 * zw_pos / (double) nbel;
			    zs_neg = 2.0 * zs_neg / (double) nbel;
			    zw_neg = 2.0 * zw_neg / (double) nbel;

			    fprintf(out[c], "%lg %lg %lg %lg %lg\n", mgp_time[state],
				    zs_pos,zw_pos,zs_neg,zw_neg);
			  }


			if (!strcmp((const char *) pstcom[c].name, "evol_compacite_casag"))
			  {
			    double          C, Vs = 0.0, Vt;

			    for (i = 0; i < nbel; i++)
			      if(bdyty[i]==MGP_SPHER)
			      {
				Vs += 1.333333 * M_PI * pow(radius[i][state], 3.0);
			      }

			    findBoundaries(state, &xminB, &yminB, &zminB,
					   &xmaxB, &ymaxB, &zmaxB);
			    /* Vt = M_PI * 0.03 * 0.03 * (ymaxB - yminB); */
			    Vt = M_PI * 0.023 * 0.023 * (ymaxB - yminB);
			    C = Vs / Vt;
			    fprintf(out[c], "%lg %lg\n", mgp_time[state], C);
			  }


			if (!strcmp((const char *) pstcom[c].name, "evol_compacite_casag_corrige")) {
				double          C, Vs = 0.0, Vt;
				double          Pij, h, h2;

				k = 0;
				for (i = 0; i < nbel; i++) {
					Vs += 1.333333 * M_PI * pow(radius[i][state], 3.0);
					for (ii = 0; ii < nbneighbors[i][state]; ii++) {
						j = neighbor[k][state] - 1;
						Pij = sqrt(
							   pow(x[j][state] - x[i][state], 2.0)
							   + pow(y[j][state] - y[i][state], 2.0)
							   + pow(z[j][state] - z[i][state], 2.0));

						if (Pij < (radius[i][state] + radius[j][state])) {
							h2 = (pow(radius[i][state], 2.0) - pow(radius[j][state], 2.0) + Pij * Pij) / (2.0 * Pij);
							h2 = radius[i][state] * radius[i][state] - h2 * h2;
							h = sqrt(h2);
							Vs -= .166666 * M_PI * h2 * (3.0 * (radius[i][state] + radius[j][state]) - 2.0 * h);
						}
						k++;
					}
				}

				findBoundaries(state, &xminB, &yminB, &zminB,
					       &xmaxB, &ymaxB, &zmaxB);
				Vt = M_PI * 0.03 * 0.03 * (ymaxB - yminB);
				C = Vs / Vt;
				fprintf(out[c], "%lg %lg\n", mgp_time[state], C);
			}
			if (!strcmp((const char *) pstcom[c].name, "evol_indice_vide")) {
				f1 = 0.0;	/* volume solide */

				for (i = 0; i < nbel; i++) {
					/* distance entre les centres */
					f2 = sqrt(pow(x[i][state] - pstcom[c].param[0], 2.0)
						  + pow(y[i][state] - pstcom[c].param[1], 2.0)
						  + pow(z[i][state] - pstcom[c].param[2], 2.0));

					/* "partie a enlevee" */
					f3 = f2 + radius[i][state] - pstcom[c].param[3];

					if (f2 < (pstcom[c].param[3] - radius[i][state]))
						f1 += 1.333333 * M_PI * pow(radius[i][state], 3.0);

					if ((f3 > 0) && (f3 < radius[i][state]))
						f1 += 1.333333 * M_PI * pow(radius[i][state], 3.0)
							- 0.333333 * M_PI * f3 * f3 * (3 * radius[i][state] - f3);

					if ((f3 < 2 * radius[i][state]) && (f3 >= radius[i][state]))
						f1 += 0.333333 * M_PI * pow(2 * radius[i][state] - f3, 2.0) * (radius[i][state] + f3);
				}

				/* volume zone */
				f4 = 1.333333 * M_PI * pow(pstcom[c].param[3], 3.0);
				/* indice de vides : (Vt - Vs) / Vs */
				f5 = (f4 - f1) / f1;
				fprintf(out[c], "%g %g\n", mgp_time[state], f5);
			}
		}

		ifile++;
		sprintf((char *) numfile, "%03d", ifile);
		strcpy((char *) datanewfilename, "mgp.out.");
		strcat((char *) datanewfilename, (const char *) numfile);
		if (iscompressed)
			strcat((char *) datanewfilename, ".gz");
	}

	for (c = 0; c < nbcom; c++) {
		fclose(out[c]);
		fprintf(stdout, "%s cree\n", pstcom[c].file);
	}

}

