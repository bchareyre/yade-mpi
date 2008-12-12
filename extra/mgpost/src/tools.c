/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

void 
display_infos_on_body(int i)
{
	int             type;
  int             a,k0;

	fprintf(stdout, "................................................\n");

	type = bdyty[i];
	switch (type) {
	case 0:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: DISKx\n");
		fprintf(stdout, "  Radius: %lg\n", radius[i][state]);
		fprintf(stdout, "  Position: %lg, %lg\n", x[i][state] + xvec, y[i][state] + yvec);
		fprintf(stdout, "  Velocity: %lg, %lg\n", vx[i][state], vy[i][state]);
		break;

	case 1:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: SPHER\n");
		fprintf(stdout, "  Radius: %lg\n", radius[i][state]);
		fprintf(stdout, "  Position: %lg, %lg, %lg\n", x[i][state] + xvec, y[i][state] + yvec, z[i][state] + zvec);
		fprintf(stdout, "  Velocity: %lg, %lg, %lg\n", vx[i][state], vy[i][state], vz[i][state]);
    
    a=k0=0;
    while(a<i) k0 += nbneighbors[a++][state];
      
    for (a=0;a<nbneighbors[i][state];++a) {
      if (Fn[k0+a][state]!=0.) 
      fprintf(stdout, "  force %d-%d: %lg\n", i+1,neighbor[k0+a][state]+1,Fn[k0+a][state]);
    }
    
		break;

	case 2:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: POLYG\n");
		fprintf(stdout, "  Position: %lg, %lg\n", x[i][state] + xvec, y[i][state] + yvec);
		fprintf(stdout, "  Velocity: %lg, %lg\n", vx[i][state], vy[i][state]);
		break;

	case 3:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: SEGMT\n");
		break;

	case 4:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: JONCx\n");
		fprintf(stdout, "  Position: %lg, %lg, %lg\n", x[i][state] + xvec, y[i][state] + yvec, z[i][state] + zvec);
		fprintf(stdout, "  Velocity: %lg, %lg, %lg\n", vx[i][state], vy[i][state], vz[i][state]);
		break;

	case 5:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: POLYE\n");
		fprintf(stdout, "  Position: %lg, %lg, %lg\n", x[i][state] + xvec, y[i][state] + yvec, z[i][state] + zvec);
		fprintf(stdout, "  Velocity: %lg, %lg, %lg\n", vx[i][state], vy[i][state], vz[i][state]);
		break;

	case 10:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: PICKT\n");
		fprintf(stdout, "  Position Y: %lg\n", datas[datadistrib[i]]);
		break;

	case 100:
	case 101:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: WALLX\n");
		fprintf(stdout, "  Position: %lg\n", datas[datadistrib[i]]);
		break;

	case 102:
	case 103:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: WALLY\n");
		fprintf(stdout, "  Position: %lg\n", datas[datadistrib[i]]);
		break;

	case 104:
	case 105:

		fprintf(stdout, "  Body id : %d\n", i + 1);
		fprintf(stdout, "  Type: WALLZ\n");
		fprintf(stdout, "  Position: %lg\n", datas[datadistrib[i]]);
		break;

	default:
		break;
	}

	fprintf(stdout, "................................................\n");
}

void 
info_gap()
{
	int             i, j, k = 0, l;
	int             type;
	double          gap = 0.0, gapmax = 0.0, gapmin = 0.0, gapmoy = 0.0;
	double          gapmoypos = 0.0, gapmoyneg = 0.0;
	int             nbgap = 0, nbgap_p = 0, nbgap_m = 0;

	k = 0;
	for (i = 0; i < nbel; i++) {
		for (l = 0; l < nbneighbors[i][state]; l++) {

			j = neighbor[k][state] - 1;
			type = bdyty[i];
			switch (type) {
			case 0:

				gap = sqrt(pow(x[j][state] - x[i][state], 2.0)
				      + pow(y[j][state] - y[i][state], 2.0))
					- radius[i][state] - radius[j][state];
				nbgap++;
				break;

			case 1:

				gap = sqrt(pow(x[j][state] - x[i][state], 2.0)
				       + pow(y[j][state] - y[i][state], 2.0)
				      + pow(z[j][state] - z[i][state], 2.0))
					- radius[i][state] - radius[j][state];
				nbgap++;
				break;

			default:
				break;
			}

			gapmoy += gap;
			gapmin = (gapmin > gap) ? gap : gapmin;
			gapmax = (gapmax < gap) ? gap : gapmax;

			if (gap > 0.0) {
				gapmoypos += gap;
				nbgap_p++;
			} else {
				gapmoyneg += gap;
				nbgap_m++;
			}

			k++;
		}
	}


	fprintf(stdout, "...................................................\n");
	fprintf(stdout, "  INFO on GAP = OiOj - Ri - Rj\n  (only for DISKx or SPHER bodies)\n");
	fprintf(stdout, "...................................................\n");
	fprintf(stdout, "  Gap min = %14.7E\n", gapmin);
	fprintf(stdout, "  Gap max = %14.7E\n", gapmax);
	fprintf(stdout, "  Gap moy = %14.7E\n", gapmoy / (double) nbgap);
	fprintf(stdout, "  Gap moy pos = %14.7E\n", gapmoypos / (double) nbgap_p);
	fprintf(stdout, "  Gap moy neg = %14.7E\n", gapmoyneg / (double) nbgap_m);
	fprintf(stdout, "...................................................\n");

}


void 
affiche_infos()
{
	double          xm, xM, ym, yM, zm, zM;
	int             i, ii, k, Nc_minus, Nc_plus, nbi;
	int             type;
	double          surf = 0.0, volum = 0.0;
	double          F_moy_pos, Strong_percent, Weak_percent;
	double          Vt = 1.0, rc = 30.0e-3, h = 0.0, C;

	Nc_minus = Nc_plus = nbi = 0;
	k = 0;
	for (i = 0; i < nbel; i++) {
		type = bdyty[i];
		switch (type) {

		case 0:

			surf += MG_PI * pow(radius[i][0], 2.0);
			break;

		case 1:

			volum += 1.333333 * MG_PI * pow(radius[i][0], 3.0);
			break;

		default:
			break;
		}

		for (ii = 0; ii < nbneighbors[i][state]; ii++) {
			if (Fn[k][state] > 0.0)
				Nc_plus += 1;
			if (Fn[k][state] < 0.0)
				Nc_minus += 1;
			k++;
		}
	}

	nbi = Nc_plus + Nc_minus;

	findBoundaries(state, &xm, &ym, &zm, &xM, &yM, &zM);

	/* >>>>>> petit bricolage pour la boite de cisaillement */

	h = yM - ym;
	Vt = M_PI * rc * rc * h;
	if (Vt != 0.0)
		C = volum / Vt;
	else
		C = 0.0;

	/* <<<<<<< */

	F_moy_pos = Strong_percent = Weak_percent = 0.0;
	if (nbi > 0) {
		for (i = 0; i < nbi; i++)
			if (Fn[i][state] > 0.0) {
				F_moy_pos += Fn[i][state];
			}
		F_moy_pos = F_moy_pos / (double) Nc_plus;

		for (i = 0; i < nbi; i++)
			if (Fn[i][state] > F_moy_pos)
				Strong_percent += 1.0;
			else
				Weak_percent += 1.0;


		Strong_percent = 100.0 * Strong_percent / (double) nbi;
		Weak_percent = 100.0 * Weak_percent / (double) nbi;
	}
	fprintf(stdout, "................................................\n");

#ifdef _FR

	fprintf(stdout, "  Temps : %lg\n", mgp_time[state]);
	fprintf(stdout, "  Nombre de corps : %d\n\n", nbel);

	fprintf(stdout, "  Nombre d'interactions : %d\n", nbi);
	fprintf(stdout, "  Nombre de contact avec Fn > 0 : %d\n", Nc_plus);
	fprintf(stdout, "  Nombre de contact avec Fn < 0 : %d\n\n", Nc_minus);

	fprintf(stdout, "  Rayon moyen (initial) : %lg\n", r_moy);

	fprintf(stdout, "  Limites : Xmin = %lg\tXmax = %lg\n", xm - xvec, xM - xvec);
	fprintf(stdout, "            Ymin = %lg\tYmax = %lg\n", ym - yvec, yM - yvec);
	fprintf(stdout, "            Zmin = %lg\tZmax = %lg\n\n", zm - zvec, zM - zvec);

	fprintf(stdout, "  Contact forts   : %lg%%\n", Strong_percent);
	fprintf(stdout, "  Contact faibles : %lg%%\n\n", Weak_percent);

	fprintf(stdout, "  Surface cumulee des grains (2D) : %lg\n", surf);
	fprintf(stdout, "  Volume cumule des grains (3D) : %lg\n", volum);

	if (C != 0.0)
		fprintf(stdout, "  Compacite de l'echantillon : %lg (e=%lg)\n", C, 1 / C - 1);

	fprintf(stdout, "  Hauteur de l'echantillon : %lg\n", h);

#else

	fprintf(stdout, "  Time : %lg\n", mgp_time[state]);
	fprintf(stdout, "  Number of bodies : %d\n", nbel);

	fprintf(stdout, "  Number of interactions : %d\n", nbi);
	fprintf(stdout, "  Number of contact with Fn > 0 : %d\n", Nc_plus);
	fprintf(stdout, "  Number of contact with Fn < 0 : %d\n", Nc_minus);

	fprintf(stdout, "  Mean Raduis (initial) : %lg\n", r_moy);

	fprintf(stdout, "  Boundaries : Xmin = %lg\tXmax = %lg\n", xm - xvec, xM - xvec);
	fprintf(stdout, "               Ymin = %lg\tYmax = %lg\n", ym - yvec, yM - yvec);
	fprintf(stdout, "               Zmin = %lg\tZmax = %lg\n\n", zm - zvec, zM - zvec);

	fprintf(stdout, "  Strongs contacts : %lg%%\n", Strong_percent);
	fprintf(stdout, "  Weaks contacts   : %lg%%\n\n", Weak_percent);

	fprintf(stdout, "  Cumulated surface of grains (2D) : %lg\n", surf);
	fprintf(stdout, "  Cumulated volume of grains (3D) : %lg\n", volum);

#endif

	fprintf(stdout, "................................................\n");
}

void 
tool_output_positions()
{
	int             i;
	FILE           *outfile;

	outfile = fopen("pts.txt", "w");
	fprintf(outfile, "%d\n", nbel);
	for (i = 0; i < nbel; i++) {
		fprintf(outfile, "%lg %lg\n", x[i][state], y[i][state]);
	}
	fclose(outfile);
	fprintf(stdout, "File 'pts.txt' created\n");
}

void 
tool_surf_vol()
{				/* MEMO a supprimer */
	int             i;
	int             type;
	double          surf = 0.0, volum = 0.0;

	for (i = 0; i < nbel; i++) {
		type = bdyty[i];
		switch (type) {
		case 0:
			surf += MG_PI * pow(radius[i][0], 2.0);
			break;
		case 1:
			volum += 1.333333 * MG_PI * pow(radius[i][0], 3.0);
			break;
		default:
			break;
		}
	}
	fprintf(stdout, "cumulated surface: %le\n", surf);
	fprintf(stdout, "cumulated volume: %le\n", volum);
}

void 
tool_anisotropy(int cstate, int save, FILE * out)
{
	int             i, j, k, current_adh = 0, anta;
	double          pijx, pijy, pijz;
	double          inv_norm_pij;
	double          n1, n2, n3;
	int             Nc;
	double          Fmoy;
	float         **Fabric;
	double          a1, a2, a3;
	float          *d;
	float           p;
	float         **v;
	int             nrot;

	no_warranty();

	Fabric = matrix(1, 3, 1, 3);	/* in nrutil */
	v = matrix(1, 3, 1, 3);
	d = vector(1, 3);

	for (i = 1; i <= 3; i++)
		for (j = 1; j <= 3; j++)
			Fabric[i][j] = 0.0;

	Nc = 0;
	Fmoy = 0.0;
	for (i = 0; i < nbel; i++)
		for (j = 0; j < nbneighbors[i][cstate]; j++)
			if (Fn[i][cstate] > 0.0) {
				Nc++;
				Fmoy += Fn[i][cstate];
			}
	if (Nc != 0) {
		Fmoy = Fmoy / Nc;
		/* calcul du tenseur de fabrique */
		for (i = 0; i < nbel; i++)
			for (j = 0; j < nbneighbors[i][cstate]; j++) {
				anta = neighbor[current_adh][cstate] - 1;
				if (Fn[current_adh][cstate] > /* Fmoy */ 0.0) {
					pijx = x[i][cstate] - x[anta][cstate];
					pijy = y[i][cstate] - y[anta][cstate];
					pijz = z[i][cstate] - z[anta][cstate];

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
				current_adh++;
			}

		/* Symetrie du tenseur */
		Fabric[2][1] = Fabric[1][2];
		Fabric[3][1] = Fabric[1][3];
		Fabric[3][2] = Fabric[2][3];

		/* division par nb de contact */
		for (i = 1; i <= 3; i++)
			for (j = 1; j <= 3; j++)
				Fabric[i][j] = Fabric[i][j] / Nc;

		/* affichage du tenseur */
		fprintf(stdout, "\nFabric tensor:\n");
		fprintf(stdout, "%14.7f\t%14.7f\t%14.7f\n", Fabric[1][1], Fabric[1][2], Fabric[1][3]);
		fprintf(stdout, "%14.7f\t%14.7f\t%14.7f\n", Fabric[2][1], Fabric[2][2], Fabric[2][3]);
		fprintf(stdout, "%14.7f\t%14.7f\t%14.7f\n", Fabric[3][1], Fabric[3][2], Fabric[3][3]);

		if (save == 1) {
			fprintf(out, "%lg\n", mgp_time[cstate]);
			fprintf(out, "%14.7f\t%14.7f\t%14.7f\n", Fabric[1][1], Fabric[1][2], Fabric[1][3]);
			fprintf(out, "%14.7f\t%14.7f\t%14.7f\n", Fabric[2][1], Fabric[2][2], Fabric[2][3]);
			fprintf(out, "%14.7f\t%14.7f\t%14.7f\n", Fabric[3][1], Fabric[3][2], Fabric[3][3]);
		}
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

		fprintf(stdout, "\nEigenvalues (descending order):\n");
		fprintf(stdout, "F1 = %lg\n", d[1]);
		fprintf(stdout, "F2 = %lg\n", d[2]);
		fprintf(stdout, "F3 = %lg\n", d[3]);

		/* affichage du tenseur */
		fprintf(stdout, "\nEigenvectors (same order as eigenvalues):\n");
		fprintf(stdout, "%14.7f\t%14.7f\t%14.7f\n", v[1][1], v[1][2], v[1][3]);
		fprintf(stdout, "%14.7f\t%14.7f\t%14.7f\n", v[2][1], v[2][2], v[2][3]);
		fprintf(stdout, "%14.7f\t%14.7f\t%14.7f\n", v[3][1], v[3][2], v[3][3]);

		/* affichage des valeurs caract�ristiques de l'anisotropie */
		a1 = 2.0 * (d[1] - d[2]);
		a2 = 2.0 * (d[2] - d[3]);
		a3 = 2.0 * (d[1] - d[3]);
		fprintf(stdout, "\nAnisotropies:\n");
		fprintf(stdout, "a1 = 2(F1-F2) = %lg\n", a1);
		fprintf(stdout, "a2 = 2(F2-F3) = %lg\n", a2);
		fprintf(stdout, "a3 = 2(F1-F3) = %lg\n", a3);

		fprintf(stdout, "direction principale: H=%lg  V=%lg\n", /* fabs */ ((180.0 / M_PI) * atan(v[2][1] / v[1][1])),
		     /* fabs */ ((180.0 / M_PI) * atan(v[3][1] / v[1][1])));

		if (save == 1) {
			fprintf(out, "%lg %lg %lg\n", d[1], d[2], d[3]);
			fprintf(out, "%14.7f\t%14.7f\t%14.7f\n", v[1][1], v[1][2], v[1][3]);
			fprintf(out, "%14.7f\t%14.7f\t%14.7f\n", v[2][1], v[2][2], v[2][3]);
			fprintf(out, "%14.7f\t%14.7f\t%14.7f\n", v[3][1], v[3][2], v[3][3]);
			/* fprintf (out, "%lg %lg %lg\n", a1, a2, a3 ); */

			fprintf(out, "\n");
		}
	} else
		fprintf(stdout, "No contact !!\n");

	free_matrix(Fabric, 1, 3, 1, 3);
	free_matrix(v, 1, 3, 1, 3);
	free_vector(d, 1, 3);
}


