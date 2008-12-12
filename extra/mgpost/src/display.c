/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

void 
accFrustum(GLdouble left, GLdouble right, GLdouble bottom,
	   GLdouble top, GLdouble nnear, GLdouble ffar, GLdouble pixdx,
	   GLdouble pixdy, GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
	GLdouble        xwsize, ywsize;
	GLdouble        dx, dy;
	GLint           viewport[4];

	glGetIntegerv(GL_VIEWPORT, viewport);

	xwsize = right - left;
	ywsize = top - bottom;

	dx = -(pixdx * xwsize / (GLdouble) viewport[2] + eyedx * nnear / focus);
	dy = -(pixdy * ywsize / (GLdouble) viewport[3] + eyedy * nnear / focus);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(left + dx, right + dx, bottom + dy, top + dy, nnear, ffar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-eyedx, -eyedy, 0.0);
}

void 
accPerspective(GLdouble fovy, GLdouble aspect,
	       GLdouble nnear, GLdouble ffar, GLdouble pixdx, GLdouble pixdy,
	       GLdouble eyedx, GLdouble eyedy, GLdouble focus)
{
	GLdouble        fov2, left, right, bottom, top;

	fov2 = ((fovy * M_PI) / 180.0) / 2.0;

	top = nnear / (cos(fov2) / sin(fov2));
	bottom = -top;

	right = top * aspect;
	left = -right;

	accFrustum(left, right, bottom, top, nnear, ffar,
		   pixdx, pixdy, eyedx, eyedy, focus);
}

void 
affichage()
{
	GLint           viewport[4];
	int             jitter;
  
	if (disp_only_text) 
    {
		disp_text(quick_text);
		disp_only_text = MG_FALSE;
		glFlush();
		glutSwapBuffers();
		/* mgp_delay(7000); */
		sleep(1.0);
    }
  
	if (modeTrace)
		glClear(GL_DEPTH_BUFFER_BIT);
	else
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	glLightfv(GL_LIGHT0, GL_POSITION, L0pos);
	glLightfv(GL_LIGHT1, GL_POSITION, L1pos);

  if (rendu_fluid != NULL)
	  rendu_fluid();
  
	if (rendu != NULL) 
    {
		if (!antialiased)
			rendu();
		else 
      {
			glGetIntegerv(GL_VIEWPORT, viewport);
			glClear(GL_ACCUM_BUFFER_BIT);
			for (jitter = 0; jitter < ACSIZE; jitter++) 
        {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				accPerspective(45.0f, 1.0f, 0.1f, DISTANCE_MAX,
                       j8[jitter].x, j8[jitter].y, 0.0f, 0.0f, 1.0f);
				rendu();
				glAccum(GL_ACCUM, 1.0 / ACSIZE);
        }
			glAccum(GL_RETURN, 1.0f);
      
			antialiased = MG_FALSE;
      }
    }
  
  if (rendu_sup != NULL)
	  rendu_sup();
  
	if (afficheRepere) 
    {
		glPushMatrix();
		glScalef(2.2 * maxmax * adim,
             2.2 * maxmax * adim,
             2.2 * maxmax * adim);
		glCallList(repere);
		glPopMatrix();
    }
  
	if (afftime)
		disp_time();
  
	if (affgraphic)
		affgraph_func();
  
	if (affFuncname)
		disp_func_name();
  
	glFlush();
  
	glutSwapBuffers();
}

void 
play()
{
	const char     *name[12];
  
	if (state < nb_state - 1) 
    {
		state++;
		glutPostRedisplay();
		if (sauve_anim) 
      {
			sprintf((char *) name, "./ani%04d.tif", state);
			writetiff((char *) name, "mgpost", 0, 0, W, H, COMPRESSION_PACKBITS);
      }
    } 
  else 
    {
		glutIdleFunc(NULL);
		sauve_anim = 0;
    }
}

void 
next_state()
{
	int i, j;
  
	nfile++;
	sprintf((char *) numfile, num_file_format, nfile);
	if (his_mode) strcpy((char *) datanewfilename, "spl_nwk_");
	else strcpy((char *) datanewfilename, "mgp.out.");
	strcat((char *) datanewfilename, (const char *) numfile);
	if (his_mode) strcat((char *) datanewfilename, ".his");
	if (fgziped && !his_mode)
		strcat((char *) datanewfilename, ".gz");
  
	if (!access((const char *) datanewfilename, F_OK)) 
    {
    
    /* 		if (multifiles) { */
    /* 			for (i = 0; i < nbel; i++) { */
    /* 				x_0[i] = x[i][state]; */
    /* 				y_0[i] = y[i][state]; */
    /* 				z_0[i] = z[i][state]; */
    /* 			} */
    /* 		} */
		strcpy(datafilename, (const char *) datanewfilename);
		nullifyCumulatedTabs();
		if (his_mode) charger_HISfile();
		else charger_geometrie();
    
		for (i = 0; i < nbel; i++)
      {
			for (j = 0; j < nb_state; j++) 
        {
				x[i][j] -= xvec;
				y[i][j] -= yvec;
				z[i][j] -= zvec;
        }
      }
    } 
  else 
    {
    nfile--;
    play_again = MG_FALSE;
    sauve_anim = MG_FALSE;
    }
  
}

void 
previous_state()
{
	int  i, j;
  
	nfile--;
	sprintf((char *) numfile, num_file_format, nfile);
	if (his_mode) strcpy((char *) datanewfilename, "spl_nwk_");
	else strcpy((char *) datanewfilename, "mgp.out.");
	strcat((char *) datanewfilename, (const char *) numfile);
    if (his_mode) strcat((char *) datanewfilename, ".his");
	if (fgziped && !his_mode)
		strcat((char *) datanewfilename, ".gz");
  
	if (!access((const char *) datanewfilename, F_OK)) 
    {
    
		if (multifiles) 
      {
			for (i = 0; i < nbel; i++) 
        {
				x_0[i] = x[i][state];
				y_0[i] = y[i][state];
				z_0[i] = z[i][state];
        }
      }
		strcpy(datafilename, (const char *) datanewfilename);
		nullifyCumulatedTabs();
		if (his_mode) charger_HISfile();
		else charger_geometrie();
    
		for (i = 0; i < nbel; i++)
			for (j = 0; j < nb_state; j++) 
        {
				x[i][j] -= xvec;
				y[i][j] -= yvec;
				z[i][j] -= zvec;
        }
        state = 0;
    } 
  else nfile++;
  
}

void 
play_filetofile()
{
	const char     *name[12];

	if (play_again) {
		if (sauve_anim) {
			sprintf((char *) name, "./ani%04d.tif", nfile);
			writetiff((char *) name, "mgpost", 0, 0, W, H, COMPRESSION_PACKBITS);
		}
		next_state();

		findBoundaries(state, &xminB, &yminB, &zminB,
			       &xmaxB, &ymaxB, &zmaxB);

		glutPostRedisplay();
	} else {
		glutIdleFunc(NULL);
		sauve_anim = MG_FALSE;
	}
}

void 
redim(int l, int h)
{

	W = l;
	H = h;

	if (W < H)
		glViewport((W - H) / 2, 0, H, H);
	else
		glViewport(0, (H - W) / 2, W, W);

}

void 
creatDisplayLists()
{
	int             i;

	/* global frame */
	repere = glGenLists(1);
	glNewList(repere, GL_COMPILE);
	glDisable(GL_LIGHTING);
	glLineWidth(2.0f);

	/* X */
	glColor3f(1.0f, 0.0f, 0.0f);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(1.0f, 0.0f, 0.0f);
	for (i = 0; i <= 360; i += 15)
		glVertex3f(0.9, 0.05 * cos(i * MG_DEG2RAD), 0.05 * sin(i * MG_DEG2RAD));
	glEnd();

	/* Z */
	glColor3f(0.0f, 0.0f, 1.0f);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, -1.0f, 0.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, -1.0f, 0.0f);
	for (i = 0; i <= 360; i += 15)
		glVertex3f(0.05 * cos(i * MG_DEG2RAD), -0.9, 0.05 * sin(i * MG_DEG2RAD));
	glEnd();

	/* Y */
	glColor3f(0.0f, 1.0f, 0.0f);

	glBegin(GL_LINES);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glVertex3f(0.0f, 0.0f, 1.0f);
	for (i = 0; i <= 360; i += 15)
		glVertex3f(0.05 * cos(i * MG_DEG2RAD), 0.05 * sin(i * MG_DEG2RAD), 0.9);
	glEnd();

	glEnable(GL_LIGHTING);
	glEndList();

	/* stick point */
	stickpoint = glGenLists(2);
	glNewList(stickpoint, GL_COMPILE);
	glDisable(GL_LIGHTING);
	glPointSize(2.0f);
	glBegin(GL_POINTS);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glEnd();
	glEnable(GL_LIGHTING);
	glEndList();
}



