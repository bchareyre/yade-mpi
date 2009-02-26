/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/* Ce fichier contient les fonctions de dessin
 * (ATTENTION, bcp de choses sont bricolees)
 * V. Richefeu
 */

/* ordre composantes = X,-Z, Y  */

void disp_points ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);

  glDisable (GL_LIGHTING);
  glPointSize (1.0f);

  glBegin (GL_POINTS);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section))
      glVertex3f (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);

  glEnd ();

  if (bodies_numbers)
    for (i = 0; i < nbel; i++)
      bodies_number (i, state);

  glEnable (GL_LIGHTING);
}

void disp_boundaries ()
{
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  char txt[128];
  int ii;
      
  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);

  glLineWidth (1.0f);

  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  glBegin (GL_LINE_LOOP);
  glVertex3f (xminB * adim, -zminB * adim, yminB * adim);
  glVertex3f (xminB * adim, -zminB * adim, ymaxB * adim);
  glVertex3f (xmaxB * adim, -zminB * adim, ymaxB * adim);
  glVertex3f (xmaxB * adim, -zminB * adim, yminB * adim);
  glVertex3f (xminB * adim, -zminB * adim, yminB * adim);
  glEnd();

  glBegin(GL_LINE_LOOP);
  glVertex3f (xminB * adim, -zmaxB * adim, yminB * adim);
  glVertex3f (xminB * adim, -zmaxB * adim, ymaxB * adim);
  glVertex3f (xmaxB * adim, -zmaxB * adim, ymaxB * adim);
  glVertex3f (xmaxB * adim, -zmaxB * adim, yminB * adim);
  glVertex3f (xminB * adim, -zmaxB * adim, yminB * adim);
  glEnd();

  glBegin (GL_LINES);
  glVertex3f (xminB * adim, -zminB * adim, yminB * adim);
  glVertex3f (xminB * adim, -zmaxB * adim, yminB * adim);

  glVertex3f (xminB * adim, -zminB * adim, ymaxB * adim);
  glVertex3f (xminB * adim, -zmaxB * adim, ymaxB * adim);

  glVertex3f (xmaxB * adim, -zminB * adim, ymaxB * adim);
  glVertex3f (xmaxB * adim, -zmaxB * adim, ymaxB * adim);

  glVertex3f (xmaxB * adim, -zminB * adim, yminB * adim);
  glVertex3f (xmaxB * adim, -zmaxB * adim, yminB * adim);
  glEnd ();

  
  sprintf(txt,"(0,0,0)");
  glRasterPos3f (xminB * adim, -zminB * adim, yminB * adim);
  for (ii = 0; txt[ii]; ii++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, txt[ii]);
  
  
  glEnable (GL_LIGHTING);
}

void disp_outline () /* Only for spheres */
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  GLint ndiv1=(GLint)(.5*nb_subdiv_sphere),ndiv2=(GLint)(.5*nb_subdiv_sphere_2);
  
  glPushMatrix ();
  glLineWidth (1.0f);
  
  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);
  glColor3f (.0f,.0f,.0f);
  
  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section) && bdyty[i]==MGP_SPHER)
      {
      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
      
      glutSolidSphere (radius[i][state]*adim*1.2, ndiv1, ndiv2);
      }
      
      glColor3f (1.0f,1.0f,1.0f);
  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section) && bdyty[i]==MGP_SPHER)
      {
      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
      
      glutSolidSphere (radius[i][state]*adim, ndiv1, ndiv2);
      }
      
      glPopMatrix ();
  
  if(!mode2D)
    {
    glEnable (GL_LIGHTING);
    glEnable (GL_DEPTH_TEST);
    }
}


void disp_obstacles ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  
  glPushMatrix ();
  glLineWidth (1.0f);
  for (i = 0; i < nbel; i++)
    {
    glLoadIdentity ();
    gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef (phi, 1.0f, 0.0f, 0.0f);
    glRotatef (theta, 0.0f, 0.0f, 1.0f);
    glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
    mgobstselect(i,state);
    }
  glPopMatrix ();
}

void disp_fluid_ux ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  
  if (!with_fluid) return;
  
  valc_bleu = 0.0;
  if (dynamic_scale)
    {
    valc_bleu = FluidCell[0].ux;
    for (i=1;i<nbFluidCells;++i)
      if (valc_bleu > FluidCell[i].ux) valc_bleu = FluidCell[i].ux;
    
    valc_rouge = FluidCell[0].ux;
    for (i=1;i<nbFluidCells;++i)
      if (valc_rouge < FluidCell[i].ux) valc_rouge = FluidCell[i].ux;
    }
  
  
  glPushMatrix ();
  glLineWidth (1.0f);
  
  for (i = 0 ; i < nbFluidCells ; i++)
    {
    
    coul = (int) (nb_val_couleurs * (FluidCell[i].ux - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
    if (coul < 0)
      coul = 0;
    if (coul > nb_val_couleurs)
      coul = nb_val_couleurs;
    
    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
    
    glLoadIdentity ();
    gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef (phi, 1.0f, 0.0f, 0.0f);
    glRotatef (theta, 0.0f, 0.0f, 1.0f);
    glTranslatef ((FluidCell[i].x-xvec) * adim, 0.0f, (FluidCell[i].y-yvec) * adim);
    
    mgFluidCell(i);
    
    }
  
    glPopMatrix ();
}

void disp_fluid_uy ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  
  if (!with_fluid) return;
  
  valc_bleu = 0.0;
  if (dynamic_scale)
    {
    valc_bleu = FluidCell[0].uy;
    for (i=1;i<nbFluidCells;++i)
      if (valc_bleu > FluidCell[i].ux) valc_bleu = FluidCell[i].ux;
    
    valc_rouge = FluidCell[0].uy;
    for (i=1;i<nbFluidCells;++i)
      if (valc_rouge < FluidCell[i].ux) valc_rouge = FluidCell[i].ux;
    }
  
  
  glPushMatrix ();
  glLineWidth (1.0f);
  
  for (i = 0 ; i < nbFluidCells ; i++)
    {
    
    coul = (int) (nb_val_couleurs * (FluidCell[i].uy - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
    if (coul < 0)
      coul = 0;
    if (coul > nb_val_couleurs)
      coul = nb_val_couleurs;
    
    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
    
    glLoadIdentity ();
    gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef (phi, 1.0f, 0.0f, 0.0f);
    glRotatef (theta, 0.0f, 0.0f, 1.0f);
    glTranslatef ((FluidCell[i].x-xvec) * adim, 0.0f, (FluidCell[i].y-yvec) * adim);
    
    mgFluidCell(i);
    }
  
  glPopMatrix ();
}

void disp_fluid_normeu ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  
  if (!with_fluid) return;
  
  valc_bleu = 0.0;
  if (dynamic_scale)
    {
    valc_rouge = FluidCell[0].ux*FluidCell[0].ux+FluidCell[0].uy*FluidCell[0].uy;
    for (i=1;i<nbFluidCells;++i)
      if (valc_rouge < FluidCell[i].ux*FluidCell[i].ux+FluidCell[i].uy*FluidCell[i].uy) 
        valc_rouge = FluidCell[i].ux*FluidCell[i].ux+FluidCell[i].uy*FluidCell[i].uy;
    valc_rouge = sqrt(valc_rouge);
    }
  
  
  glPushMatrix ();
  glLineWidth (1.0f);
  
  for (i = 0 ; i < nbFluidCells ; i++)
    {
    
    coul = (int) (nb_val_couleurs * ((sqrt(FluidCell[i].ux*FluidCell[i].ux+FluidCell[i].uy*FluidCell[i].uy)) - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
    if (coul < 0)
      coul = 0;
    if (coul > nb_val_couleurs)
      coul = nb_val_couleurs;
    
    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
    
    glLoadIdentity ();
    gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef (phi, 1.0f, 0.0f, 0.0f);
    glRotatef (theta, 0.0f, 0.0f, 1.0f);
    glTranslatef ((FluidCell[i].x-xvec) * adim, 0.0f, (FluidCell[i].y-yvec)* adim);
    
    mgFluidCell(i);
    }
  
  glPopMatrix ();
}

void disp_fluid_p ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  
  if (!with_fluid) return;
  
  valc_bleu = 0.0;
  if (dynamic_scale)
    {
    valc_bleu = FluidCell[0].p;
    for (i=1;i<nbFluidCells;++i)
      if (valc_bleu > FluidCell[i].p) valc_bleu = FluidCell[i].p;   
    
    valc_rouge = FluidCell[0].p;
    for (i=1;i<nbFluidCells;++i)
      if (valc_rouge < FluidCell[i].p) valc_rouge = FluidCell[i].p;
    }
  
  
  glPushMatrix ();
  glLineWidth (1.0f);
  
  for (i = 0 ; i < nbFluidCells ; i++)
    {
    
    coul = (int) (nb_val_couleurs * (FluidCell[i].p - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
    if (coul < 0)
      coul = 0;
    if (coul > nb_val_couleurs)
      coul = nb_val_couleurs;
    
    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
    
    glLoadIdentity ();
    gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
    glRotatef (phi, 1.0f, 0.0f, 0.0f);
    glRotatef (theta, 0.0f, 0.0f, 1.0f);
    glTranslatef ((FluidCell[i].x-xvec) * adim, 0.0f, (FluidCell[i].y-yvec) * adim);
    
    mgFluidCell(i);
    }
  
  glPopMatrix ();
}

void disp_geo_layers ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  /*if (!layers_defined) return;*/
  
  glPushMatrix ();
  glLineWidth (1.0);
  if (mode2D)
    glDisable (GL_LIGHTING);
  
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
      
      if (layer[i] % 2)
        glColor3f (0.7f, 1.0f, 0.7f);
      else
        glColor3f (0.0f, 0.5f, 0.25f);
      
      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
      
      mgbodyselect_plein (i, state);

      }
      glEnable (GL_LIGHTING);
  glPopMatrix ();
  
  if ((affgradcolor)&&(renderMode == GL_RENDER))
    disp_grad_color ();
}


void disp_sizes () 
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul = 0;
  
  if (dynamic_scale)
    {
    valc_rouge = findAbsMax ((double **) radius, nbel, state);    
    valc_bleu = findAbsMin ((double **) radius, nbel, state);
    }
  
  glPushMatrix ();
  glLineWidth (1.0);
  if (mode2D)
    glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
      /*
      coul = (int) (nb_val_couleurs * (radius[i][state] - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
      if (coul < 0)
        coul = 0;
      if (coul > nb_val_couleurs)
        coul = nb_val_couleurs;
      
      glColor4f (gradc[coul].r, gradc[coul].v, gradc[coul].b, alpha_color);
       */
      
      /* bricolage temporaire */
      if (radius[i][state] > 0.4e-3) glColor3f (1.0f,1.0f,1.0f);
      else  glColor3f (0.0f,0.0f,0.0f);
      
      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
      
      mgbodyselect_plein (i, state);
      if (mgterminal == PS_TERMINAL)
        mgbodyselectPS_plein (i, state, coul);
      }
      glEnable (GL_LIGHTING);
  glPopMatrix ();
  
  if ((affgradcolor)&&(renderMode == GL_RENDER))
    disp_grad_color ();
  
}

void disp_shapes () 
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  glPushMatrix ();
  glLineWidth (1.0f);

  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);
	glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);

	mgbodyselect (i, state);

	if (mgterminal == PS_TERMINAL)
	  mgbodyselectPS (i, state);

	if (mgterminal == POV_TERMINAL)
	  mgbodyselectPOV (i, state);

      }
  glPopMatrix ();
}

void disp_discrim ()
{
  int i;
  int coul;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  glPushMatrix ();
  glLineWidth (1.0f);
  
  if (mode2D)
    glDisable (GL_LIGHTING);

  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section))
      {
	if (discrim[i])
	  {
	  glColor3f (fg_color.r, fg_color.v, fg_color.b);
	  coul = 63; 
	  }
	else
	  {
	  glColor3f (bg_color.r, bg_color.v, bg_color.b);
	  coul = 0; 
	  }

	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);
	glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);

	mgbodyselect_plein (i, state);

	if (mgterminal == PS_TERMINAL)
	  mgbodyselectPS_plein (i, state, coul);
      }
  glPopMatrix ();
}

void disp_colors ()
{
  int i;
  int coul;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  
  glPushMatrix ();
  glLineWidth (1.0f);
  
  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section))
      {
      coul = color[i];
      if (!colIsShown[coul]) continue;
      else if (coul < 0 || coul > 5) continue;
      glColor4f (rcolor[coul], gcolor[coul], bcolor[coul], alpha_color);

      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
      
      mgbodyselect_plein (i, state);
      
      if (mgterminal == PS_TERMINAL)
        mgbodyselectPS_plein (i, state, coul);
      }
      glPopMatrix ();
}

void disp_force_colorlines ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  
  if (dynamic_scale)
    {
    valc_rouge = findMax ((double **) Fn, nbel, state); /* nbel ??? */
    valc_bleu = findMin ((double **) Fn, nbel, state);
    }
  
  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);
  
  glLineWidth (2.0f);
  for (i = 0 ; i < nbel ; ++i)
    if (pres_du_plan (i, dist_section))
      {
      
      coul = (int) (nb_val_couleurs * (Fn[i][state] - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
      if (coul < 0)
        coul = 0;
      if (coul > nb_val_couleurs)
        coul = nb_val_couleurs;
      
      glColor4f (gradc[coul].r, gradc[coul].v, gradc[coul].b, alpha_color);    
      
      for (ii = 0; ii < nbneighbors[i][state]; ++ii)
        {
        anta = neighbor[current_adh][state] - 1;
        if (pres_du_plan (anta, dist_section))
          {
          glBegin (GL_LINES);
          glVertex3f (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
          glVertex3f (x[anta][state] * adim, -z[anta][state] * adim, y[anta][state] * adim);
          glEnd ();
          }
        current_adh++;
        }
      }
      else
        {
        current_adh += nbneighbors[i][state];
        }
}


void disp_network () /* liste de Verlet */
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);

  //glLineWidth (2.0f);
  for (i = 0 ; i < nbel ; ++i)
    if (pres_du_plan (i, dist_section))
      {       
	for (ii = 0; ii < nbneighbors[i][state]; ++ii)
	  {
	    anta = neighbor[current_adh][state] - 1;
	    if (pres_du_plan (anta, dist_section))
	      {
                glLineWidth (2.0f);
                glColor3f (0.0f,0.0f,0.0f);
                
		glBegin (GL_LINES);
		glVertex3f (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
		glVertex3f (x[anta][state] * adim, -z[anta][state] * adim, y[anta][state] * adim);
		glEnd ();
	      }
	    current_adh++;
	  }
      }
    else
      {
	current_adh += nbneighbors[i][state];
      }
}

void disp_cracks ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  double nx, ny, tx ,ty, d, rmin;
  double xc,yc;
  
  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);

  glDisable (GL_LIGHTING);
  
  for (i = 0 ; i < nbel ; ++i)
    if (pres_du_plan (i, dist_section))
  {       
    for (ii = 0; ii < nbneighbors[i][state]; ++ii)
    {
      anta = neighbor[current_adh][state] - 1;
      if (pres_du_plan (anta, dist_section))
      {
        rmin = (radius[i][state] < radius[anta][state])? radius[i][state] : radius[anta][state];
        if (statut[current_adh][state] != MGP_STT_STICK)
        { 
          nx = x[anta][state] - x[i][state];
          ny = y[anta][state] - y[i][state];
          d = 1.0/sqrt(nx*nx+ny*ny);
          nx *= d ; ny *= d;
          tx = -ny ; ty = nx;
          xc = 0.5*(x[anta][state] + x[i][state]);
          yc = 0.5*(y[anta][state] + y[i][state]);
          
          glLineWidth (2.0f);
          glColor3f (1.0f,0.0f,0.0f);
          glBegin (GL_LINES);
          glVertex3f ((xc-rmin*tx) * adim, 0.0f, (yc-rmin*ty) * adim);
          glVertex3f ((xc+rmin*tx) * adim, 0.0f, (yc+rmin*ty) * adim);
          glEnd ();
        }
        else 
        {
          glLineWidth (4.0f);
          glColor3f (1.0f,1.0f,1.0f);
          glBegin (GL_LINES);
          glVertex3f (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
          glVertex3f (x[anta][state] * adim, -z[anta][state] * adim, y[anta][state] * adim);
          glEnd ();
        }
                
        
      }
      
      current_adh++;
    }
  }
  else
  {
    current_adh += nbneighbors[i][state];
  }
  
  glEnable (GL_LIGHTING);
}



void disp_stick_links ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  glLineWidth (4.0f);
  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	glColor3f (0.0f,0.0f,1.0f);

	for (ii = 0; ii < nbneighbors[i][state]; ++ii)
	  {
	    anta = neighbor[current_adh][state] - 1;

	    glBegin (GL_LINES);
	    if (statut[current_adh][state] == MGP_STT_STICK)
	      {
	    glVertex3f (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
	    glVertex3f (x[anta][state] * adim, -z[anta][state] * adim, y[anta][state] * adim);
	      }
	    glEnd ();
	    current_adh++;
	  }
      }
}

void disp_coord_number ()
{
  int i, ii, k;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  int *NC;

  NC = (int *) calloc ((size_t) nbel, sizeof (int));

  k = 0;
  for (i = 0; i < nbel; ++i)
    {
    for (ii = 0 ; ii < nbneighbors[i][state]; ++ii)
      {
      if(Fn[k][state]>0.)
        {
        NC[i] += 1;
        NC[neighbor[k][state] - 1] += 1;
        }
	    ++k;
      }
    }

  if (dynamic_scale)
    {
      valc_bleu = 0.0;
      valc_rouge = 12.0; /* provisoir */
    }

  glPushMatrix ();
  glLineWidth (1.0);
  if (mode2D)
    glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	coul = (int) (nb_val_couleurs * ((double) NC[i] - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
	if (coul < 0)
	  coul = 0;
	if (coul > nb_val_couleurs)
	  coul = nb_val_couleurs;

	glColor4f (gradc[coul].r, gradc[coul].v, gradc[coul].b, alpha_color);
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);
	glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);

	mgbodyselect_plein (i, state);
	if (mgterminal == PS_TERMINAL)
	  mgbodyselectPS_plein (i, state, coul);
      }
  glEnable (GL_LIGHTING);
  glPopMatrix ();

  if ((affgradcolor)&&(renderMode == GL_RENDER))
    disp_grad_color ();

  free(NC);

}


void disp_tensile_compressive_forces_2d ()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, ls, lc, sina, cosa;
  double val_max = 0.0;
  double lmin = r_moy * 0.04;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  if (dynamic_scale)
    {
     for (i = 0; i < nbel; i++) nbtac += nbneighbors[i][state];
     valc_bleu  = findMin ((double **) Fn, nbtac, state); 
     valc_rouge = findMax ((double **) Fn, nbtac, state);
    }
    
  val_max = (fabs(valc_rouge) > fabs(valc_bleu)) ? fabs(valc_rouge) : fabs(valc_bleu);

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  glEnable (GL_DEPTH_TEST);
  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section))
      {
        glLoadIdentity ();
        gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
        glRotatef (phi, 1.0f, 0.0f, 0.0f);
        glRotatef (theta, 0.0f, 0.0f, 1.0f);

        for (ii = 0; ii < nbneighbors[i][state]; ii++)
          {
            anta = neighbor[current_adh][state] - 1;
            
            dist = sqrt (pow ((x[i][state] - x[anta][state]), 2.0) + pow ((y[i][state] - y[anta][state]), 2.0));
            cosa = (x[anta][state] - x[i][state]) / dist;
            sina = (y[anta][state] - y[i][state]) / dist;
            
            if (Fn[current_adh][state] > 0.0 && val_max > 0.0)
              {
                l = r_moy * ((Fn[current_adh][state]) / (val_max));
                l = (l > r_moy) ? r_moy : l;
                l = (l < lmin)  ? lmin  : l;
                lc = l * cosa;
                ls = l * sina;

                glColor3f (0.5f, 0.5f, 0.5f);

                glDisable (GL_LIGHTING);
                glBegin (GL_POLYGON);
                glVertex3f ((ls + x[i][state]) * adim, lmin*adim, (-lc + y[i][state]) * adim);
                glVertex3f ((ls + x[anta][state]) * adim, lmin*adim, (-lc + y[anta][state]) * adim);
                glVertex3f ((-ls + x[anta][state]) * adim, lmin*adim, (lc + y[anta][state]) * adim);
                glVertex3f ((-ls + x[i][state]) * adim, lmin*adim, (lc + y[i][state]) * adim);
                glEnd ();
                
                if (mgterminal == PS_TERMINAL)
                  mglinkPS(state, l, i, anta);
              }
              else if (Fn[current_adh][state] < 0.0 && val_max > 0.0)
              {
                l = r_moy * ((-Fn[current_adh][state]) / (val_max));
                l = (l > r_moy) ? r_moy : l;
                l = (l < lmin)  ? lmin  : l;
                lc = l * cosa;
                ls = l * sina;

                glColor3f (0.0f, 0.0f, 0.0f);
                
                glDisable (GL_LIGHTING);
                glBegin (GL_POLYGON);
                glVertex3f ((ls + x[i][state]) * adim, 0.0f, (-lc + y[i][state]) * adim);
                glVertex3f ((ls + x[anta][state]) * adim, 0.0f, (-lc + y[anta][state]) * adim);
                glVertex3f ((-ls + x[anta][state]) * adim, 0.0f, (lc + y[anta][state]) * adim);
                glVertex3f ((-ls + x[i][state]) * adim, 0.0f, (lc + y[i][state]) * adim);
                glEnd ();

                /*pastille2d(i,anta,2.0*l);*/

                if (mgterminal == PS_TERMINAL)
                  mglinkPS(state, l, i, anta);
              }
             
            current_adh++;
          }
 }

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}


void disp_positive_normal_strength_2d ()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, ls, lc, sina, cosa;
  double F_moy_pos,Fcut;
  static double Fmax = -1.0;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  valc_bleu = 0.0;
  if (dynamic_scale || Fmax < 0.0)
    {
     for (i = 0; i < nbel; i++)
       nbtac += nbneighbors[i][state];
     valc_rouge = Fmax = findPositiveMax ((double **) Fn, nbtac, state);
    }
  else
    {
      valc_rouge = Fmax;
    }  

  F_moy_pos = 0.0;
  if (sep_networks)
    {
      F_moy_pos = 0.0;
      ii=0;
      for (i = 0; i < nbtac; i++)
	if (Fn[i][state] > 0.0)
	  {
	    F_moy_pos += Fn[i][state];
	    ii++;
	  }
      F_moy_pos = F_moy_pos / (double)ii;
    }
  Fcut = Fcutlevel * F_moy_pos;/* a parametrer */

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; ++i)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;
	    if (Fn[current_adh][state] > 0.0)
	      {
		dist = sqrt (pow ((x[i][state] - x[anta][state]), 2.0) + pow ((y[i][state] - y[anta][state]), 2.0));
		cosa = (x[anta][state] - x[i][state]) / dist;
		sina = (y[anta][state] - y[i][state]) / dist;
		l = r_moy * ((Fn[current_adh][state]) / (valc_rouge)) * 0.5;
                l = (l > r_moy) ? r_moy : l;
		lc = l * cosa;
		ls = l * sina;

		if (!sep_networks)
		  {
		    if (affgradlinkcolor)
		      {
			coul = (int) (nb_val_couleurs * (Fn[current_adh][state]) / (valc_rouge)) + 1;
			if (coul < 0)
			  coul = 0;
			if (coul > nb_val_couleurs)
			  coul = nb_val_couleurs;
			glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		      }
		    else
		      glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);
		  }
		else
		  {
		    if (Fn[current_adh][state] > Fcut)
		      glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);
		    else
		      glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
		  }

		glBegin (GL_POLYGON);
		glVertex3f ((ls + x[i][state]) * adim, 0.0f, (-lc + y[i][state]) * adim);
		glVertex3f ((ls + x[anta][state]) * adim, 0.0f, (-lc + y[anta][state]) * adim);
		glVertex3f ((-ls + x[anta][state]) * adim, 0.0f, (lc + y[anta][state]) * adim);
		glVertex3f ((-ls + x[i][state]) * adim, 0.0f, (lc + y[i][state]) * adim);
		glEnd ();

		if (mgterminal == PS_TERMINAL)
		  mglinkPS(state, l, i, anta);
	      }
	    current_adh++;

	  }
      }

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}

/* Fonction special pour les forces entres polye lues dans les fichiers for.out.xxx */
/* BRICOLAGE */
void disp_special_forces_3d ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, a, b;
  double t,PX,PY,PZ,deltaA;
  double xij,yij,zij;
  double dst;
  
  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;
  
  valc_bleu = 0.0;
  if (dynamic_scale)
    {
    valc_rouge = Contact[0].fn; 
    for (i=1;i<nbContacts;++i)
      {
      if (valc_rouge < Contact[i].fn) valc_rouge = Contact[i].fn;
      }
    }
  
  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  
  deltaA = M_PI * 0.25;
  for (i = 0; i < nbContacts; i++)
    {
    
    dst = section.a * Contact[i].x 
    + section.b * Contact[i].y 
    + section.c * Contact[i].z 
    + section.d;
    
    if( !sectionActive || (sectionActive && (fabs(dst) < dist_section)) )
      {
      xij = Contact[i].nx;
      yij = Contact[i].ny;
      zij = Contact[i].nz;
      a = -atan(fabs(zij) / fabs(xij)) + M_PI;
      b = -atan(sqrt(pow(xij,2.0) + pow(zij,2.0))
                / fabs(yij));
      
      if (yij < 0.0)
        b = -(M_PI - b);
      else
        b=-b;
      
      if ((xij > 0.0) && (zij > 0.0))
        a = -a;
      
      if ((xij < 0.0) && (zij > 0.0))
        a= -(M_PI -a);
      
      if ((xij < 0.0) && (zij < 0.0))
        a = M_PI - a;
            
      
      dist = r_moy;
      l = r_moy * ((Contact[i].fn - valc_bleu) / (valc_rouge - valc_bleu)) * 0.5;
      l = (l > r_moy) ? r_moy : l;
      
      
      if (affgradlinkcolor)
        {
        coul = (int) (nb_val_couleurs * (Contact[i].fn) / (valc_rouge)) + 1;
        if (coul < 0)
          coul = 0;
        if (coul > nb_val_couleurs)
          coul = nb_val_couleurs;
        glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
        }
      else
        glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
      
      
      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      
      glPushMatrix();
      /*glTranslatef(x[i][state]*adim, -z[i][state]*adim, y[i][state]*adim);*/
      glTranslatef(Contact[i].x*adim, -Contact[i].z*adim, Contact[i].y*adim);
      glRotatef (b * MG_RAD2DEG, -sin (a), -cos (a), 0.0f);
      glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);
      
      for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
        {
        glBegin (GL_POLYGON);
        
        PX = l * sin(t);
        PZ = l * cos(t);
        PY = - 0.5*dist;
        glVertex3f (PX*adim, -PZ*adim, PY*adim);
        
        PX = l * sin(t+deltaA);
        PZ = l * cos(t+deltaA);
        glVertex3f (PX*adim, -PZ*adim, PY*adim);
        
        PY = 0.5*dist;
        glVertex3f (PX*adim, -PZ*adim, PY*adim);
        
        PX = l * sin(t);
        PZ = l * cos(t);
        glVertex3f (PX*adim, -PZ*adim, PY*adim);
        
        glEnd ();
        }
      glPopMatrix();
      }
    }
  
  
  glEnable (GL_LIGHTING);
  
  if (affgradlinkcolor)
    disp_grad_color ();
  
  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;
  
}

/* Fonction special pour les forces entres polye lues dans les fichiers for.out.xxx */
/* affichage comme le fait J J Moreau pour les polygones */
/* BRICOLAGE */
void disp_special_forces_lines_3d ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double l;
  double deltaA;
  double dst;
  
  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;
  
  valc_bleu = 0.0;
  if (dynamic_scale)
    {
    valc_rouge = Contact[0].fn; 
    for (i=1;i<nbContacts;++i)
      {
      if (valc_rouge < Contact[i].fn) valc_rouge = Contact[i].fn;
      }
    }
  
  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  
  deltaA = M_PI * 0.25;
  for (i = 0; i < nbContacts; i++)
    {
    
    dst = section.a * Contact[i].x 
    + section.b * Contact[i].y 
    + section.c * Contact[i].z 
    + section.d;
    

    if((sectionActive && (fabs(dst) < dist_section)) || !sectionActive)
      {
      
      l = r_moy * ((Contact[i].fn - valc_bleu) / (valc_rouge - valc_bleu)) * 2.;
      
      if (affgradlinkcolor)
        {
        coul = (int) (nb_val_couleurs * (Contact[i].fn) / (valc_rouge)) + 1;
        if (coul < 0)
          coul = 0;
        if (coul > nb_val_couleurs)
          coul = nb_val_couleurs;
        glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
        }
      else
        {
        glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
        }
    
      
      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      
      /*glPushMatrix();*/
      /*glTranslatef(x[i][state]*adim, -z[i][state]*adim, y[i][state]*adim);*/
      /*glTranslatef(Contact[i].x*adim, -Contact[i].z*adim, Contact[i].y*adim);
      glRotatef (b * MG_RAD2DEG, -sin (a), -cos (a), 0.0f);
      glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);*/
      

        glBegin (GL_LINES);
        
        glVertex3f( (Contact[i].x-Contact[i].nx*l) * adim,
                   -(Contact[i].z-Contact[i].nz*l) * adim, 
                    (Contact[i].y-Contact[i].ny*l) * adim);
        glVertex3f( (Contact[i].x+Contact[i].nx*l) * adim,
                   -(Contact[i].z+Contact[i].nz*l) * adim, 
                    (Contact[i].y+Contact[i].ny*l) * adim);        

        
        glEnd ();
        
      /*glPopMatrix();*/
      }
    }
  
  
  glEnable (GL_LIGHTING);
  
  if (affgradlinkcolor)
    disp_grad_color ();
  
  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;
  
}

void disp_positive_normal_strength_3d ()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, a, b;
  double t,PX,PY,PZ,deltaA;
  double xij,yij,zij;
  double F_moy_pos,Fcut;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  for (i = 0; i < nbel; i++)
    nbtac += nbneighbors[i][state];

  valc_bleu = 0.0;
  if (dynamic_scale)
    {
      valc_rouge = findPositiveMax ((double **) Fn, nbtac, state);
    }

  F_moy_pos = 0.0;
  if (sep_networks)
    {
      F_moy_pos = 0.0;
      ii=0;
      for (i = 0; i < nbtac; i++)
	if (Fn[i][state] > 0.0)
	  {
	    F_moy_pos += Fn[i][state];
	    ii++;
	  }
      F_moy_pos = F_moy_pos / (double)ii;
    }
  Fcut = Fcutlevel*F_moy_pos;


  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {

	deltaA = M_PI * 0.25;
	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;
	    if  ((pres_du_plan (anta, dist_section)) && (Fn[current_adh][state] > 0.0))
	      {
		xij = x[anta][state] - x[i][state];
		yij = y[anta][state] - y[i][state];
		zij = z[anta][state] - z[i][state];
		a = -atan(fabs(zij) / fabs(xij)) + M_PI;
		b = -atan(sqrt(pow(xij,2.0) + pow(zij,2.0))
                          / fabs(yij));

		if (yij < 0.0)
		  b = -(M_PI - b);
		else
		  b=-b;

		if ((xij > 0.0) && (zij > 0.0))
		  a = -a;

		if ((xij < 0.0) && (zij > 0.0))
		  a= -(M_PI -a);

		if ((xij < 0.0) && (zij < 0.0))
		  a = M_PI - a;

		dist = sqrt (xij*xij + yij*yij + zij *zij );

		l = r_moy * ((Fn [current_adh][state] - valc_bleu) / (valc_rouge - valc_bleu)) * 0.75 ;
                l = (l > r_moy) ? r_moy : l;

		if (!sep_networks)
		  {

		    if (affgradlinkcolor)
		      {
			coul = (int) (nb_val_couleurs * (Fn[current_adh][state]) / (valc_rouge)) + 1;
			if (coul < 0)
			  coul = 0;
			if (coul > nb_val_couleurs)
			  coul = nb_val_couleurs;
			glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		      }
		    else
		      glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
		  }
		else
        {
          if (Fn[current_adh][state] > Fcut)
          {
			if (affgradlinkcolor)
            {
              coul = (int) (nb_val_couleurs * (Fn[current_adh][state]) / (valc_rouge)) + 1;
              if (coul < 0)
                coul = 0;
              if (coul > nb_val_couleurs)
                coul = nb_val_couleurs;
              glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
            }
			else
			  glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
          }
          else
          {
			/*glColor4f (mggris.r, mggris.v, mggris.b, 0.2f);
			l = 0.05 * r_moy;*/
            goto suite;
          }
        }

		glLoadIdentity ();
		gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef (phi, 1.0f, 0.0f, 0.0f);
		glRotatef (theta, 0.0f, 0.0f, 1.0f);

		glPushMatrix();
		glTranslatef(x[i][state]*adim, -z[i][state]*adim, y[i][state]*adim);
		glRotatef (b * MG_RAD2DEG, -sin (a), -cos (a), 0.0f);
		glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);

		for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
		  {
		    glBegin (GL_POLYGON);

		    PX = l * sin(t);
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, 0.0f);

		    PX = l * sin(t+deltaA);
		    PZ = l * cos(t+deltaA);
		    glVertex3f (PX*adim, -PZ*adim, 0.0f);

		    PY = dist;
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t);
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    glEnd ();
		  }
		glPopMatrix();
	      }
suite:
	    current_adh++;

	  }
      }
    else
      current_adh += nbneighbors[i][state];

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}

/* Version pour more_forces (dans fichier for.out.xxx) */
/* centre-centre */
void disp_positive_normal_strength_3d_v2 ()
{
  int i, ii, nbtac = 0, anta;
  int c;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, a, b;
  double t,PX,PY,PZ,deltaA;
  double xij,yij,zij;
  double F_moy_pos,Fcut;
  double dst;
  
  
  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;
  
  nbtac = nbContacts;
  
  valc_bleu = 0.0;
  if (dynamic_scale)
    {
    valc_rouge = Contact[0].fn;
    for (c=1;c<nbtac;c++) 
      valc_rouge = (valc_rouge<Contact[c].fn)?Contact[c].fn:valc_rouge;
    }
  
  F_moy_pos = 0.0;
  if (sep_networks)
    {
    F_moy_pos = 0.0;
    ii=0;
    for (i = 0; i < nbtac; i++)
      {
      if (Contact[i].fn > 0.0)
        {
        F_moy_pos += Contact[i].fn;
        ii++;
        }
      } 
    if (ii > 0) F_moy_pos = F_moy_pos / (double)ii;
    }
  Fcut = Fcutlevel * F_moy_pos;
  
  
  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  deltaA = M_PI * 0.25;
  
  for (c = 0; c < nbtac; c++)
    {
    i    = Contact[c].i - 1;
    anta = Contact[c].j - 1;
    
    dst = section.a * Contact[c].x 
        + section.b * Contact[c].y 
        + section.c * Contact[c].z 
        + section.d;
    
    if((sectionActive && (fabs(dst) < dist_section)) || !sectionActive)
      {
        if  (Contact[c].fn > 0.0)
          {
          xij = x[anta][state] - x[i][state];
          yij = y[anta][state] - y[i][state];
          zij = z[anta][state] - z[i][state];
          a = -atan(fabs(zij) / fabs(xij)) + M_PI;
          b = -atan(sqrt(pow(xij,2.0) + pow(zij,2.0))
                    / fabs(yij));
          
          if (yij < 0.0)
            b = -(M_PI - b);
          else
            b=-b;
          
          if ((xij > 0.0) && (zij > 0.0))
            a = -a;
          
          if ((xij < 0.0) && (zij > 0.0))
            a= -(M_PI -a);
          
          if ((xij < 0.0) && (zij < 0.0))
            a = M_PI - a;
          
          dist = sqrt (xij*xij + yij*yij + zij *zij );
          
          l = r_moy * ((Contact[c].fn - valc_bleu) / (valc_rouge - valc_bleu)) * 0.3 ;
          l = (l > r_moy) ? r_moy : l;
          
          if (!sep_networks)
            {
            
            if (affgradlinkcolor)
              {
              coul = (int) (nb_val_couleurs * (Contact[c].fn) / (valc_rouge)) + 1;
              if (coul < 0)
                coul = 0;
              if (coul > nb_val_couleurs)
                coul = nb_val_couleurs;
              glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
              }
            else
              glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
            }
          else
            {
            if (Contact[i].fn > Fcut)
              {
              if (affgradlinkcolor)
                {
                coul = (int) (nb_val_couleurs * (Contact[c].fn) / (valc_rouge)) + 1;
                if (coul < 0)
                  coul = 0;
                if (coul > nb_val_couleurs)
                  coul = nb_val_couleurs;
                glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
                }
              else
                glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
              }
            else
              {
              /*glColor4f (mggris.r, mggris.v, mggris.b, 0.2f);
              l = 0.05 * r_moy;*/
              goto suite;
              }
            }
          
          glLoadIdentity ();
          gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
          glRotatef (phi, 1.0f, 0.0f, 0.0f);
          glRotatef (theta, 0.0f, 0.0f, 1.0f);
          
          glPushMatrix();
          glTranslatef(x[i][state]*adim, -z[i][state]*adim, y[i][state]*adim);
          glRotatef (b * MG_RAD2DEG, -sin (a), -cos (a), 0.0f);
          glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);
          
          for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
            {
            glBegin (GL_POLYGON);
            
            PX = l * sin(t);
            PZ = l * cos(t);
            glVertex3f (PX*adim, -PZ*adim, 0.0f);
            
            PX = l * sin(t+deltaA);
            PZ = l * cos(t+deltaA);
            glVertex3f (PX*adim, -PZ*adim, 0.0f);
            
            PY = dist;
            glVertex3f (PX*adim, -PZ*adim, PY*adim);
            
            PX = l * sin(t);
            PZ = l * cos(t);
            glVertex3f (PX*adim, -PZ*adim, PY*adim);
            
            glEnd ();
            }
          glPopMatrix();
          }
suite:
          /*current_adh++;*/

; /* pour supprimer un warning du compilo */
        
      }
    }
  glEnable (GL_LIGHTING);
  
  if (affgradlinkcolor)
    disp_grad_color ();
  
  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;
  
}




void disp_networks_pos_neg()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  /*int coul;*/
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, a, b;
  double t,PX,PY,PZ,deltaA;
  double xij,yij,zij;
  /*double F_moy_pos;*/
  double Fcut;
  
  
  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;
  
  for (i = 0; i < nbel; i++)
    nbtac += nbneighbors[i][state];
  
  if (dynamic_scale)
    {
    /*valc_rouge = findPositiveMax ((double **) Fn, nbtac, state);*/
    valc_bleu = findNegativeMax ((double **) Fn, nbtac, state);
    }
  
  Fcut= fabs(valc_bleu)/10.; 
  
  
  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
      
      deltaA = M_PI * 0.25;
      for (ii = 0; ii < nbneighbors[i][state]; ii++)
        {
        anta = neighbor[current_adh][state] - 1;
        if  (pres_du_plan (anta, dist_section))
          {
          xij = x[anta][state] - x[i][state];
          yij = y[anta][state] - y[i][state];
          zij = z[anta][state] - z[i][state];
          a = -atan(fabs(zij) / fabs(xij)) + M_PI;
          b = -atan(sqrt(pow(xij,2.0) + pow(zij,2.0))
                    / fabs(yij));
          
          if (yij < 0.0)
            b = -(M_PI - b);
          else
            b=-b;
          
          if ((xij > 0.0) && (zij > 0.0))
            a = -a;
          
          if ((xij < 0.0) && (zij > 0.0))
            a= -(M_PI -a);
          
          if ((xij < 0.0) && (zij < 0.0))
            a = M_PI - a;
          
          dist = sqrt (xij*xij + yij*yij + zij *zij );
          
          if(Fn[current_adh][state]>0.)
            l = r_moy * ((Fn [current_adh][state]) / (/*valc_rouge*/valc_bleu)) * 0.2 ;
          else
            l= r_moy * ((Fn [current_adh][state]) / (valc_bleu)) * 0.2 ;
          
          l = (l > r_moy) ? r_moy : l;
          

            if (Fn[current_adh][state] > 0.)
              {
              glColor3f (1.0f, 0.0f, 0.0f);
              }
            else
              {
              glColor3f (0.0f, 1.0f, 0.0f);
              }
            
            if(fabs(Fn[current_adh][state])<Fcut) goto suite;
          
          glLoadIdentity ();
          gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
          glRotatef (phi, 1.0f, 0.0f, 0.0f);
          glRotatef (theta, 0.0f, 0.0f, 1.0f);
          
          glPushMatrix();
          glTranslatef(x[i][state]*adim, -z[i][state]*adim, y[i][state]*adim);
          glRotatef (b * MG_RAD2DEG, -sin (a), -cos (a), 0.0f);
          glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);
          
          for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
            {
            glBegin (GL_POLYGON);
            
            PX = l * sin(t);
            PZ = l * cos(t);
            glVertex3f (PX*adim, -PZ*adim, 0.0f);
            
            PX = l * sin(t+deltaA);
            PZ = l * cos(t+deltaA);
            glVertex3f (PX*adim, -PZ*adim, 0.0f);
            
            PY = dist;
            glVertex3f (PX*adim, -PZ*adim, PY*adim);
            
            PX = l * sin(t);
            PZ = l * cos(t);
            glVertex3f (PX*adim, -PZ*adim, PY*adim);
            
            glEnd ();
            }
          glPopMatrix();
          }
suite:
          current_adh++;
        
        }
      }
      else
        current_adh += nbneighbors[i][state];
  
  glEnable (GL_LIGHTING);
  
  
  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;
  
}

void disp_dep_relatif ()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, a, b;
  double t,PX,PY,PZ,deltaA;
  double xij,yij,zij;

  double *dp;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  dp = (double *) calloc((size_t) nbbodies * nbcontacts, sizeof(double));

  current_adh=0;
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))   
      for (ii = 0; ii < nbneighbors[i][state]; ii++)
	{
	  anta = neighbor[current_adh][state] - 1;

	  dp[current_adh]=sqrt(
			       pow(x[anta][state] - x_0[anta],2.0) +
			       pow(y[anta][state] - y_0[anta],2.0) +
			       pow(z[anta][state] - z_0[anta],2.0)
			       );

	  current_adh++;
	}
      

  if (dynamic_scale)
    {
     for (i = 0 ; i < nbel ; i++)
       nbtac += nbneighbors[i][state];

     valc_rouge=0.0;
     for (i = 0; i < nbtac ; i++)
       if (dp[i]> valc_rouge) valc_rouge=dp[i];
    }

  glLineWidth (2.0);
  glDisable (GL_LIGHTING);
  current_adh=0;
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	deltaA = M_PI * 0.5;

	  for (ii = 0; ii < nbneighbors[i][state]; ii++)
	    {

	      anta = neighbor[current_adh][state] - 1;

	      if (dp[current_adh] > 0.0)
	      {
		xij = x[anta][state] - x[i][state];
		yij = y[anta][state] - y[i][state];
		zij = z[anta][state] - z[i][state];
		a = -atan(fabs(zij) / fabs(xij)) + M_PI;
		b = -atan(sqrt(pow(xij,2.0) + pow(zij,2.0))
                          /fabs(yij));
	       
		if (yij < 0.0)
		  b = -(M_PI - b);
		else
		  b=-b;

		if ((xij > 0.0) && (zij > 0.0))
		  a = -a;

		if ((xij < 0.0) && (zij > 0.0))
		  a= -(M_PI -a);

		if ((xij < 0.0) && (zij < 0.0))
		  a = M_PI - a;

		dist = sqrt (xij*xij + yij*yij + zij *zij );

		l = r_moy * ((dp[current_adh]) / (valc_rouge)) * 0.25;
                l = (l>r_moy)?r_moy:l;

		if (affgradlinkcolor)
		  {
		    coul = (int) (nb_val_couleurs * (dp[current_adh]) / (valc_rouge)) + 1;
		    if (coul < 0)
		      coul = 0;
		    if (coul > nb_val_couleurs)
		      coul = nb_val_couleurs;
		    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		  }
		else
		  glColor3f (mgbleu.r, mgbleu.v, mgbleu.b);

		glLoadIdentity ();
		gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef (phi, 1.0f, 0.0f, 0.0f);
		glRotatef (theta, 0.0f, 0.0f, 1.0f);

		glPushMatrix();
		glTranslatef((x[i][state]+xij*0.5)*adim,-(z[i][state]+zij*0.5)*adim, (y[i][state]+yij*0.5)*adim);
		glRotatef (b * MG_RAD2DEG,-sin (a),-cos (a), 0.0f);
		glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);
		for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
		  {
		    glBegin (GL_POLYGON);

		    PY=-l;

		    PX = l * sin(t);
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t+deltaA);
		    PZ = l * cos(t+deltaA);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PY = l/* dist */;
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t);
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    glEnd ();
		  }

		glPopMatrix();
	      }
	      current_adh++;

	    }
      }
    else
      current_adh += nbneighbors[i][state];

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

  free(dp);

}


void disp_Vliq_3d ()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double l;
  double xij,yij,zij;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

/*   valc_bleu = 0.0; */
  if (dynamic_scale)
    {
     for (i = 0 ; i < nbel ; i++)
       nbtac += nbneighbors[i][state];
     valc_rouge = findPositiveMax ((double **) Vliq, nbtac, state);

     valc_bleu = valc_rouge;
     for (i = 0; i < nbtac ; i++)
       if ((Vliq[i][state] > 0.0) && (valc_bleu > Vliq[i][state]))
         valc_bleu = Vliq[i][state];

    }

  glLineWidth (2.0);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {

	  for (ii = 0; ii < nbneighbors[i][state]; ii++)
	    {

	      anta = neighbor[current_adh][state] - 1;

	      if ((Vliq[current_adh][state] > 0.0)&&(pres_du_plan (anta, dist_section)))
	      {
		
		xij = x[anta][state] - x[i][state];
		yij = y[anta][state] - y[i][state];
		zij = z[anta][state] - z[i][state];

		l = r_moy * ((Vliq[current_adh][state]) / (valc_rouge)) * 0.25;
                l = (l>r_moy)?r_moy:l;

		if (affgradlinkcolor)
		  {
		    coul = (int) (nb_val_couleurs * (Vliq[current_adh][state]) / (valc_rouge)) + 1;
		    if (coul < 0)
		      coul = 0;
		    if (coul > nb_val_couleurs)
		      coul = nb_val_couleurs;
		    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		  }
		else
		  glColor3f (mgbleu.r, mgbleu.v, mgbleu.b);

		glLoadIdentity ();
		gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef (phi, 1.0f, 0.0f, 0.0f);
		glRotatef (theta, 0.0f, 0.0f, 1.0f);

		glPushMatrix();
		glTranslatef((x[i][state]+xij*0.5)*adim,-(z[i][state]+zij*0.5)*adim, (y[i][state]+yij*0.5)*adim);

		glutSolidSphere (pow(0.238739455674*Vliq[current_adh][state],0.3333333)*adim, 6,4);

	      }
	      current_adh++;

	    }
      }
    else
      current_adh += nbneighbors[i][state];

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}

void disp_negative_normal_strength_2d ()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, ls, lc, sina, cosa;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  valc_bleu = 0.0;
  if (dynamic_scale)
    {
     for (i = 0; i < nbel; i++)
       nbtac += nbneighbors[i][state];
     valc_rouge = findNegativeMax ((double **) Fn, nbtac, state);
    }

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;
	    if (Fn[current_adh][state] < 0.0)
	      {
		dist = sqrt (pow ((x[i][state] - x[anta][state]), 2.0) + pow ((y[i][state] - y[anta][state]), 2.0));
		cosa = (x[anta][state] - x[i][state]) / dist;
		sina = (y[anta][state] - y[i][state]) / dist;
		l = r_moy * ((-Fn[current_adh][state]) / (valc_rouge)) * 0.5;
                l = (l > r_moy*0.5) ? 0.5*r_moy : l;
		lc = l * cosa;
		ls = l * sina;

		if (affgradlinkcolor)
		  {
		    coul = (int) (nb_val_couleurs * (-Fn[current_adh][state]) / (valc_rouge)) + 1;
		    if (coul < 0)
		      coul = 0;
		    if (coul > nb_val_couleurs)
		      coul = nb_val_couleurs;
		    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		  }
		else
		  glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);

		glBegin (GL_POLYGON);
		glVertex3f ((ls + x[i][state]) * adim, 0.0f, (-lc + y[i][state]) * adim);
		glVertex3f ((ls + x[anta][state]) * adim, 0.0f, (-lc + y[anta][state]) * adim);
		glVertex3f ((-ls + x[anta][state]) * adim, 0.0f, (lc + y[anta][state]) * adim);
		glVertex3f ((-ls + x[i][state]) * adim, 0.0f, (lc + y[i][state]) * adim);
		glEnd ();

		if (mgterminal == PS_TERMINAL)
		  mglinkPS(state, l, i, anta);
	      }
	    current_adh++;

	  }
      }
  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;
}

void disp_negative_normal_strength_3d ()
{
  int i, ii, nbtac = 0, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, a, b;
  double t,PX,PY,PZ,deltaA;
  double xij,yij,zij;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  valc_bleu = 0.0;
  if (dynamic_scale)
    {
      for (i = 0; i < nbel; i++)
	nbtac += nbneighbors[i][state];

      valc_rouge = findNegativeMax ((double **) Fn, nbtac, state);
    }

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {

	deltaA = M_PI * 0.25;
	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;
	    if  ((pres_du_plan (anta, dist_section)) && (Fn[current_adh][state] < 0.0))
	      {
		xij = x[anta][state] - x[i][state];
		yij = y[anta][state] - y[i][state];
		zij = z[anta][state] - z[i][state];
		a = -atan(fabs(zij) / fabs(xij)) + M_PI;
		b = -atan(sqrt(pow(xij,2.0) + pow(zij,2.0))
                          / fabs(yij));

		if (yij < 0.0)
		  b = -(M_PI - b);
		else
		  b=-b;

		if ((xij > 0.0) && (zij > 0.0))
		  a = -a;

		if ((xij < 0.0) && (zij > 0.0))
		  a= -(M_PI -a);

		if ((xij < 0.0) && (zij < 0.0))
		  a = M_PI - a;

		dist = sqrt (xij * xij + yij * yij + zij * zij);

		l = r_moy * ((-Fn [current_adh][state]) / (valc_rouge)) * 0.5;
                l = (l > r_moy) ? r_moy : l;
		l=l*myfactor;

		if (affgradlinkcolor)
		  {
		    coul = (int) (nb_val_couleurs * (-Fn[current_adh][state]) / (valc_rouge)) + 1;
		    if (coul < 0)
		      coul = 0;
		    if (coul > nb_val_couleurs)
		      coul = nb_val_couleurs;
		    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		  }
		else
		  glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);

		glLoadIdentity ();
		gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef (phi, 1.0f, 0.0f, 0.0f);
		glRotatef (theta, 0.0f, 0.0f, 1.0f);

		glPushMatrix();
		glTranslatef(x[i][state]*adim,-z[i][state]*adim, y[i][state]*adim);
		glRotatef (b * MG_RAD2DEG,-sin (a),-cos (a), 0.0f);
		glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);
		for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
		  {
		    glBegin (GL_POLYGON);

		    PX = l * sin(t);
		    PY = 0;
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t+deltaA);
		    PY = 0;
		    PZ = l * cos(t+deltaA);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t+deltaA);
		    PY = dist;
		    PZ = l * cos(t+deltaA);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t);
		    PY = dist;
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    glEnd ();
		  }
		glPopMatrix();
	      }
	    current_adh++;

	  }
      }
    else
      current_adh += nbneighbors[i][state];

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}

void disp_tangential_strength_2d ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double l;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  valc_rouge = findMax ((double **) Ft, nbel, state);
  valc_bleu = findMin ((double **) Ft, nbel, state);
  
  if (fabs(valc_bleu) > fabs(valc_rouge)) valc_rouge = fabs(valc_bleu);
  valc_bleu = 0.0;

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;
            /*
	    dist = sqrt (pow ((x[i][state] - x[anta][state]), 2.0) + pow ((y[i][state] - y[anta][state]), 2.0));
	    cosa = (x[anta][state] - x[i][state]) / dist;
	    sina = (y[anta][state] - y[i][state]) / dist;
	    l = r_moy * ((fabs(Ft[current_adh][state]) - valc_bleu) / (valc_rouge - valc_bleu)) / 2.0;
            l = (l > r_moy*0.5) ? 0.5*r_moy : l;
	    lc = l * cosa;
	    ls = l * sina;
*/
	    if (affgradlinkcolor)
	      {
		coul = (int) (nb_val_couleurs * (fabs(Ft[current_adh][state]) - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
		if (coul < 0)
		  coul = 0;
		if (coul > nb_val_couleurs)
		  coul = nb_val_couleurs;

                               
                if (Ft[current_adh][state] > 0.0)
                  glColor3f (gradrouge[coul].r, gradrouge[coul].v, gradrouge[coul].b);
                else
                  glColor3f (gradbleu[coul].r, gradbleu[coul].v, gradbleu[coul].b);
	      }
	    else
	      glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);
            
            l = r_moy * ((fabs(Ft[current_adh][state]) - valc_bleu) / (valc_rouge - valc_bleu)) / 2.0;
            l = (l > r_moy*0.5) ? 0.5*r_moy : l;
            pastille2d(i,anta,l);
/*
	    glBegin (GL_POLYGON);
	    glVertex3f ((ls + x[i][state]) * adim, 0.0f, (-lc + y[i][state]) * adim);
	    glVertex3f ((ls + x[anta][state]) * adim, 0.0f, (-lc + y[anta][state]) * adim);
	    glVertex3f ((-ls + x[anta][state]) * adim, 0.0f, (lc + y[anta][state]) * adim);
	    glVertex3f ((-ls + x[i][state]) * adim, 0.0f, (lc + y[i][state]) * adim);
	    glEnd ();
*/
		if (mgterminal == PS_TERMINAL)
		  mglinkPS(state, l, i, anta);

	    current_adh++;

	  }
      }
  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}

void disp_tangential_strength_3d ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  double dist;
  double l, a, b;
  double t, PX, PY, PZ, deltaA;
  double xij, yij, zij;

  if (dynamic_scale)
    {
      valc_rouge = findMax ((double **) Ft, nbel, state);
/*       valc_bleu = findMin ((double **) Ft, nbel, state); */
    }
  valc_bleu = 0.0;

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	deltaA = M_PI * 0.25;
	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;

	    if  (pres_du_plan (anta, dist_section)) 
	      {
		xij = x[anta][state] - x[i][state];
		yij = y[anta][state] - y[i][state];
		zij = z[anta][state] - z[i][state];
		a = -atan(fabs(zij) / fabs(xij)) + M_PI;
		b = -atan(sqrt(pow(xij, 2.0) + pow(zij, 2.0))
                          / fabs(yij));
	       
		if (yij < 0.0)
		  b = -(M_PI - b);
		else
		  b=-b;

		if ((xij > 0.0) && (zij > 0.0))
		  a = -a;

		if ((xij < 0.0) && (zij > 0.0))
		  a= -(M_PI -a);

		if ((xij < 0.0) && (zij < 0.0))
		  a = M_PI - a;

		dist = sqrt (xij * xij + yij * yij + zij * zij);

		l = r_moy * (fabs(Ft [current_adh][state]) / (valc_rouge)) * 0.5;
                l = (l > r_moy) ? r_moy : l;

		if (affgradlinkcolor)
		  {
		    coul = (int) (nb_val_couleurs * (Ft[current_adh][state] - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
		    if (coul < 0)
		      coul = 0;
		    if (coul > nb_val_couleurs)
		      coul = nb_val_couleurs;
		    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		  }
		else
		  glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);


		glLoadIdentity ();
		gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef (phi, 1.0f, 0.0f, 0.0f);
		glRotatef (theta, 0.0f, 0.0f, 1.0f);

		glPushMatrix();
		glTranslatef(x[i][state]*adim,-z[i][state]*adim, y[i][state]*adim);
		glRotatef (b * MG_RAD2DEG,-sin (a),-cos (a), 0.0f);
		glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);
		for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
		  {
		    glBegin (GL_POLYGON);

		    PX = l * sin(t);
		    PY = 0;
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t+deltaA);
		    PY = 0;
		    PZ = l * cos(t+deltaA);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t+deltaA);
		    PY = dist;
		    PZ = l * cos(t+deltaA);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t);
		    PY = dist;
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    glEnd ();
		  }
		glPopMatrix();

	      }
	    current_adh++;

	  }
      }
    else
      current_adh += nbneighbors[i][state];

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

}


void disp_strength_3d ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  double dist;
  double l, a, b;
  double t, PX, PY, PZ, deltaA;
  double xij, yij, zij;
  double norm_F;

  double unX,unY,unZ;
  double utX,utY,utZ;
  double usX,usY,usZ;
  double inv_norm;
  double FX,FY,FZ;

  if (dynamic_scale) /* en les effort normaux sont les plus forts */
    {
      valc_rouge = findMax ((double **) Fn, nbel, state);
      valc_bleu = findMin ((double **) Fn, nbel, state);
    }

  unX = unY = unZ = utX = utY = utZ = usX = usY = usZ = 0.0;
 
  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	deltaA = M_PI * 0.25;
	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;

	    if  (pres_du_plan (anta, dist_section)) 
	      {
		xij = x[anta][state] - x[i][state];
		yij = y[anta][state] - y[i][state];
		zij = z[anta][state] - z[i][state];

		dist = sqrt (xij * xij + yij * yij + zij * zij);
		inv_norm = 1.0 / dist;
		unX = -xij * inv_norm;
		unY = -yij * inv_norm;
		unZ = -zij * inv_norm;

		if ((unX != 0.0) || (unZ != 0.0))
		  {
		    /* s=Y^n (donc usY=0) */
		    usX = unZ;
		    usZ = -unX;

		    inv_norm = 1 / sqrt (usX * usX + usZ * usZ);

		    usX = usX * inv_norm;
		    usZ = usZ * inv_norm;

		    /* t = s ^ n */
		    utX = -usZ * unY;
		    utY = unX * usZ - usX * unZ;
		    utZ = usX * unY;
		  }
		else
		  {
		    if (unY == 1.0)
		      {
			usX = 0.0;
			usZ = 1.0;
			utX = -1.0;
			utY = 0.0;
			utZ = 0.0;
		      }

		    if (unY == -1.0)
		      {
			usX = 0.0;
			usZ = 1.0;
			utX = 1.0;
			utY = 0.0;
			utZ = 0.0;
		      }
		  }

		FX = Fn[current_adh][state] * unX + Ft[current_adh][state] * utX + Fs[current_adh][state] * usX;
		FY = Fn[current_adh][state] * unY + Ft[current_adh][state] * utY + Fs[current_adh][state] * usY;
		FZ = Fn[current_adh][state] * unZ + Ft[current_adh][state] * utZ + Fs[current_adh][state] * usZ;

		a = -atan(fabs(FZ) / fabs(FX)) + M_PI;
		b = -atan(sqrt(pow(FX,2.0) + pow(FZ,2.0))
                          / fabs(FY));

		/* c'est super pas beau mais ça fonctionne : */
	       
		if (FY < 0.0)
		  b = -(M_PI - b);
		else
		  b=-b;

		if ((FX > 0.0) && (FZ > 0.0))
		  a = -a;

		if ((FX < 0.0) && (FZ > 0.0))
		  a= -(M_PI -a);

		if ((FX < 0.0) && (FZ < 0.0))
		  a = M_PI - a;

		norm_F = sqrt(  Fn [current_adh][state] * Fn [current_adh][state]
			      + Ft [current_adh][state] * Ft [current_adh][state]
			      + Fs [current_adh][state] * Fs [current_adh][state] );

		l = r_moy * (norm_F / valc_rouge) * 0.5;
		l = (l > r_moy) ? r_moy : l;

		if (affgradlinkcolor)
		  {
		    coul = (int) (nb_val_couleurs * (norm_F - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
		    if (coul < 0)
		      coul = 0;
		    if (coul > nb_val_couleurs)
		      coul = nb_val_couleurs;
		    glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
		  }
		else
		  glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);


		glLoadIdentity ();
		gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
		glRotatef (phi, 1.0f, 0.0f, 0.0f);
		glRotatef (theta, 0.0f, 0.0f, 1.0f);

		glPushMatrix();
		glTranslatef(( x[i][state] + 0.5 * xij) * adim,
                             (-z[i][state] - 0.5 * zij) * adim,
                             ( y[i][state] + 0.5 * yij) * adim);

		glRotatef (b * MG_RAD2DEG,-sin (a),-cos (a), 0.0f);
		glRotatef (a * MG_RAD2DEG, 0.0f, 0.0f, 1.0f);

		for (t = 0; t < (M_PI * 2.0); t = t + deltaA)
		  {
		    glBegin (GL_POLYGON);

		    PX = l * sin(t);
		    PY = -dist * 0.5;
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t+deltaA);
/* 		    PY = 0; */
		    PZ = l * cos(t+deltaA);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

/* 		    PX = l * sin(t+deltaA); */
		    PY = dist * 0.5;
/* 		    PZ = l * cos(t+deltaA); */
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    PX = l * sin(t);
/* 		    PY = dist; */
		    PZ = l * cos(t);
		    glVertex3f (PX*adim, -PZ*adim, PY*adim);

		    glEnd ();
		  }
		glPopMatrix();

	      }
	    current_adh++;

	  }
      }
    else
      current_adh += nbneighbors[i][state];

  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

}

void disp_torque_2d ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double l;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  valc_rouge = findMax ((double **) Crot, nbel, state);
  valc_bleu = findMin ((double **) Crot, nbel, state);
  
  if (fabs(valc_bleu) > fabs(valc_rouge)) valc_rouge = fabs(valc_bleu);
  valc_bleu = 0.0;

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;

	    if (affgradlinkcolor)
	      {
		coul = (int) (nb_val_couleurs * (fabs(Crot[current_adh][state]) - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
		if (coul < 0)
		  coul = 0;
		if (coul > nb_val_couleurs)
		  coul = nb_val_couleurs;
                
                if (Crot[current_adh][state] > 0.0)
		  glColor3f (gradrouge[coul].r, gradrouge[coul].v, gradrouge[coul].b);
                else
                  glColor3f (gradbleu[coul].r, gradbleu[coul].v, gradbleu[coul].b);
	      }
	    else
	      glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);

            l = r_moy * ((fabs(Crot[current_adh][state]) - valc_bleu) / (valc_rouge - valc_bleu)) / 2.0;
            l = (l > r_moy*0.5) ? 0.5*r_moy : l;
            pastille2d(i,anta,l);


		if (mgterminal == PS_TERMINAL)
		  mglinkPS(state, l, i, anta);

	    current_adh++;

	  }
      }
  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}


void disp_shear_vlocy_2d ()
{
  int i, ii, current_adh = 0, anta;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  float save_valc_bleu, save_valc_rouge;
  double dist;
  double l, ls, lc, sina, cosa;

  save_valc_bleu = valc_bleu;
  save_valc_rouge = valc_rouge;

  if (dynamic_scale)
    {
      valc_rouge = findMax ((double **) Vt, nbel, state);
      valc_bleu = findMin ((double **) Vt, nbel, state);
    }

  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);

	for (ii = 0; ii < nbneighbors[i][state]; ii++)
	  {
	    anta = neighbor[current_adh][state] - 1;

	    if (affgradlinkcolor)
	      {
		coul = (int) (nb_val_couleurs * (Vt[current_adh][state] - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
		if (coul < 0)
		  coul = 0;
		if (coul > nb_val_couleurs)
		  coul = nb_val_couleurs;
		glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
	      }
	    else
	      glColor3f (fg_color2.r, fg_color2.v, fg_color2.b);

	    dist = sqrt (pow ((x[i][state] - x[anta][state]), 2.0) + pow ((y[i][state] - y[anta][state]), 2.0));
	    cosa = (x[anta][state] - x[i][state]) / dist;
	    sina = (y[anta][state] - y[i][state]) / dist;
	    l = r_moy * (fabs (Vt[current_adh][state]) / (valc_rouge - valc_bleu)) * 0.5;
	    lc = l * cosa;
	    ls = l * sina;

	    glBegin (GL_POLYGON);
	    glVertex3f ((ls + x[i][state]) * adim, 0.0f, (-lc + y[i][state]) * adim);
	    glVertex3f ((ls + x[anta][state]) * adim, 0.0f, (-lc + y[anta][state]) * adim);
	    glVertex3f ((-ls + x[anta][state]) * adim, 0.0f, (lc + y[anta][state]) * adim);
	    glVertex3f ((-ls + x[i][state]) * adim, 0.0f, (lc + y[i][state]) * adim);
	    glEnd ();
	    current_adh++;

	  }
      }
  glEnable (GL_LIGHTING);

  if (affgradlinkcolor)
    disp_grad_color ();

  valc_bleu = save_valc_bleu;
  valc_rouge = save_valc_rouge;

}


void disp_angular_vlocy_2d ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;

  if (dynamic_scale)
    {
      valc_rouge = findMax ((double **) vrot, nbel, state);
      valc_bleu = findMin ((double **) vrot, nbel, state);
    }

  glPushMatrix ();
  glLineWidth (1.0);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	coul = (int) (nb_val_couleurs * (vrot[i][state] - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
	if (coul < 0)
	  coul = 0;
	if (coul > nb_val_couleurs)
	  coul = nb_val_couleurs;

	glColor4f (gradc[coul].r, gradc[coul].v, gradc[coul].b, alpha_color);
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);
	glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);

	mgbodyselect_plein (i, state);
      }
  glPopMatrix ();

  if (affgradcolor)
    disp_grad_color ();

}

void disp_vlocy_magnitude ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  double v_abs;
  double vcmax, vcmin;

  if (dynamic_scale)
    {
      valc_rouge = findAbsMax ((double **) vx, nbel, state);
      vcmax = findAbsMax ((double **) vy, nbel, state);
      if (vcmax > valc_rouge)
	valc_rouge = vcmax;
      vcmax = findAbsMax ((double **) vz, nbel, state);
      if (vcmax > valc_rouge)
	valc_rouge = vcmax;

      valc_bleu = findAbsMin ((double **) vx, nbel, state);
      vcmin = findAbsMin ((double **) vy, nbel, state);
      if (vcmin < valc_bleu)
	valc_bleu = vcmin;
      vcmin = findAbsMin ((double **) vz, nbel, state);
      if (vcmin < valc_bleu)
	valc_bleu = vcmin;
    }

  glPushMatrix ();
  glLineWidth (1.0);
  if (mode2D)
    glDisable (GL_LIGHTING);
  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
      v_abs = sqrt (pow (vx[i][state], 2) + pow (vy[i][state], 2) + pow (vz[i][state], 2));
      
      coul = (int) (nb_val_couleurs * (v_abs - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
      if (coul < 0)
        coul = 0;
      if (coul > nb_val_couleurs)
        coul = nb_val_couleurs;
      
      glColor4f (gradc[coul].r, gradc[coul].v, gradc[coul].b, alpha_color);
      glLoadIdentity ();
      gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
      glRotatef (phi, 1.0f, 0.0f, 0.0f);
      glRotatef (theta, 0.0f, 0.0f, 1.0f);
      glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
      
      mgbodyselect_plein (i, state);
      if (mgterminal == PS_TERMINAL)
        mgbodyselectPS_plein (i, state, coul);
      }
  glEnable (GL_LIGHTING);
  glPopMatrix ();

  if ((affgradcolor)&&(renderMode == GL_RENDER))
    disp_grad_color ();

}


void disp_dist_ref ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  double dmin, dmax, stor, dep;

  if ((multifiles) && (dynamic_scale))
    {
      dmax = 0.0;
      dmin = 0.0;
      for (i = 0; i < nbel; i++)
	{
	  stor = sqrt (pow (x[i][state] - x_0[i], 2.0) 
                     + pow (y[i][state] - y_0[i], 2.0)
                     + pow (z[i][state] - z_0[i], 2.0) );
	  if (stor > dmax)
	    dmax = stor;
	  if (stor < dmin)
	    dmin = stor;
	}
      valc_rouge = dmax;
    }

  if ((!multifiles) && (dynamic_scale))
    {
      dmax = 0.0;
      dmin = 0.0;
      for (i = 0; i < nbel; i++)
	{
	  stor = sqrt (pow (x[i][state] - x[i][0], 2.0) 
                     + pow (y[i][state] - y[i][0], 2.0)
                     + pow (z[i][state] - z[i][0], 2.0) );
	  if (stor > dmax)
	    dmax = stor;
	}
      valc_rouge = dmax;
    }
  valc_bleu = 0.0;

  glPushMatrix ();
  glLineWidth (1.0);

  if (mode2D)
    glDisable (GL_LIGHTING);

  for (i = 0; i < nbel; i++)
    if (pres_du_plan (i, dist_section))
      {
	if (multifiles)
	  dep = sqrt (pow (x[i][state] - x_0[i], 2.0) 
                    + pow (y[i][state] - y_0[i], 2.0)
                    + pow (z[i][state] - z_0[i], 2.0) );
	else
	  dep = sqrt (pow (x[i][state] - x[i][0], 2.0) 
                    + pow (y[i][state] - y[i][0], 2.0)
                    + pow (z[i][state] - z[i][0], 2.0) );

	coul = (int) (nb_val_couleurs * (dep - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
	if (coul < 0)
	  coul = 0;
	if (coul > nb_val_couleurs)
	  coul = nb_val_couleurs;

	glColor4f (gradc[coul].r, gradc[coul].v, gradc[coul].b, alpha_color);
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);
	glTranslatef (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);

	mgbodyselect_plein (i, state);
	if (mgterminal == PS_TERMINAL)
	  mgbodyselectPS_plein (i, state, coul);
      }
  glEnable (GL_LIGHTING);
  glPopMatrix ();

  if (affgradcolor)
    disp_grad_color ();

}

void disp_vlocy_field ()
{
  int i;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int coul;
  double v_abs;
  double vcmax, vcmin;
  
  if ((dynamic_scale) && (affgradcolor)) 
    {
    valc_rouge = findAbsMax ((double **) vx, nbel, state);
    vcmax = findAbsMax ((double **) vy, nbel, state);
    if (vcmax > valc_rouge)
      valc_rouge = vcmax;
    vcmax = findAbsMax ((double **) vz, nbel, state);
    if (vcmax > valc_rouge)
      valc_rouge = vcmax;
    
    valc_bleu = findAbsMin ((double **) vx, nbel, state);
    vcmin = findAbsMin ((double **) vy, nbel, state);
    if (vcmin < valc_bleu)
      valc_bleu = vcmin;
    vcmin = findAbsMin ((double **) vz, nbel, state);
    if (vcmin < valc_bleu)
      valc_bleu = vcmin;
    }
  
  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);
  
  glLineWidth (2.0f);
  glDisable (GL_LIGHTING);
  
  glBegin (GL_LINES);
  for (i = 0; i < nbel; i++)
    {
    if (affgradcolor)
      {
      v_abs = sqrt (pow (vx[i][state], 2.0) + pow (vy[i][state], 2.0) + pow (vz[i][state], 2.0));
      
      coul = (int) (nb_val_couleurs * (v_abs - valc_bleu) / (valc_rouge - valc_bleu)) + 1;
      if (coul < 0)
        coul = 0;
      if (coul > nb_val_couleurs)
        coul = nb_val_couleurs;
      
      glColor4f (gradc[coul].r, gradc[coul].v, gradc[coul].b, alpha_color);
      }
    else
      glColor3f (fg_color.r, fg_color.v, fg_color.b);
    
    
    if (pres_du_plan (i, dist_section))
      {    
      glVertex3f (x[i][state] * adim, -z[i][state] * adim, y[i][state] * adim);
      glVertex3f (x[i][state] * adim + (vx[i][state] - vxrep) * v_adi,
                  -(z[i][state] * adim + (vz[i][state] - vzrep) * v_adi),
                  y[i][state] * adim + (vy[i][state] - vyrep) * v_adi);
      }
    
    }
  glEnd ();
  
  glEnable (GL_LIGHTING);
  
  if ((affgradcolor) && (renderMode == GL_RENDER))
    disp_grad_color ();
}

/* ******************************************************************* */
/* ******************************************************************* */
/* ----- BODIES SHAPES: */

void mgdisk (double r)
{
  int i, pas = 18;

  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  glBegin (GL_LINE_LOOP);
  for (i = 0; i < 360; i += pas)
    glVertex3f (r * cos (i * MG_DEG2RAD), 0.0f, r * sin (i * MG_DEG2RAD));
  glEnd ();

  glEnable (GL_LIGHTING);
}

void mgdisk_plein (double r)
{
  int i, pas = 18;

  glDisable (GL_LIGHTING);

  if (affgradcolor)
    {
      glBegin (GL_POLYGON);
      for (i = 0; i < 360; i += pas)
	glVertex3f (r * cos (i * MG_DEG2RAD), 0.0f, r * sin (i * MG_DEG2RAD));
      glEnd ();
    }

  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  glBegin (GL_LINE_LOOP);
  for (i = 0; i < 360; i += pas)
    glVertex3f (r * cos (i * MG_DEG2RAD), 0.0f, r * sin (i * MG_DEG2RAD));
  glEnd ();

  glEnable (GL_LIGHTING);
}

void mgjonc (double ax1, double ax2)
{
  int i, pas = 18;

  glDisable (GL_LIGHTING);

  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glBegin (GL_LINE_LOOP);

  for (i = -90; i <= 90; i += pas)
    glVertex3f (ax2 * cos (i * MG_DEG2RAD) + ax1, 0.0f, ax2 * sin (i * MG_DEG2RAD));

  glVertex3f (-ax1, 0.0f, ax2);

  for (i = 90; i <= 270; i += pas)
    glVertex3f (ax2 * cos (i * MG_DEG2RAD) - ax1, 0.0f, ax2 * sin (i * MG_DEG2RAD));

  glVertex3f (ax1, 0.0f, -ax2);

  glEnd ();

  glEnable (GL_LIGHTING);
}

void mgjonc_plein (double ax1, double ax2)
{
  int i, pas = 18;

  if (affgradcolor)
    {
      glDisable (GL_LIGHTING);

      glBegin (GL_POLYGON);

      for (i = -90; i <= 90; i += pas)
	glVertex3f (ax2 * cos (i * MG_DEG2RAD) + ax1, 0.0f, ax2 * sin (i * MG_DEG2RAD));

      glVertex3f (-ax1, 0.0f, ax2);

      for (i = 90; i <= 270; i += pas)
	glVertex3f (ax2 * cos (i * MG_DEG2RAD) - ax1, 0.0f, ax2 * sin (i * MG_DEG2RAD));

      glVertex3f (ax1, 0.0f, -ax2);

      glEnd ();

      glEnable (GL_LIGHTING);
    }

  mgjonc (ax1, ax2);

}

void mgSegmt (double r)
{

  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  
  glBegin (GL_LINES);
  glVertex3f (0.0f, 0.0f, 0.0f);
  glVertex3f (r, 0.0f, 0.0f);
  glEnd ();

  glEnable (GL_LIGHTING);
}


void mgmurX (double pos)
{
  double my, mz, My, Mz;
  double my2, mz2, My2, Mz2;
  double ly, lz;

  my = (yminB+yvec) * adim;
  mz = (zminB+zvec) * adim;
  My = (ymaxB+yvec) * adim;
  Mz = (zmaxB+zvec) * adim;

  ly = (My - my) * 0.1;
  lz = (Mz - mz) * 0.1;

  my2 = my - ly;
  mz2 = mz - lz;
  My2 = My + ly;
  Mz2 = Mz + lz;

  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glBegin (GL_LINES);
  glVertex3f (pos, -mz2, my);
  glVertex3f (pos, -Mz2, my);
  glVertex3f (pos, -mz2, My);
  glVertex3f (pos, -Mz2, My);

  glVertex3f (pos, -mz, my2);
  glVertex3f (pos, -mz, My2);
  glVertex3f (pos, -Mz, my2);
  glVertex3f (pos, -Mz, My2);
  glEnd ();
  glBegin (GL_LINE_LOOP);
  glVertex3f (pos, -mz2, my2);
  glVertex3f (pos, -mz2, My2);
  glVertex3f (pos, -Mz2, My2);
  glVertex3f (pos, -Mz2, my2);
  glEnd ();
  glEnable (GL_LIGHTING);
}

void mgmurY (double pos)
{
  double mx, mz, Mx, Mz;
  double mx2, mz2, Mx2, Mz2;
  double lx, lz;

  mx = (xminB+xvec) * adim;
  mz = (zminB+zvec) * adim;
  Mx = (xmaxB+xvec) * adim;
  Mz = (zmaxB+zvec) * adim;

  lx = (Mx - mx) * 0.1;
  lz = (Mz - mz) * 0.1;

  mx2 = mx - lx;
  mz2 = mz - lz;
  Mx2 = Mx + lx;
  Mz2 = Mz + lz;

  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glBegin (GL_LINES);
  glVertex3f (mx, -mz2, pos);
  glVertex3f (mx, -Mz2, pos);
  glVertex3f (Mx, -mz2, pos);
  glVertex3f (Mx, -Mz2, pos);

  glVertex3f (mx2, -mz, pos);
  glVertex3f (Mx2, -mz, pos);
  glVertex3f (mx2, -Mz, pos);
  glVertex3f (Mx2, -Mz, pos);
  glEnd ();
  glBegin (GL_LINE_LOOP);
  glVertex3f (mx2, -mz2, pos);
  glVertex3f (Mx2, -mz2, pos);
  glVertex3f (Mx2, -Mz2, pos);
  glVertex3f (mx2, -Mz2, pos);
  glEnd ();
  glEnable (GL_LIGHTING);
}

void mgmbr2D (int datapos)
{
  int nbi;
  int i, ideb, iend;

  nbi = (int)(datas[datapos]);
  ideb = datapos + 1;
  iend = ideb + 2 * (nbi + 1) - 1; 

  glColor3f (1.0, 0.0, 0.0);
  glLineWidth (1.0);

  glBegin (GL_LINE_STRIP);  
  for(i = ideb ; i < iend ; i = i + 2)
    {
      glVertex3f ( datas[i]*adim, 0.0,datas[i+1]*adim);
    }
  glEnd ();

  glColor3f (0.0, 1.0, 0.0);
  glPointSize(4.0);
  glBegin (GL_POINTS);  
  for(i = ideb ; i < iend ; i = i + 2)
    {
      glVertex3f ( datas[i]*adim, 0.0,datas[i+1]*adim);
    }
  glEnd ();

  glColor3f (0.0, 0.0, 0.0);
}

void mgPicket (int datapos)
{
  int i;
  double p, r, angle, ltot;
  double lpointe;

  p = datas[datapos] * adim;
  r = datas[datapos+1] * adim;
  angle = datas[datapos+2];
  ltot = datas[datapos+3] * adim;

  lpointe = fabs(r / tan(angle));

  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  glBegin (GL_LINE_LOOP);
  for (i = 0; i <= 360; i += 15)
    glVertex3f (r * cos (i * MG_DEG2RAD), r * sin (i * MG_DEG2RAD), p + lpointe);
  glEnd();

  glBegin (GL_LINE_LOOP);
  for (i = 0; i <= 360; i += 15)
    glVertex3f (r * cos (i * MG_DEG2RAD), r * sin (i * MG_DEG2RAD), p + ltot);
  glEnd();

  glBegin (GL_LINES);
  for (i = 0; i <= 360; i += 15)
    {
      glVertex3f (0.0f, 0.0f, p);
      glVertex3f (r * cos (i * MG_DEG2RAD), r * sin (i * MG_DEG2RAD), p + lpointe);
      glVertex3f (r * cos (i * MG_DEG2RAD), r * sin (i * MG_DEG2RAD), p + lpointe);
      glVertex3f (r * cos (i * MG_DEG2RAD), r * sin (i * MG_DEG2RAD), p + ltot);
    }
  glEnd();

  glEnable (GL_LIGHTING);

}

void mgCylY(int datapos)
{
  int i;
  double r, base, l, pos;

  r= datas[datapos] * adim;
  base = datas[datapos+1] * adim;
  l = datas[datapos+2] * adim;
  pos = datas[datapos+3] * adim;

  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  glBegin (GL_LINE_STRIP/* LOOP */);
  for (i = 0; i <= 180/* 360 */; i += 15)
    glVertex3f (r * cos (i * MG_DEG2RAD) + pos, r * sin (i * MG_DEG2RAD), base);
  glEnd();

  glBegin (GL_LINE_STRIP/* LOOP */);
  for (i = 0; i <= 180/* 360 */; i += 15)
    glVertex3f (r * cos (i * MG_DEG2RAD) + pos, r * sin (i * MG_DEG2RAD), base + 0.3333 * l);
  glEnd();

  glBegin (GL_LINE_STRIP/* LOOP */);
  for (i = 0; i <= 180/* 360 */; i += 15)
    glVertex3f (r * cos (i * MG_DEG2RAD) + pos, r * sin (i * MG_DEG2RAD), base + 0.6666 * l);
  glEnd();

  glBegin (GL_LINE_STRIP/* LOOP */);
  for (i = 0; i <= 180/* 360 */; i += 15)
    glVertex3f (r * cos (i * MG_DEG2RAD) + pos, r * sin (i * MG_DEG2RAD), base + l);
  glEnd();

  glBegin (GL_LINES);
  for (i = 0; i <= 180/* 360 */; i += 15)
    {
      glVertex3f (r * cos (i * MG_DEG2RAD) + pos, r * sin (i * MG_DEG2RAD), base);
      glVertex3f (r * cos (i * MG_DEG2RAD) + pos, r * sin (i * MG_DEG2RAD), base + l);
    }
  glEnd();

  glEnable (GL_LIGHTING);
}

void mgFluidCell (int num)
{
  if (!with_fluid) return;
  
  glDisable (GL_LIGHTING);
  
  glBegin (GL_POLYGON);
  glVertex3f (0.0f, 0.0f, 0.0f);
  glVertex3f (FluidCell[num].delx*adim, 0.0f, 0.0f);
  glVertex3f (FluidCell[num].delx*adim, 0.0f, FluidCell[num].dely*adim);
  glVertex3f (0.0f, 0.0f, FluidCell[num].dely*adim);
  glEnd ();
  
  glEnable (GL_LIGHTING);
}

void mgpolyg (int bdy)
{
  int i,nbi, ideb,iend;
  
  datapos = datadistrib[bdy];
  nbi =  dataqty[bdy];
  
  ideb = datapos;
  iend = ideb + nbi - 1;
  
  /*  glDisable (GL_LIGHTING); */
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  
  glBegin  (GL_LINE_LOOP);
  for (i = ideb ; i < iend ; i = i + 2)
    {
    glVertex3f (datas[i]*adim, 0.0f, datas[i+1]*adim);
    }
  
  glEnd ();
  /*   glEnable (GL_LIGHTING); */
}

void mgpolyg_plein (int bdy)
{
  int i, nbi, ideb, iend;
  
  datapos = datadistrib[bdy];
  nbi =  dataqty[bdy];
  
  ideb = datapos;
  iend = ideb + nbi - 1;
  
  glDisable (GL_LIGHTING);
  if (affgradcolor)
    {
    glBegin (GL_POLYGON);
    for (i = ideb ; i < iend ; i = i + 2)
      {
      glVertex3f (datas[i]*adim, 0.0f, datas[i+1]*adim);
      }
    glEnd ();
    }
  mgpolyg (bdy);
  glEnable (GL_LIGHTING);
}

void mgpolye_plein (int bdy)
{
  int i,nbi, nbf, ideb, fdeb, fend;
  int n1, n2, n3;
  double n1x, n1y, n1z;
  double n2x, n2y, n2z;
  double n3x, n3y, n3z;
  double normx, normy, normz, normalize;
  
  datapos = datadistrib[bdy];
  nbi = datas[datapos + dataqty[bdy] - 2];
  nbf = datas[datapos + dataqty[bdy] - 1];
  
  ideb = datapos;
  fdeb = ideb + nbi * 3;
  fend = fdeb + nbf * 3 - 1;
  
  if (shade_polye)
    glEnable (GL_LIGHTING);
  else
    glDisable (GL_LIGHTING);
  
  glEnable(GL_POLYGON_OFFSET_FILL);
  
  glPolygonOffset(1.0f, 1.0f);
  
  for (i = fdeb ; i < fend ; i = i + 3)
    {
    n1 = (int)datas[i];
    n2 = (int)datas[i + 1];
    n3 = (int)datas[i + 2];
    
    n1x = datas[ideb + n1 * 3];
    n1y = datas[ideb + n1 * 3 + 1];
    n1z = datas[ideb + n1 * 3 + 2];
    
    n2x = datas[ideb + n2 * 3];
    n2y = datas[ideb + n2 * 3 + 1];
    n2z = datas[ideb + n2 * 3 + 2];
    
    n3x = datas[ideb + n3 * 3];
    n3y = datas[ideb + n3 * 3 + 1];
    n3z = datas[ideb + n3 * 3 + 2];
    
    glBegin (GL_POLYGON);
    
    if (shade_polye)
      {
      normx = (n2x - n1x) * (n3y - n1y) - (n2y - n1y) * (n3x - n1x);
      normy = (n2z - n1z) * (n3x - n1x) - (n2x - n1x) * (n3z - n1z);
      normz = (n2y - n1y) * (n3z - n1z) - (n2z - n1z) * (n3y - n1y);
      normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);
      
      glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
      }
    glVertex3f (n1x * adim, -n1z * adim, n1y * adim);
    glVertex3f (n2x * adim, -n2z * adim, n2y * adim);
    glVertex3f (n3x * adim, -n3z * adim, n3y * adim);
    
    glEnd (); 
    }
  
  glDisable (GL_LIGHTING);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glDisable(GL_POLYGON_OFFSET_FILL);
  
  if(!shade_polye)
    {
    for (i = fdeb ; i < fend ; i = i + 3)
      {
      n1 = (int)datas[i];
      n2 = (int)datas[i + 1];
      n3 = (int)datas[i + 2];
      
      glBegin (GL_LINE_LOOP);
      glVertex3f (datas[ideb + n1 * 3] * adim,
                  -datas[ideb + n1 * 3 + 2] * adim,
                  datas[ideb + n1 * 3 + 1] * adim);
      glVertex3f (datas[ideb + n2 * 3] * adim,
                  -datas[ideb + n2 * 3 + 2] * adim, 
                  datas[ideb + n2 * 3 + 1] * adim);
      glVertex3f (datas[ideb + n3 * 3] * adim,
                  -datas[ideb + n3 * 3 + 2] * adim, 
                  datas[ideb + n3 * 3 + 1] * adim);
      glEnd ();
      }
    }
  
  glEnable (GL_LIGHTING);
}

void mgsphere (double r) /* FIXME: finalement c'est peut-etre pas necessaire d'ecrire une fonction pour ca !*/
{
/*   if (mgterminal != GL2PS_TERMINAL) */
/*     { */
      glutSolidSphere (r, nb_subdiv_sphere, nb_subdiv_sphere_2);
/*     } */
/*   else */
/*     { */
/*   int i, pas = 36; */

/*       glPopMatrix(); */

/*       /\* FIXME: la perspective pose probleme, il faudrait une projection orthogonale *\/ */
/*       glRotatef (theta, 0.0, 0.0, -1.0); */
/*       glRotatef (0.5*M_PI-phi, -1.0, 0.0, 0.0); */

/*       glDisable (GL_LIGHTING); */

/*       glBegin (GL_POLYGON); */
/*       for (i = 0; i < 360; i += pas) */
/* 	glVertex3f (r * cos (i * MG_DEG2RAD), 0.0, r * sin (i * MG_DEG2RAD)); */
/*       glEnd (); */

/*       glColor3f (fg_color.r, fg_color.v, fg_color.b); */

/*       glBegin (GL_LINE_LOOP); */
/*       for (i = 0; i < 360; i += pas) */
/* 	glVertex3f (r * cos (i * MG_DEG2RAD), 0.0, r * sin (i * MG_DEG2RAD)); */
/*       glEnd (); */

/*       glEnable (GL_LIGHTING); */

/*       glPushMatrix(); */
/*     } */
}


void mgobstselect (int i, int t)
{
  glPushMatrix ();

  glRotatef (rot[i][t] * MG_RAD2DEG, 0.0f, -1.0f, 0.0f);

  switch (bdyty[i])
    {
    case MGP_SEGMT:

      mgSegmt(radius[i][t] * adim);
      break;

    case MGP_JONCx:

      mgjonc (datas[datadistrib[i]] * adim, datas[datadistrib[i] + 1] * adim);
      if (orient)
	mgorient ((datas[datadistrib[i]] + datas[datadistrib[i] + 1]) * adim);
      break;

    case MGP_PICKT:

      mgPicket (datadistrib[i]);
      break;

    case MGP_MURX0:
    case MGP_MURX1:

      mgmurX (datas[datadistrib[i]] * adim);
      break;

    case MGP_MURY0:
    case MGP_MURY1:

      mgmurY (datas[datadistrib[i]] * adim);
      break;

    case MGP_MBR2D:

      mgmbr2D (datadistrib[i]);
      break;

    case MGP_CYLYx:

      mgCylY (datadistrib[i]);
      break;
    }
  glPopMatrix ();
}

void mgbodyselect (int i, int t)
{
  glPushMatrix ();
  
  glRotatef (rot[i][t] * MG_RAD2DEG, 0.0f, -1.0f, 0.0f); // 2D seulement
  
  if (renderMode == GL_SELECT) 
    glLoadName(i);
  
  if ((i == selectedBody) && (renderMode == GL_RENDER))
    glLineWidth(4.0f);
  
  switch (bdyty[i])
    {
    
    case MGP_DISKx:
      
      mgdisk (radius[i][t] * adim);
      if (orient)
        mgorient (radius[i][t] * adim);
        break;
      
    case MGP_SPHER:
      
      if (orient)
        {
        glColor3f (fg_color.r, fg_color.v, fg_color.b);
        mgorient (radius[i][t] * adim);
        glColor4f (mggris.r, mggris.v, mggris.b, 0.2f);
        /* mgsphere(radius[i][t] * adim); */
        glutSolidSphere (radius[i][t] * adim, nb_subdiv_sphere, nb_subdiv_sphere_2);
        }
      else
        {
        glColor4f (mggris.r, mggris.v, mggris.b, alpha_color);
        /* 	  mgsphere(radius[i][t] * adim); */
        glutSolidSphere (radius[i][t] * adim, nb_subdiv_sphere, nb_subdiv_sphere_2);
        }
      break;
      
    case MGP_POLYG:
      
      mgpolyg(i);
      if (orient)
        mgorient (radius[i][t] * adim);
        break;
      
    case MGP_SEGMT:
      
      mgSegmt(radius[i][t] * adim);
      break;
      
    case MGP_JONCx:
      
      mgjonc (datas[datadistrib[i]] * adim, datas[datadistrib[i] + 1] * adim);
      if (orient)
        mgorient ((datas[datadistrib[i]] + datas[datadistrib[i] + 1]) * adim);
        break;
      
    case MGP_POLYE:
      
      glColor4f (mggris.r, mggris.v, mggris.b, alpha_color);
      mgpolye_plein(i);
      break;
      
    case MGP_PICKT:
      
      mgPicket (datadistrib[i]);
      break;
      
    case MGP_MURX0:
    case MGP_MURX1:
      
      mgmurX (datas[datadistrib[i]] * adim);
      break;
      
    case MGP_MURY0:
    case MGP_MURY1:
      
      mgmurY (datas[datadistrib[i]] * adim);
      break;
      
    case MGP_MBR2D:
      
      mgmbr2D (datadistrib[i]);
      break;
      
    case MGP_CYLYx:
      
      mgCylY (datadistrib[i]);
      break;
    }
  
  if ((i == selectedBody) && (renderMode == GL_RENDER))
    {
    glColor3f(fg_color.r, fg_color.v, fg_color.b); 
    if (bdyty[selectedBody] == 1)
      glutWireSphere (radius[i][t] * adim * 1.01, nb_subdiv_sphere, nb_subdiv_sphere_2);
    
    glLineWidth(1.0);
    }
  
  if (bodies_numbers)
    bodies_number (i, t);
  
  glPopMatrix ();
}

void mgbodyselect_plein (int i, int t)
{
  glPushMatrix ();

  glRotatef (rot[i][t] * MG_RAD2DEG, 0.0f, -1.0f, 0.0f);

  if (renderMode == GL_SELECT)
    glLoadName(i);

  if ((i == selectedBody) && (renderMode == GL_RENDER))
    glLineWidth(4.0f);

  switch (bdyty[i])
    {

    case MGP_DISKx:

      mgdisk_plein (radius[i][t] * adim);
      if (orient) mgorient (radius[i][t] * adim);
      break;

    case MGP_SPHER:

      /* mgsphere(radius[i][t] * adim); */
      glutSolidSphere (radius[i][t] * adim, nb_subdiv_sphere, nb_subdiv_sphere_2);
      break;

    case MGP_POLYG:

      mgpolyg_plein (i);
      if (orient) mgorient (radius[i][t] * adim);
      break;

    case MGP_SEGMT:

      mgSegmt(radius[i][t] * adim);
      break;

    case MGP_JONCx:

      mgjonc_plein (datas[datadistrib[i]] * adim, datas[datadistrib[i] + 1] * adim);
      if (orient) mgorient ((datas[datadistrib[i]] + datas[datadistrib[i] + 1]) * adim);
      break;

    case MGP_POLYE:

      mgpolye_plein(i);
      break;

    case MGP_PICKT:

      mgPicket (datadistrib[i]);
      break;

    case MGP_MURX0:
    case MGP_MURX1:

      mgmurX (datas[datadistrib[i]] * adim);
      break;

    case MGP_MURY0:
    case MGP_MURY1:

      mgmurY (datas[datadistrib[i]] * adim);
      break;

    case MGP_CYLYx:

      mgCylY (datadistrib[i]);
      break;
    }

  if ((i == selectedBody) && (renderMode == GL_RENDER))
    {
      glColor3f(fg_color.r, fg_color.v, fg_color.b); 
      if (bdyty[selectedBody] == MGP_SPHER)
       glutWireSphere (radius[i][t] * adim * 1.01, nb_subdiv_sphere, nb_subdiv_sphere_2);

      glLineWidth(1.0f);
    }
 
  if (bodies_numbers)
    bodies_number (i, t);

  glPopMatrix ();
}

void mgbodyselectPOV (int i, int t)
{
  switch (bdyty[i])
    {
    case MGP_DISKx:

      fprintf (povfile, "cylinder { <%lg, %lg, %lg> <%lg, %lg, %lg> %lg }\n",
	       x[i][t], y[i][t], -(z[i][t] - r_moy * 5.0),
               x[i][t], y[i][t], -(z[i][t] + r_moy * 5.0), radius[i][t]);
      break;

    case MGP_SPHER:

      fprintf (povfile, "sphere { <%lg, %lg, %lg> %lg }\n",
	       x[i][t], y[i][t], -z[i][t], radius[i][t]);
      break;

    }
}

void mgbodyselectPS (int i, int t)
{
  double angldeb, anglfin, angle;
  double xjonc, yjonc;

  switch (bdyty[i])
    {
    case MGP_DISKx:

      fprintf (psfile, "newpath  %g %g %g 0.0 360.0 arc closepath\n0.0 setgray stroke\n",
	       (x[i][t] - xminB) * psZoom + 2, (y[i][t] - yminB) * psZoom + 2, radius[i][state] * psZoom);
      break;

    case MGP_JONCx:

      angle = rot[i][t];

      angldeb = -(1.5707963 - angle) * 57.295779;
      anglfin = angldeb + 180.0;
      if (angldeb < 0.0)
	angldeb = angldeb + 360.0;
      if (anglfin < 0.0)
	anglfin = anglfin + 360.0;
      if (angldeb > 360.0)
	angldeb = angldeb - 360.0;
      if (anglfin > 360.0)
	anglfin = anglfin - 360.0;
      fprintf (psfile, "newpath\n");
      xjonc = datas[datadistrib[i]];
      yjonc = datas[datadistrib[i] + 1];
      fprintf (psfile, "%g %g %g %g %g arc\n",
	       (x[i][t] - xminB + xjonc * cos (angle)) * psZoom + 2,
	       (y[i][t] - yminB + xjonc * sin (angle)) * psZoom + 2, yjonc * psZoom, angldeb, anglfin);
      angldeb = anglfin;
      anglfin = angldeb + 180.0;
      if (anglfin > 360.0)
	anglfin = anglfin - 360.0;
      fprintf (psfile, "%g %g lineto\n",
	       (x[i][t] - xminB - (xjonc * cos (angle) + yjonc * sin (angle))) * psZoom + 2,
	       (y[i][t] - yminB - (xjonc * sin (angle) - yjonc * cos (angle))) * psZoom + 2);
      fprintf (psfile, "%g %g %g %g %g arc\n",
	       (x[i][t] - xminB - xjonc * cos (angle)) * psZoom + 2,
	       (y[i][t] - yminB - xjonc * sin (angle)) * psZoom + 2, yjonc * psZoom, angldeb, anglfin);
      fprintf (psfile, "closepath\n0.0 setgray stroke\n");
      break;
    }

}

void mglinkPS (int state, double larg, int i, int j)
{
  double x1, x2, x3, x4;
  double y1, y2, y3, y4;
  double dist, cosa, sina, ls, lc;

  dist = sqrt (pow ((x[i][state] - x[j][state]), 2.0) + pow ((y[i][state] - y[j][state]), 2.0));
  cosa = (x[j][state] - x[i][state]) / dist;
  sina = (y[j][state] - y[i][state]) / dist;
 
  lc = larg * cosa;
  ls = larg * sina;

  x1 = ( ls + x[i][state] - xminB) * psZoom + 2;
  x2 = ( ls + x[j][state] - xminB) * psZoom + 2;
  x3 = (-ls + x[j][state] - xminB) * psZoom + 2;
  x4 = (-ls + x[i][state] - xminB) * psZoom + 2;

  y1 = (-lc + y[i][state] - yminB) * psZoom + 2;
  y2 = (-lc + y[j][state] - yminB) * psZoom + 2;
  y3 = ( lc + y[j][state] - yminB) * psZoom + 2;
  y4 = ( lc + y[i][state] - yminB) * psZoom + 2;

  fprintf (psfile, "newpath\n %g %g moveto\n %g %g lineto\n %g %g lineto\n %g %g lineto\nclosepath\nfill\n",
	            x1, y1, x2, y2, x3, y3, x4, y4);

}

void mgbodyselectPS_plein (int i, int t, int coul)
{
  double angldeb, anglfin, angle;
  double xjonc, yjonc;


  switch (bdyty[i])
    {
    case 0:
      fprintf (psfile, "newpath  %g %g %g 0.0 360.0 arc closepath\n%g setgray fill stroke\n",
	       (x[i][t] - xminB) * psZoom + 2, (y[i][t] - yminB) * psZoom + 2,
	       radius[i][state] * psZoom, (1.0 - (float) coul / (float) nb_val_couleurs));
      break;
    case 4:
      angle = rot[i][t];

      angldeb = -(1.5707963 - angle) * 57.295779;
      anglfin = angldeb + 180.0;
      if (angldeb < 0.0)
	angldeb = angldeb + 360.0;
      if (anglfin < 0.0)
	anglfin = anglfin + 360.0;
      if (angldeb > 360.0)
	angldeb = angldeb - 360.0;
      if (anglfin > 360.0)
	anglfin = anglfin - 360.0;
      fprintf (psfile, "newpath\n");
      xjonc = datas[datadistrib[i]];
      yjonc = datas[datadistrib[i] + 1];
      fprintf (psfile, "%g %g %g %g %g arc\n",
	       (x[i][t] - xminB + xjonc * cos (angle)) * psZoom + 2,
	       (y[i][t] - yminB + xjonc * sin (angle)) * psZoom + 2, yjonc * psZoom, angldeb, anglfin);
      angldeb = anglfin;
      anglfin = angldeb + 180.0;
      if (anglfin > 360.0)
	anglfin = anglfin - 360.0;
      fprintf (psfile, "%g %g lineto\n",
	       (x[i][t] - xminB - (xjonc * cos (angle) + yjonc * sin (angle))) * psZoom + 2,
	       (y[i][t] - yminB - (xjonc * sin (angle) - yjonc * cos (angle))) * psZoom + 2);
      fprintf (psfile, "%g %g %g %g %g arc\n",
	       (x[i][t] - xminB - xjonc * cos (angle)) * psZoom + 2,
	       (y[i][t] - yminB - xjonc * sin (angle)) * psZoom + 2, yjonc * psZoom, angldeb, anglfin);
      fprintf (psfile, "closepath\n%f setgray fill stroke\n",(1.0 - (float) coul / (float) nb_val_couleurs));
      break;
    }
  mgbodyselectPS (i, t);
}

/*==============================================================================
 * displaying informations  
 *==============================================================================*/

void bodies_number (int i, int t)
{
  int ii;
  char txt[10];
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;

  glDisable (GL_LIGHTING);

  glDisable (GL_DEPTH_TEST);

  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  sprintf (txt, "%d", i + 1);

  /* MEMO il faudrait deplacer tout ca car on l'appelle souvant ! voir les if(bodies_numbers)
     et aussi la fonction qui affiche les points. (La vitesse est toutefois correcte) */
  glPushMatrix ();
  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);

  glRasterPos3f (x[i][t]*adim, -z[i][t]*adim, y[i][t]*adim);

  for (ii = 0; txt[ii]; ii++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, txt[ii]);

  glPopMatrix ();

  glEnable (GL_DEPTH_TEST);
}

void mgorient (double r)
{
  glDisable (GL_LIGHTING);
  glBegin (GL_LINES);

  glVertex3f (0.0f, 0.0f, 0.0f);
  glVertex3f (r, 0.0f, 0.0f);

  if(!mode2D)
    {
      glVertex3f (0.0f, 0.0f, 0.0f);
      glVertex3f (0.0f, 0.0f, r);

      glVertex3f (0.0f, 0.0f, 0.0f);
      glVertex3f (0.0f, -r, 0.0f);
    }

  glEnd ();
  glEnable (GL_LIGHTING);
}

void pastille2d (int i, int j, double size)
{
  double xc,yc,extend;
  double inv_sumR = 1.0/(radius[i][state]+radius[j][state]);
  double facti = radius[i][state] * inv_sumR;
  double factj = radius[j][state] * inv_sumR;
  xc = (factj*x[i][state]+facti*x[j][state]);
  yc = (factj*y[i][state]+facti*y[j][state]);
  /*zc = 0.5 * (z[i][state]+z[j][state]);*/
  extend = 0.5 * size;
  int n;
  
  glDisable (GL_LIGHTING);
  glBegin (GL_POLYGON);
  
  for (n = 0; n < 360; n += 18)
        glVertex3f (adim*xc+adim*size * cos ((float)n * MG_DEG2RAD), 0.0f, adim*yc+adim*size * sin ((float)n * MG_DEG2RAD));
  /*
  glVertex3f((xc-extend)*adim,0.0f,(yc-extend)*adim);
  glVertex3f((xc+extend)*adim,0.0f,(yc-extend)*adim);
  glVertex3f((xc+extend)*adim,0.0f,(yc+extend)*adim);
  glVertex3f((xc-extend)*adim,0.0f,(yc+extend)*adim);
  */
  
  glEnd ();
  glEnable (GL_LIGHTING);
  /*mgdisk_plein (r * adim);*/ 
}

void disp_time ()
{
  int i;
  char timetxt[50];
  char stattxt[50];

  if (!multifiles)
    state_id = state;
  sprintf (stattxt, "State: %i", state_id);
  sprintf (timetxt, "Time : %5.4E", mgp_time[state]);
  glPushMatrix ();

  glViewport (0, 0, W, H);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0.0f, (GLdouble) W, 0.0f, (GLdouble) H);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);

  glColor4f (bg_color.r, bg_color.v, bg_color.b, 0.8f);
  glBegin (GL_POLYGON);
  glVertex2i (0, H - 44);
  glVertex2i (W, H - 44);
  glVertex2i (W, H);
  glVertex2i (0, H);
  glEnd ();

  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glRasterPos2i (5, (GLint) (H - 20));

  for (i = 0; stattxt[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, stattxt[i]);

  glRasterPos2i (5, (GLint) (H - 40));
  for (i = 0; timetxt[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, timetxt[i]);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0f, 1.0f, cut_distance, DISTANCE_MAX);
  glMatrixMode (GL_MODELVIEW);
  glEnable (GL_LIGHTING);

  if (W < H)
    glViewport ((GLint)((W - H) / 2), 0, H, H);
  else
    glViewport (0, (GLint)((H - W) / 2), W, W);

  glPopMatrix ();

  if (!mode2D)
    glEnable (GL_DEPTH_TEST);
}

void disp_func_name ()
{
  int i;

  glPushMatrix ();
  glViewport (0, 0, W, H);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0.0f, (GLdouble) W, 0.0f, (GLdouble) H);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);

  glColor4f (bg_color.r, bg_color.v, bg_color.b, 0.8f);
  glBegin (GL_POLYGON);
  glVertex2i (0, 0);
  glVertex2i (W, 0);
  glVertex2i (W, 44);
  glVertex2i (0, 44);
  glEnd ();

  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glRasterPos2i (10, 25);
  for (i = 0; funcname[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, funcname[i]);

  glRasterPos2i (10, 4);
  for (i = 0; funcname[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, supfuncname[i]);

  glPointSize (8.0f);
  glColor3f (fg_color1.r, fg_color1.v, fg_color1.b);
  glBegin (GL_POINTS);
  if (affgradcolor)
    glVertex2i (5, 34);
  if (affgradlinkcolor)
    glVertex2i (5, 11);
  glEnd ();

  glEnable (GL_LIGHTING);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0f, 1.0f, cut_distance, DISTANCE_MAX);
  glMatrixMode (GL_MODELVIEW);

  if (W < H)
    glViewport ((GLint)((W - H) / 2), 0, H, H);
  else
    glViewport (0, (GLint)((H - W) / 2), W, W);

  glPopMatrix ();

  if (!mode2D)
    glEnable (GL_DEPTH_TEST);
}

void disp_text (char * text)
{
  int i;
  GLint x, y;

  glPushMatrix ();
  glViewport (0, 0, W, H);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0.0f, (GLdouble) W, 0.0f, (GLdouble) H);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);

  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  x = (GLint) (0.5 * W - strlen(text) * 6.0);
  y = (GLint) (0.5 * H - 12.0);
  glRasterPos2i (x, y);

  for (i = 0; text[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_TIMES_ROMAN_24, text[i]);

  glEnable (GL_LIGHTING);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0f, 1.0f, cut_distance, DISTANCE_MAX);
  glMatrixMode (GL_MODELVIEW);

  if (W < H)
    glViewport ((GLint)((W - H) / 2), 0, H, H);
  else
    glViewport (0, (GLint)((H - W) / 2), W, W);

  glPopMatrix ();

  if (!mode2D)
    glEnable (GL_DEPTH_TEST);
}

void graph_position (int xm, int xM, int ym, int yM)
{
  graph_xm = xm;
  graph_xM = xM;
  graph_ym = ym;
  graph_yM = yM;
}

void graph_open ()
{
  glPushMatrix ();
  glViewport (0, 0, W, H);

  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0.0f, (GLdouble) W, 0.0f, (GLdouble) H);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();

  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);

  glColor4f (bg_color.r, bg_color.v, bg_color.b, 0.8f);
  glBegin (GL_POLYGON);
  glVertex2i (graph_xm, graph_ym);
  glVertex2i (graph_xM, graph_ym);
  glVertex2i (graph_xM, graph_yM);
  glVertex2i (graph_xm, graph_yM);
  glEnd ();
}

void graph_close ()
{
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0f, 1.0f, cut_distance, DISTANCE_MAX);
  glMatrixMode (GL_MODELVIEW);

  if (W < H)
    glViewport ((GLint)((W - H) / 2), 0, H, H);
  else
    glViewport (0, (GLint)((H - W) / 2), W, W);

  glPopMatrix ();

  if (mode2D == MG_FALSE)
    glEnable (GL_DEPTH_TEST);
}

void plot_hist_fn()
{
  int i; 
  double *gx, *gy;
  double Fnmin,Fnmax;
  int nc;
  unsigned int nbins=60,ibin;
  double bin;
  
  nc=0;
  for(i=0;i<nbel;++i) nc += nbneighbors[i][state];
  
  Fnmin=Fnmax=Fn[0][state];  
  for(i=1;i<nc;++i)
  {
    Fnmin = (Fnmin > Fn[i][state]) ? Fn[i][state] : Fnmin;
    Fnmax = (Fnmax < Fn[i][state]) ? Fn[i][state] : Fnmax;
  }
  bin=(Fnmax-Fnmin)/(double)(nbins);
  if (bin == 0.) return;
  
  gx = (double *) calloc ((size_t)nbins, sizeof (double));
  gy = (double *) calloc ((size_t)nbins, sizeof (double));
  
  for(i=0;i<nc;++i)
    if (Fn[i][state]!=0.)
    {
      ibin = (unsigned int)floor((Fn[i][state]-Fnmin)/bin);
      if(ibin<nbins) gy[ibin]+=1.;
    }
  
  for(i=0;i<nbins;++i) gx[i] = Fnmin + i*bin;
  
  graph_position((int)((double)W / 6.0), (int)(5.0 * (double)W / 6.0),
                 (int)((double)H / 6.0), (int)(5.0 * (double)H / 6.0) );
  graph_open();
  graph_trace_buffer (gx, gy, nbins, 1, 1);
  graph_close();
  
  free (gx);
  free (gy);
  
}

void plot_fn_vs_ft()
{
  int i; 
  double *gx, *gy;
  int nc;

  nc=0;
  for(i=0;i<nbel;++i) nc += nbneighbors[i][state];
  gx = (double *) calloc ((size_t)nc, sizeof (double));
  gy = (double *) calloc ((size_t)nc, sizeof (double));

  if(mode2D)
    {
      for(i=0;i<nc;++i)
	{
	  gx[i]=Fn[i][state];
	  gy[i]=Ft[i][state];
	}
    }
  else
    {
      for(i=0;i<nc;++i)
	{
	  gx[i]=Fn[i][state];
	  gy[i]=sqrt(Ft[i][state]*Ft[i][state]+Fs[i][state]*Fs[i][state]);
	}
    }

  graph_position((int)((double)W / 6.0), (int)(5.0 * (double)W / 6.0),
                 (int)((double)H / 6.0), (int)(5.0 * (double)H / 6.0) );
  graph_open();
  graph_trace_buffer (gx, gy, nc, 1, 0);
  graph_close();

  free (gx);
  free (gy);

}

void plot_ncloud ()
{
  int i,j,k,anta;
  double nx,ny,nz,invnorm;

  GLfloat scal;

  scal=.5*Lmax*adim; 

  glPointSize (1.0f);
  glBegin (GL_POINTS);

  k =0;
  for (i = 0; i < nbel; ++i)
    for (j = 0; j < nbneighbors[i][state]; ++j)
      {
	anta = neighbor[k][state] - 1;
	nx=x[i][state]-x[anta][state];
	ny=y[i][state]-y[anta][state];
	nz=z[i][state]-z[anta][state];
	invnorm=1./sqrt(nx*nx+ny*ny+nz*nz);

	nx*=invnorm;
	ny*=invnorm;
	nz*=invnorm;

	if(Fn[k][state]>0.)
	  {
	    glColor3f (1.0f,0.0f,0.0f);
	    glVertex3f (nx*scal,-nz*scal,ny*scal);
	    glVertex3f (-nx*scal,nz*scal,-ny*scal);
	  }
	else if (Fn[k][state]<0.)
	  {
	    glColor3f (0.0f,0.0f,1.0f);	  
	    glVertex3f (nx*scal,-nz*scal,ny*scal);
	    glVertex3f (-nx*scal,nz*scal,-ny*scal);
	  }

	++k;
      }
  glEnd ();
}


void plot_distri_TCT_3d ()
{
	int i, j, current_adh = 0, anta;
	int coul;
	GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
	int secur = 10;
	double angle_section1;
	double angle_section2;
	double pijx, pijy, pijz, pijXZ;
	double dir1 = 0.0; /* 0 -> 2PI  */
	double dir2 = 0.0; /* 0 -> PI/2 */
	double **P;
	double norm_P = 0;
	double Fmoy, nbf;
	double scal;
	double P1, P2;
	double gx1, gy1, gz1;
	double gx2, gy2, gz2;
	double gx3, gy3, gz3;
	double gx4, gy4, gz4;
	double normx, normy, normz, normalize;
	unsigned int contact_vertical;
	
	if (!multifiles)
		state_id = state;
	
	P = (double **) calloc ((size_t) (nb_section1 + secur), sizeof (double));
	if (P == NULL)
		fprintf (stderr, "Error, memory allocation\n");
	
	for (i = 0; i < (nb_section1 + secur); i++)
    {
		P[i] = (double *) calloc ((size_t) (nb_section2 + secur), sizeof (double));
		if (P == NULL)
			fprintf (stderr, "Error, memory allocation\n");
    }
	
	angle_section1 = 2.0 * M_PI / (double) nb_section1;
	angle_section2 = 0.5 * M_PI / (double) nb_section2;
	
	current_adh = 0;
	nbf = Fmoy = 0.0;
	for (i = 0; i < nbel; i++)
		for (j = 0; j < nbneighbors[i][state]; j++)
		{
			anta = neighbor[current_adh][state] - 1;
			
			if (Fn[current_adh][state]>0.0) 
			{
				Fmoy += Fn[current_adh][state];
				nbf += 1.0;
			}
			
			current_adh++;
		}
			
	if(nbf > 0.0)
	  Fmoy = Fmoy / nbf;
	
	current_adh = 0;
	for (i = 0; i < nbel; i++)
		for (j = 0; j < nbneighbors[i][state]; j++)
		{
			anta = neighbor[current_adh][state] - 1;
			
			if (Fn[current_adh][state] > 0.0 /*Fmoy*/)
			{
				pijx = (x[anta][state] - x[i][state]);
				pijy = (y[anta][state] - y[i][state]);
				pijz = (z[anta][state] - z[i][state]);
				
				if (pijy < 0.0)
				{
					pijx = -pijx;
					pijy = -pijy;
					pijz = -pijz;
				}
				
				contact_vertical = 0;
				if(pijz>0.0)
				{
					if (pijx>0.0)  dir1 = atan(pijz/pijx);
					if (pijx==0.0) dir1 = M_PI_2;
					if (pijx<0.0)  dir1 = M_PI - atan(pijz/(-pijx));
				}
				if(pijz==0.0)
				{
					if (pijx>0.0)  dir1 = 0.0;
					if (pijx==0.0) contact_vertical = 1;
					if (pijx<0.0)  dir1 = M_PI;
				}
				if(pijz<0.0)
				{
					if (pijx>0.0)  dir1 = 2.0 * M_PI - atan((-pijz)/pijx);
					if (pijx==0.0) dir1 = 3.0 * M_PI_2;
					if (pijx<0.0)  dir1 = M_PI + atan(pijz/pijx);;
				}
				
				if (!contact_vertical)
				{
					pijXZ = sqrt(pijx*pijx + pijz*pijz);
					dir2=atan(pijy/pijXZ);
				}
				else
				{
					dir1 = 0.0;
					dir2 = M_PI_2;
				}
				
				P[(int) (dir1 / angle_section1)][(int) (dir2 / angle_section2)] += 1.0 ;
				norm_P += 1.0 ;				
			}
			current_adh++;
		}
			
			if (norm_P == 0.0) return;
	
	P2=P[0][0];
	
	for (i = 0; i < nb_section1; i++)
		for (j = 0; j <= nb_section2; j++)
			if (P[i][j]>P2) P2 = P[i][j];
	
	scal = adim * xmaxB / P2;
	valc_bleu = 0.0;
	valc_rouge = P2;
	
	glLoadIdentity ();
	gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
	glRotatef (phi, 1.0f, 0.0f, 0.0f);
	glRotatef (theta, 0.0f, 0.0f, 1.0f);
	
	if (shade_polye)
		glEnable (GL_LIGHTING);
	else
		glDisable (GL_LIGHTING);
	
	glEnable(GL_POLYGON_OFFSET_FILL);
	
	glPolygonOffset(1.0f, 1.0f);
	
	for (i = 0; i < nb_section1; i++)
		for (j = 0; j <= nb_section2; j++)
		{
			P1 = P[i][j] * cos((j - 0.5) * angle_section2);
			P2 = P[i][j] * cos((j + 0.5) * angle_section2);
			
			gx1 = cos((i - 0.5) * angle_section1) * P1 * scal;
			gz1 = sin((i - 0.5) * angle_section1) * P1 * scal;
			gy1 = sin((j - 0.5) * angle_section2) * P[i][j] * scal;
			
			gx2 = cos((i - 0.5) * angle_section1) * P2 * scal;
			gz2 = sin((i - 0.5) * angle_section1) * P2 * scal;
			gy2 = sin((j + 0.5) * angle_section2) * P[i][j] * scal;
			
			gx3 = cos((i + 0.5) * angle_section1) * P2 * scal;
			gz3 = sin((i + 0.5) * angle_section1) * P2 * scal;
			gy3 = sin((j + 0.5) * angle_section2) * P[i][j] * scal;
			
			gx4 = cos((i + 0.5) * angle_section1) * P1 * scal;
			gz4 = sin((i + 0.5) * angle_section1) * P1 * scal;
			gy4 = sin((j - 0.5) * angle_section2) * P[i][j] * scal;
			
			
			coul = (int) (nb_val_couleurs * (P[i][j]) / (valc_rouge)) + 1;
			if (coul < 0)
				coul = 0;
			if (coul > nb_val_couleurs)
				coul = nb_val_couleurs;
			glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);
			
			glBegin(GL_QUADS);
			
			if (shade_polye)
			{
				normx = (gx2 - gx1) * (gy3 - gy1) - (gy2 - gy1) * (gx3 - gx1);
				normy = (gz2 - gz1) * (gx3 - gx1) - (gx2 - gx1) * (gz3 - gz1);
				normz = (gy2 - gy1) * (gz3 - gz1) - (gz2 - gz1) * (gy3 - gy1);
				normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);
				
				glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
			}
			
			glVertex3f (gx1, -gz1 , gy1);
			glVertex3f (gx2, -gz2 , gy2);
			glVertex3f (gx3, -gz3 , gy3);
			glVertex3f (gx4, -gz4 , gy4);
			
			glEnd();
			
			glBegin(GL_TRIANGLES);
			if (shade_polye)
			{
				normx = (gx1) * (gy2) - (gy1) * (gx2);
				normy = (gz1) * (gx2) - (gx1) * (gz2);
				normz = (gy1) * (gz2) - (gz1) * (gy2);
				normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);
				
				glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
			}
			glVertex3f (0.0,0.0,0.0);
			glVertex3f (gx1, -gz1 , gy1);
			glVertex3f (gx2, -gz2 , gy2);
			glEnd();
			
			glBegin(GL_TRIANGLES);
			if (shade_polye)
			{
				normx = (gx2) * (gy3) - (gy2) * (gx3);
				normy = (gz2) * (gx3) - (gx2) * (gz3);
				normz = (gy2) * (gz3) - (gz2) * (gy3);
				normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);
				
				glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
			}
			glVertex3f (0.0,0.0,0.0);
			glVertex3f (gx2, -gz2 , gy2);
			glVertex3f (gx3, -gz3 , gy3);
			
			glEnd();
			
			glBegin(GL_TRIANGLES);
			if (shade_polye)
			{
				normx = (gx3) * (gy4) - (gy3) * (gx4);
				normy = (gz3) * (gx4) - (gx3) * (gz4);
				normz = (gy3) * (gz4) - (gz3) * (gy4);
				normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);
				
				glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
			}
			glVertex3f (0.0,0.0,0.0);
			glVertex3f (gx3, -gz3 , gy3);
			glVertex3f (gx4, -gz4 , gy4);
			
			glEnd();
			
			glBegin(GL_TRIANGLES);
			if (shade_polye)
			{
				normx = (gx4) * (gy1) - (gy4) * (gx1);
				normy = (gz4) * (gx1) - (gx4) * (gz1);
				normz = (gy4) * (gz1) - (gz4) * (gy1);
				normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);
				
				glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
			}
			glVertex3f (0.0,0.0,0.0);
			glVertex3f (gx4, -gz4 , gy4);
			glVertex3f (gx1, -gz1 , gy1);
			
			glEnd();
			
			if (!shade_polye)
			{
				glColor3f (fg_color.r, fg_color.v, fg_color.b );
				
				glBegin(GL_LINE_LOOP);
				
				glVertex3f (0.0,0.0,0.0);
				glVertex3f (gx1, -gz1 , gy1);
				glVertex3f (gx2, -gz2 , gy2);
				glVertex3f (0.0,0.0,0.0);
				glVertex3f (gx3, -gz3 , gy3);
				glVertex3f (gx4, -gz4 , gy4);
				glVertex3f (0.0,0.0,0.0);
				
				glEnd();
				
				glBegin(GL_LINES);
				glVertex3f (gx1, -gz1 , gy1);
				glVertex3f (gx4, -gz4 , gy4);
				glEnd();
				
				glBegin(GL_LINES);
				glVertex3f (gx2, -gz2 , gy2);
				glVertex3f (gx3, -gz3 , gy3);
				glEnd();
			}
		}
			
			glDisable(GL_POLYGON_OFFSET_FILL);
	
	free (P);
}

void plot_distri_TCT()
{
  int i, j, current_adh = 0, anta;
  int nb_section = 32;
  int secur = 10;
  double angle_section;
  double pijx, pijy;
  double dir1 = 0.0;
  double *P;
  double norm_P = 0;
  double *gx, *gy;
  vect2d *pij, *X;
  double Fn_moy=0.0;
  int nfn=0;

  if (!multifiles)
    state_id = state;

  P = (double *) calloc ((size_t)(nb_section + secur) , sizeof (double));
  gx = (double *) calloc ((size_t)(nb_section * 2 + 1 + secur) , sizeof (double));
  gy = (double *) calloc ((size_t)(nb_section * 2 + 1 + secur) , sizeof (double));

  angle_section = 2.0 * M_PI / (double) nb_section;

  current_adh =0;
  for (i = 0; i < nbel; i++)
    for (j = 0; j < nbneighbors[i][state]; j++)
      {
	anta = neighbor[current_adh][state] - 1;
	if (Fn[current_adh][state] != 0.0)
	  {
	    nfn++;
	    Fn_moy += Fn[current_adh][state];
	  }

	current_adh++;
      }
  Fn_moy = Fn_moy/(double)nfn;

  current_adh =0;
  for (i = 0; i < nbel; i++)
    for (j = 0; j < nbneighbors[i][state]; j++)
      {
	anta = neighbor[current_adh][state] - 1;

	if (Fn[current_adh][state] != 0.0)
	  {
	    pijx = (x[anta][state] - x[i][state]);
	    pijy = (y[anta][state] - y[i][state]);

	    if (pijy<0.0)
	      {
		pijx=-pijx;
		pijy=-pijy;
	      }

	    pij = me_vect2d_new(pijx, pijy);
	    X = me_vect2d_new(1.0, 0.0); /* , !!*/

	    dir1 = me_vect2d_angle_rad(X, pij);

	    /* Fn forts et Fn faibles*/
	    if (Fn[current_adh][state] > Fn_moy)
	      {
		P[(int) (dir1 / angle_section)] += 1.0;
	      }
	    else if (Fn[current_adh][state]>0.0)
	      {
		P[(int) ((dir1 + M_PI) / angle_section)] += 1.0;
	      }

	    /* Fn+ et Fn-*/
/* 	    if (Fn[current_adh][state] > 0.0) */
/* 	      { */
/* 		P[(int) (dir1 / angle_section)] += 1.0; */
/* 	      } */
/* 	    else */
/* 	      { */
/* 		P[(int) ((dir1 + M_PI) / angle_section)] += 1.0; */
/* 	      } */

	    norm_P += 1.0;
	  }
	current_adh++;
      }

  if (norm_P != 0.0)
    {
      double Pi_adim;
      double a1, a2;

      for (i = 0; i <= nb_section; i++)
	{
          Pi_adim = P[i] / norm_P;
	  a1 = (double)i * angle_section;
	  a2 = (double)(i + 1) * angle_section;

	  gx[2 * i] = Pi_adim * cos(a1) ;
	  gy[2 * i] = Pi_adim * sin(a1);

	  gx[2 * i + 1] = Pi_adim * cos(a2);
	  gy[2 * i + 1] = Pi_adim * sin(a2);
	}

/*       gx[2 * nb_section - 1] = (P[0] / norm_P); */
/*       gy[2 * nb_section - 1] = 0.0; */

      gx[2 * nb_section+2] = (P[0] / norm_P);
      gy[2 * nb_section+2] = 0.0;

    }

  graph_position((int)((double)W / 6.0), (int)(5.0 * (double)W / 6.0),
                 (int)((double)H / 6.0), (int)(5.0 * (double)H / 6.0) );
  graph_open();
  graph_trace_buffer (gx, gy, 2 * nb_section, 1, 1);
  graph_close();

  free (P);
  free (gx);
  free (gy);
}


void plot_hist_ny()
{
  int i,j,k,anta;
  double nx,nz;
  double *ny,*ind;

  ny = (double *)calloc((size_t)nbel*12,sizeof(double));
  ind = (double *)calloc((size_t)nbel*12,sizeof(double));

  k =0;
  for (i = 0; i < nbel; ++i)
    for (j = 0; j < nbneighbors[i][state]; ++j)
      {
	anta = neighbor[k][state] - 1;
	nx   =x[i][state]-x[anta][state];
	ny[k]=y[i][state]-y[anta][state];
	nz   =z[i][state]-z[anta][state];
	if(ny[k]<0.) {nx=-nx;ny[k]=-ny[k];nz=-nz;}
	ny[k]=ny[k]/sqrt(nx*nx+ny[k]*ny[k]+nz*nz);
	ind[k]=k;

	++k;
      }

  qsort((void*)ny,(size_t)k,sizeof(double),compare);

  graph_position((int)((double)W / 6.0), (int)(5.0 * (double)W / 6.0),
                 (int)((double)H / 6.0), (int)(5.0 * (double)H / 6.0) );
  graph_open();
  graph_trace_buffer (ny, ind, k, 1, 1);
  graph_close();

  free (ind);
  free (ny);
}

void plot_granulo()
{
  int i,nd;
  double *d,*passing;
  
  d = (double *)calloc((size_t)nbel,sizeof(double));
  passing = (double *)calloc((size_t)nbel,sizeof(double));
  
  nd=0;
  for (i = 0; i < nbel; ++i)
    if (bdyty[i]==MGP_SPHER)  
      {
      d[nd]=2.*radius[i][state];
      passing[nd]=nd;
      ++nd;
      }
      
  qsort((void*)d,(size_t)nd,sizeof(double),compare);
  
  graph_position((int)((double)W / 6.0), (int)(5.0 * (double)W / 6.0),
                 (int)((double)H / 6.0), (int)(5.0 * (double)H / 6.0) );
  graph_open();
  graph_trace_buffer (d, passing, nd, 1, 0);
  graph_close();
  
  free (d);
  free (passing);
}

void plot_distri_TCT2_3d ()
{
  int i, j, current_adh = 0, anta;
  int coul;
  GLdouble Xcam = Xviewp * TRANS_CAM_FACTOR, Ycam = Yviewp * TRANS_CAM_FACTOR;
  int secur = 10;
  double angle_section1;
  double angle_section2;
  double pijx, pijy, pijz, pijXZ;
  double dir1 = 0.0;
  double dir2 = 0.0;
  double **P;
  double norm_P = 0;

  double scal;
  double P1, P2;
  double gx1, gy1, gz1;
  double gx2, gy2, gz2;
  double gx3, gy3, gz3;
  double gx4, gy4, gz4;
  double normx, normy, normz, normalize;
  unsigned int contact_vertical;

  if (!multifiles)
    state_id = state;

  P = (double **) calloc ((size_t) (nb_section1 + secur), sizeof (double));
  if (P == NULL)
    fprintf (stderr, "Error, memory allocation\n");

  for (i = 0; i < (nb_section1 + secur); i++)
    {
      P[i] = (double *) calloc ((size_t) (nb_section2 + secur), sizeof (double));
      if (P == NULL)
	fprintf (stderr, "Error, memory allocation\n");
    }

  angle_section1 = 2.0 * M_PI / (double) nb_section1;
  angle_section2 = 0.5 * M_PI / (double) nb_section2;

  for (i = 0; i < nbel; i++)
    for (j = 0; j < nbneighbors[i][state]; j++)
      {
	anta = neighbor[current_adh][state] - 1;

	if (Fn[current_adh][state] > 0.0)
	  {
	    pijx = (x[anta][state] - x[i][state]);
	    pijy = (y[anta][state] - y[i][state]);
	    pijz = (z[anta][state] - z[i][state]);

	    if (pijy < 0.0)
	      {
		pijx = -pijx;
		pijy = -pijy;
		pijz = -pijz;
	      }

	    contact_vertical = 0;
	    if(pijz>0.0)
	      {
		if (pijx>0.0)  dir1 = atan(pijz/pijx);
		if (pijx==0.0) dir1 = M_PI_2;
		if (pijx<0.0)  dir1 = M_PI - atan(pijz/(-pijx));
	      }
	    if(pijz==0.0)
	      {
		if (pijx>0.0)  dir1 = 0.0;
		if (pijx==0.0) contact_vertical = 1;
		if (pijx<0.0)  dir1 = M_PI;
	      }
	    if(pijz<0.0)
	      {
		if (pijx>0.0)  dir1 = 2.0*M_PI - atan((-pijz)/pijx);
		if (pijx==0.0) dir1 = 3.0*M_PI_2;
		if (pijx<0.0)  dir1 = M_PI + atan(pijz/pijx);;
	      }

	    if (!contact_vertical)
	      {
		pijXZ = sqrt(pijx*pijx+pijz*pijz);
		dir2=atan(pijy/pijXZ);
	      }
	    else
	      {
		dir1 = 0.0;
		dir2 = M_PI_2;
	      }

	    P[(int) (dir1 / angle_section1)][(int) (dir2 / angle_section2)] += Fn[current_adh][state];
	    norm_P += Fn[current_adh][state];

/* 	    if (dir2 == 0.0) */
/* 	      { */
/* 		if (dir1 == 0.0) */
/* 		  { */
/* 		    fprintf(stdout,"probleme : dir1 = dir2 = 0 */
/*                                   \nVous utilisez peut-etre une visualisation 3D pour une simulation 2D !\n"); */
/* 		  } */

/* 		if (dir1 < M_PI) */
/* 		  { */
/* 		    P[(int) ((dir1 + M_PI) / angle_section1)][(int) (dir2 / angle_section2)] += 1.0; */
/* 		    norm_P += 1.0; */
/* 		  } */
		
/* 		if (dir1 == M_PI) */
/* 		  { */
/* 		    P[0][(int) (dir2 / angle_section2)] += 1.0; */
/* 		    norm_P += 1.0; */
/* 		  } */

/* 		if (dir1 > M_PI) */
/* 		  { */
/* 		    P[(int) ((dir1 - M_PI) / angle_section1)][(int) (dir2 / angle_section2)] += 1.0; */
/* 		    norm_P += 1.0; */
/* 		  } */
/* 	      } */

	  }
	current_adh++;
      }

  if (norm_P == 0.0) return;

  P2=P[0][0];

  for (i = 0; i < nb_section1; i++)
    for (j = 0; j <= nb_section2; j++)
      if (P[i][j]>P2) P2 = P[i][j];

  scal = adim * xmaxB / P2;
  valc_bleu = 0.0;
  valc_rouge = P2;

  glLoadIdentity ();
  gluLookAt (Xcam, Ycam, distance, Xcam, Ycam, 0.0f, 0.0f, 1.0f, 0.0f);
  glRotatef (phi, 1.0f, 0.0f, 0.0f);
  glRotatef (theta, 0.0f, 0.0f, 1.0f);

  if (shade_polye)
    glEnable (GL_LIGHTING);
  else
    glDisable (GL_LIGHTING);

  glEnable(GL_POLYGON_OFFSET_FILL);

  glPolygonOffset(1.0f, 1.0f);

  for (i = 0; i < nb_section1; i++)
    for (j = 0; j <= nb_section2; j++)
      {
	P1 = P[i][j] * cos((j - 0.5) * angle_section2);
	P2 = P[i][j] * cos((j + 0.5) * angle_section2);

	gx1 = cos((i - 0.5) * angle_section1) * P1 * scal;
	gz1 = sin((i - 0.5) * angle_section1) * P1 * scal;
        gy1 = sin((j - 0.5) * angle_section2) * P[i][j] * scal;

	gx2 = cos((i - 0.5) * angle_section1) * P2 * scal;
	gz2 = sin((i - 0.5) * angle_section1) * P2 * scal;
        gy2 = sin((j + 0.5) * angle_section2) * P[i][j] * scal;

	gx3 = cos((i + 0.5) * angle_section1) * P2 * scal;
	gz3 = sin((i + 0.5) * angle_section1) * P2 * scal;
        gy3 = sin((j + 0.5) * angle_section2) * P[i][j] * scal;

	gx4 = cos((i + 0.5) * angle_section1) * P1 * scal;
	gz4 = sin((i + 0.5) * angle_section1) * P1 * scal;
        gy4 = sin((j - 0.5) * angle_section2) * P[i][j] * scal;


	coul = (int) (nb_val_couleurs * (P[i][j]) / (valc_rouge)) + 1;
	if (coul < 0)
	  coul = 0;
	if (coul > nb_val_couleurs)
	  coul = nb_val_couleurs;
	glColor3f (gradc[coul].r, gradc[coul].v, gradc[coul].b);

	glBegin(GL_QUADS);

	if (shade_polye)
	  {
	    normx = (gx2 - gx1) * (gy3 - gy1) - (gy2 - gy1) * (gx3 - gx1);
	    normy = (gz2 - gz1) * (gx3 - gx1) - (gx2 - gx1) * (gz3 - gz1);
	    normz = (gy2 - gy1) * (gz3 - gz1) - (gz2 - gz1) * (gy3 - gy1);
	    normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);

	    glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
	  }

	glVertex3f (gx1, -gz1 , gy1);
	glVertex3f (gx2, -gz2 , gy2);
	glVertex3f (gx3, -gz3 , gy3);
	glVertex3f (gx4, -gz4 , gy4);

	glEnd();

	glBegin(GL_TRIANGLES);
	if (shade_polye)
	  {
	    normx = (gx1) * (gy2) - (gy1) * (gx2);
	    normy = (gz1) * (gx2) - (gx1) * (gz2);
	    normz = (gy1) * (gz2) - (gz1) * (gy2);
	    normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);

	    glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
	  }
	glVertex3f (0.0,0.0,0.0);
	glVertex3f (gx1, -gz1 , gy1);
	glVertex3f (gx2, -gz2 , gy2);
	glEnd();

	glBegin(GL_TRIANGLES);
	if (shade_polye)
	  {
	    normx = (gx2) * (gy3) - (gy2) * (gx3);
	    normy = (gz2) * (gx3) - (gx2) * (gz3);
	    normz = (gy2) * (gz3) - (gz2) * (gy3);
	    normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);

	    glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
	  }
	glVertex3f (0.0,0.0,0.0);
	glVertex3f (gx2, -gz2 , gy2);
	glVertex3f (gx3, -gz3 , gy3);

	glEnd();

	glBegin(GL_TRIANGLES);
	if (shade_polye)
	  {
	    normx = (gx3) * (gy4) - (gy3) * (gx4);
	    normy = (gz3) * (gx4) - (gx3) * (gz4);
	    normz = (gy3) * (gz4) - (gz3) * (gy4);
	    normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);

	    glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
	  }
	glVertex3f (0.0,0.0,0.0);
	glVertex3f (gx3, -gz3 , gy3);
	glVertex3f (gx4, -gz4 , gy4);

	glEnd();

	glBegin(GL_TRIANGLES);
	if (shade_polye)
	  {
	    normx = (gx4) * (gy1) - (gy4) * (gx1);
	    normy = (gz4) * (gx1) - (gx4) * (gz1);
	    normz = (gy4) * (gz1) - (gz4) * (gy1);
	    normalize = 1.0 / sqrt(normx * normx + normy * normy + normz * normz);

	    glNormal3f (normx * normalize, -normz * normalize, normy * normalize);
	  }
	glVertex3f (0.0,0.0,0.0);
	glVertex3f (gx4, -gz4 , gy4);
	glVertex3f (gx1, -gz1 , gy1);

	glEnd();

	if (!shade_polye)
	  {
	    glColor3f (fg_color.r, fg_color.v, fg_color.b );

	    glBegin(GL_LINE_LOOP);

	    glVertex3f (0.0,0.0,0.0);
	    glVertex3f (gx1, -gz1 , gy1);
	    glVertex3f (gx2, -gz2 , gy2);
	    glVertex3f (0.0,0.0,0.0);
	    glVertex3f (gx3, -gz3 , gy3);
	    glVertex3f (gx4, -gz4 , gy4);
	    glVertex3f (0.0,0.0,0.0);

	    glEnd();

	    glBegin(GL_LINES);
	    glVertex3f (gx1, -gz1 , gy1);
	    glVertex3f (gx4, -gz4 , gy4);
	    glEnd();

	    glBegin(GL_LINES);
	    glVertex3f (gx2, -gz2 , gy2);
	    glVertex3f (gx3, -gz3 , gy3);
	    glEnd();
	  }
      }

  glDisable(GL_POLYGON_OFFSET_FILL);

  free (P);
}


void graph_trace_buffer (double *xbuf, double *ybuf, int nbp, MGuchar axes, MGuchar opt)
{
  int i;
  double xmin,xmax,ymin,ymax;
  int x0,y0;
  double ex,ey;
  /* FILE *f; */

  xmin = xmax = xbuf[0];
  ymin = ymax = ybuf[0];
  for (i = 1; i < nbp; i++)
    {
      if (xbuf[i] > xmax)
	xmax = xbuf[i];
      if (ybuf[i] > ymax)
	ymax = ybuf[i];

      if (xbuf[i] < xmin)
	xmin = xbuf[i];
      if (ybuf[i] < ymin)
	ymin = ybuf[i];
    }

  if ((xmin != xmax) && ( ymin != ymax))
    {
      ex = (double)(graph_xM - graph_xm) / (double)(xmax - xmin);
      ey = (double)(graph_yM - graph_ym) / (double)(ymax - ymin);
    }
  else
    return;

  x0 = (int)(-xmin * ex) + graph_xm;
  y0 = (int)(-ymin * ey) + graph_ym;

  if(axes==1)
    {
      glColor3f (.0f, .0f, .0f);
      glLineWidth (1.0f);
      glBegin (GL_LINES);
      glVertex2i(graph_xm, y0);
      glVertex2i(graph_xM, y0);
      glVertex2i(x0, graph_ym);
      glVertex2i(x0, graph_yM);
      glEnd();
    }

  glColor3f (1.0f, 0.0f, 0.0f);

  if(opt==0)
    {
      glPointSize (2.0f);
      glBegin (GL_POINTS);
    }
  else if (opt==1)
    {
      glLineWidth (2.0f);
      glBegin (GL_LINE_STRIP);
    }

  for (i = 0 ; i < nbp ; i++)
    glVertex2i(x0 + (int)(xbuf[i] * ex), y0 + (int)(ybuf[i] * ey));

  /*
  f=fopen("trace.txt","w");
  for (i = 0 ; i < nbp ; i++)
    fprintf(f,"%lg %lg\n",xbuf[i],ybuf[i]);
  fclose(f);
   */

  glEnd();

}

void disp_grad_color ()
{
  int ii, j, step, Htot;
  char str[12];
  float init_val, inc_val;
  
  if ((mgterminal == GL2PS_TERMINAL) || (renderMode != GL_RENDER) )
    return;
  
  glPushMatrix ();
  glViewport (0, 0, W, H);
  
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluOrtho2D (0.0f, (GLdouble) W, 0.0f, (GLdouble) H);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  
  glDisable (GL_LIGHTING);
  glDisable (GL_DEPTH_TEST);
  
  Htot = (int) (.666 * H);
  step = (int) (Htot / (nb_val_couleurs));
  
  glColor4f (bg_color.r, bg_color.v, bg_color.b, 0.8f);
  glBegin (GL_POLYGON);
  glVertex2i (1, 45);
  glVertex2i (110, 45);
  glVertex2i (110, Htot + 45);
  glVertex2i (1, Htot + 45);
  glEnd ();
  
  glBegin (GL_TRIANGLE_STRIP);
  for (ii = 0; ii <= nb_val_couleurs; ii++)
    {
    glColor3f (gradc[ii].r, gradc[ii].v, gradc[ii].b);
    glVertex2i (5, 45 + step * ii);
    glVertex2i (15, 45 + step * ii);
    }
  glEnd ();
  
  step = (int) ((.666 * H) / (nb_val_couleurs + 1));
  init_val = valc_bleu;
  inc_val = (valc_rouge - valc_bleu) / (nb_val_couleurs - 1);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  for (ii = 0; ii < nb_val_couleurs; ii += (nb_val_couleurs / 10.0))
    {
    sprintf ((char *) str, "%+5.4E", (float) (init_val + inc_val * ii));
    glRasterPos2i (17, (GLint) ((step + 45) + ii * step));
    for (j = 0; str[j]; j++)
      glutBitmapCharacter (GLUT_BITMAP_8_BY_13, str[j]);
    }
  glEnable (GL_LIGHTING);
  
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  gluPerspective (45.0f, 1.0f, cut_distance, DISTANCE_MAX);
  glMatrixMode (GL_MODELVIEW);
  
  if (W < H)
    glViewport ((GLint)((W - H)*0.5), 0, H, H);
  else
    glViewport (0, (GLint)((H - W)*0.5), W, W);
  
  glPopMatrix ();
  
  if (mode2D == MG_FALSE)
    glEnable (GL_DEPTH_TEST);
}


