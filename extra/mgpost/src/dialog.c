/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/

/* Basic homemade Widgets */

void dialogMode ()
{
  glutDisplayFunc (dial_display);
  glutReshapeFunc (NULL);
  glutKeyboardFunc (dial_keyboard);
  glutSpecialFunc (dial_specialKey);
  glutMouseFunc (dial_mouse);
  glutMotionFunc (NULL);
  glutIdleFunc (NULL);
  dialOpened = MG_TRUE;
}

void normalMode ()
{
  glutDisplayFunc (affichage);
  glutReshapeFunc (redim);
  glutMouseFunc (souris);
  glutKeyboardFunc (clavier);
  glutSpecialFunc (specialKey);
  glutMotionFunc (mouvement);
  glutIdleFunc (NULL);
  dialOpened = MG_FALSE;
}

void dial_specialKey (int touche, int x, int y)
{
  if (editedField == 0) return;
  switch (touche)
    {
    case GLUT_KEY_DOWN:
    case GLUT_KEY_UP:

      dialArea[editedField].label[curspos + 1]='\0';
      strcpy (dialArea[editedField].state, "NONE");
      editedField = 0;
      break;

    case GLUT_KEY_LEFT:

      if (curspos > 0) --(curspos);
      break;

    case GLUT_KEY_RIGHT:

      if (curspos < strlen(dialArea[editedField].label)) ++(curspos);
      break;
    }

  glutPostRedisplay ();
}

void dial_keyboard (unsigned char touche, int x, int y)
{
  int i;

  if (editedField == 0) return;

  switch (touche)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '.':

      dialArea[editedField].label[curspos]= (char) touche;
      if (curspos<9) curspos++;
      break;

    case ' ':

      for (i = curspos ; i < 10 ; i++)
        dialArea[editedField].label[i]= '\0';
      break;

    }

  glutPostRedisplay ();
}

void dial_display ()
{
  int i;

  glutReshapeWindow (W, H);

  for (i = 0 ; i < nbDialArea ; i++)
    {
      switch (dialArea[i].type)
	{
	case DIAL_WINDOW:

          drawWindow ();
	  break;

	case DIAL_BUTTON:

	  drawButtonText (i);
	  break;

	case DIAL_CHECKBOX:

	  drawCheckBox (i);
	  break;

	case DIAL_GETTEXT:

	  drawGetText (i);
	  break;

	case DIAL_LABEL:

	  drawLabel (i);
	  break;

	default:
	  break;
	}
    }

  glFlush ();
  glutSwapBuffers ();

}

void dial_mouse (int bouton, int state, int x, int y)
{
  int i;
  unsigned char noFieldSelected;

  if (bouton == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
      noFieldSelected = MG_TRUE;

      for (i = 0 ; i < nbDialArea ; i++)
	{

	  switch (dialArea[i].type)
	    {
	    case DIAL_WINDOW:      
	      break;

	    case DIAL_BUTTON:

	      if (x > dialArea[i].x1 && x < dialArea[i].x2 
               && y > (H - dialArea[i].y2) && y < (H - dialArea[i].y1)) 
		{
		  strcpy (dialArea[i].state, "SELECTED");
		  glutPostRedisplay ();
		}
	      break;

	    case DIAL_CHECKBOX:

	      if (x > dialArea[i].x1 && x < dialArea[i].x2 
               && y > (H - dialArea[i].y2) && y < (H - dialArea[i].y1)) 
		{
                  if (!strcmp(dialArea[i].state,"SELECTED"))
		    strcpy (dialArea[i].state, "UNSELECTED");
		  else
		    strcpy (dialArea[i].state, "SELECTED");
		  glutPostRedisplay ();
		}
	      break;

	    case DIAL_GETTEXT:

	      if (x > dialArea[i].x1 && x < dialArea[i].x2 
		  && y > (H - dialArea[i].y2) && y < (H - dialArea[i].y1)) 
		{
		  strcpy (dialArea[i].state, "EDITION");
		  editedField = i;
		  noFieldSelected = MG_FALSE;
		  curspos = strlen(dialArea[i].label);
		  if (curspos > 9) curspos = 9;
		  glutPostRedisplay ();
		}
	      else
		{
		  strcpy (dialArea[i].state, "NONE");

		  if (noFieldSelected) editedField = 0;

		  glutPostRedisplay ();
		}
	      break;

	    default:
	      break;
	    }
	}

    }

  if (!strcmp(dialArea[1].state,"SELECTED"))
    {
      normalMode();
      closeDialog ();
      glutPostRedisplay();
    }

  processDialog ();
}


void openDialog (GLint x1, GLint y1, GLint x2, GLint y2)
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
  glDisable (GL_POINT_SMOOTH);
  glDisable (GL_LINE_SMOOTH);

  dialArea = (DIAL_AREA*) calloc (NB_DIAL_AREA_MAX, sizeof (DIAL_AREA)); 

  dialArea[0].type = DIAL_WINDOW;
  dialArea[0].x1 = x1;
  dialArea[0].x2 = x2;
  dialArea[0].y1 = y1;
  dialArea[0].y2 = y2;
  nbDialArea = 1;

  creatButtonText (x2 - 22, y1, "OK");

  glColor4f (bg_color.r, bg_color.v, bg_color.b, 0.5f);
  glBegin (GL_POLYGON);
  glVertex2i (0, 0);
  glVertex2i (W, 0);
  glVertex2i (W, H);
  glVertex2i (0, H);
  glEnd ();
}

void closeDialog ()
{
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

  glEnable (GL_POINT_SMOOTH);
  glEnable (GL_LINE_SMOOTH);

  free (dialArea);

}

void drawWindow ()
{
  glColor3f (bg_color.r, bg_color.v, bg_color.b);
  glBegin (GL_POLYGON);
  glVertex2i (dialArea[0].x1, dialArea[0].y1);
  glVertex2i (dialArea[0].x2, dialArea[0].y1);
  glVertex2i (dialArea[0].x2, dialArea[0].y2);
  glVertex2i (dialArea[0].x1, dialArea[0].y2);
  glEnd ();

  glLineWidth(1.0f);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glBegin (GL_LINE_LOOP);
  glVertex2i (dialArea[0].x1, dialArea[0].y1);
  glVertex2i (dialArea[0].x2, dialArea[0].y1);
  glVertex2i (dialArea[0].x2, dialArea[0].y2);
  glVertex2i (dialArea[0].x1, dialArea[0].y2);
  glEnd ();
}

void creatGetText (GLint x, GLint y, const char *label)
{
  int i;

  i = nbDialArea;
  strcpy(dialArea[i].label, label);
  strcpy(dialArea[i].state,"NONE");
  dialArea[i].type = DIAL_GETTEXT;
  dialArea[i].x1 = x;
  dialArea[i].x2 = x + 9 * 10 + 4;
  dialArea[i].y1 = y;
  dialArea[i].y2 = y + 9 + 4;
  nbDialArea++;
}

void drawGetText (int n)
{
  int i;

  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  if (!strcmp(dialArea[n].state,"EDITION"))
    glLineWidth(2.0f);
  else
    glLineWidth(1.0f);

  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glBegin (GL_LINE_LOOP);
  glVertex2i (dialArea[n].x1, dialArea[n].y1);
  glVertex2i (dialArea[n].x2, dialArea[n].y1);
  glVertex2i (dialArea[n].x2, dialArea[n].y2);
  glVertex2i (dialArea[n].x1, dialArea[n].y2);
  glEnd ();

  glRasterPos2i (dialArea[n].x1 + 2, dialArea[n].y1 + 2);

  for (i = 0; dialArea[n].label[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, dialArea[n].label[i]);

  if (!strcmp(dialArea[n].state,"EDITION"))
    {
      glLineWidth(1.0f);
      glBegin (GL_LINE_LOOP);
      glVertex2i (dialArea[n].x1 + curspos * 9 + 1, dialArea[n].y1 + 1);
      glVertex2i (dialArea[n].x1 + (curspos+1) * 9 + 1, dialArea[n].y1 + 1);
      glVertex2i (dialArea[n].x1 + (curspos+1) * 9 + 1, dialArea[n].y2 - 1);
      glVertex2i (dialArea[n].x1 + curspos * 9 + 1, dialArea[n].y2 - 1);
      glEnd ();
    }
}

void creatLabel (GLint x, GLint y, const char *label)
{
  int i;

  i = nbDialArea;
  strcpy(dialArea[i].label, label);
  strcpy(dialArea[i].state,"NONE");
  dialArea[i].type = DIAL_LABEL;
  dialArea[i].x1 = x;
  dialArea[i].y1 = y;
  nbDialArea++;
}

void drawLabel (int n)
{
  int i;

  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  glRasterPos2i (dialArea[n].x1 + 2 , dialArea[n].y1 + 2);

  for (i = 0; dialArea[n].label[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, dialArea[n].label[i]);

}

void creatButtonText (GLint x, GLint y, const char *label)
{
  int i;

  i = nbDialArea;
  strcpy(dialArea[i].label, label);
  strcpy(dialArea[i].state,"NONE");
  dialArea[i].type = DIAL_BUTTON;
  dialArea[i].x1 = x;
  dialArea[i].x2 = x + 9 * strlen(dialArea[i].label) + 4;
  dialArea[i].y1 = y;
  dialArea[i].y2 = y + 9 + 4;
  nbDialArea++;
}

void drawButtonText (int n)
{
  int i;

  glColor3f (fg_color.r, fg_color.v, fg_color.b);

  glLineWidth(1.0f);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glBegin (GL_LINE_LOOP);
  glVertex2i (dialArea[n].x1, dialArea[n].y1);
  glVertex2i (dialArea[n].x2, dialArea[n].y1);
  glVertex2i (dialArea[n].x2, dialArea[n].y2);
  glVertex2i (dialArea[n].x1, dialArea[n].y2);
  glEnd ();

  glRasterPos2i (dialArea[n].x1 + 2 , dialArea[n].y1 + 2);

  for (i = 0; dialArea[n].label[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, dialArea[n].label[i]);

}

void creatCheckBox (GLint x, GLint y, const char *label, unsigned char checked)
{
  int i;

  i = nbDialArea;
  strcpy(dialArea[i].label, label);

  if (checked)
    strcpy(dialArea[i].state, "SELECTED");
  else
    strcpy(dialArea[i].state, "UNSELECTED");

  dialArea[i].type = DIAL_CHECKBOX;
  dialArea[i].x1 = x;
  dialArea[i].x2 = x + 9 * strlen(dialArea[i].label) + 4 + 13 ;
  dialArea[i].y1 = y;
  dialArea[i].y2 = y + 9 + 4;
  nbDialArea++;
}

void drawCheckBox (int n)
{
  int i;

  glLineWidth(1.0f);
  glColor3f (fg_color.r, fg_color.v, fg_color.b);
  glBegin (GL_LINE_LOOP);
  glVertex2i (dialArea[n].x1, dialArea[n].y1);
  glVertex2i (dialArea[n].x1+13, dialArea[n].y1);
  glVertex2i (dialArea[n].x1+13, dialArea[n].y1+13);
  glVertex2i (dialArea[n].x1, dialArea[n].y1+13);
  glEnd ();

  if (!strcmp(dialArea[n].state, "SELECTED"))
    {
      glBegin (GL_LINES);
      glVertex2i (dialArea[n].x1, dialArea[n].y1);
      glVertex2i (dialArea[n].x1+13, dialArea[n].y1+13);

      glVertex2i (dialArea[n].x1+13, dialArea[n].y1);
      glVertex2i (dialArea[n].x1, dialArea[n].y1+13);
      glEnd ();
    }

  glRasterPos2i (dialArea[n].x1 + 15, dialArea[n].y1 + 2);

  for (i = 0; dialArea[n].label[i]; i++)
    glutBitmapCharacter (GLUT_BITMAP_9_BY_15, dialArea[n].label[i]);

}


