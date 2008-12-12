/*************************************************************************
*  Copyright (C) 2008 by Vincent Richefeu                                *
*  vincent.richefeu@hmg.inpg.fr                                          *
*                                                                        *
*  This program is free software; it is licensed under the terms of the  *
*  GNU General Public License v2 or later. See file LICENSE for details. *
*************************************************************************/
typedef struct	  
{
  char label[40];
  char state[30];
  GLint x1, y1, x2, y2;
  unsigned char type;
} DIAL_AREA ;

DIAL_AREA *dialArea;
int nbDialArea = 0;
#define NB_DIAL_AREA_MAX 40

#define DIAL_WINDOW    0
#define DIAL_LABEL     1
#define DIAL_BUTTON    2
#define DIAL_GETTEXT   3
#define DIAL_CHECKBOX  4

void dialogMode ();
void normalMode ();

void (*processDialog) (void);

void dial_display ();
void dial_mouse (int bouton, int state, int x, int y);
void dial_keyboard (unsigned char touche, int x, int y);
void dial_specialKey (int touche, int x, int y);

unsigned char dialOpened = MG_FALSE;
void openDialog (GLint x1, GLint y1, GLint x2, GLint y2);
void closeDialog ();

void creatButtonText (GLint x, GLint y, const char *label);
void creatCheckBox (GLint x, GLint y, const char *label, unsigned char checked);
void creatGetText (GLint x, GLint y, const char *label);
void creatLabel (GLint x, GLint y, const char *label);

void drawWindow ();
void drawButtonText (int n);
void drawCheckBox (int n);
void drawGetText (int n);
void drawLabel (int n);

int editedField = 0;
int curspos = 0;

